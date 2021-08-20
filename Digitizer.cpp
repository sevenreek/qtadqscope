#include "Digitizer.h"
#include "util.h"
Acquisition Digitizer::getAcquisition() const
{
    return defaultAcquisition;
}

void Digitizer::changeDigitizerState(Digitizer::DIGITIZER_STATE newState)
{
    this->currentState = newState;
    emit this->digitizerStateChanged(newState);
}

CalibrationTable Digitizer::getDefaultCalibrationTable() const
{
    return defaultCalibrationTable;
}

void Digitizer::setDefaultCalibrationTable(const CalibrationTable &value)
{
    defaultCalibrationTable = value;
}

bool Digitizer::isStreamFullyStopped()
{
    return this->dmaChecker->isLoopStopped() && this->bufferProcessorHandler->isLoopStopped();
}

void Digitizer::joinThreads()
{
    this->dmaChecker->stopLoop();
    this->bufferProcessorHandler->stopLoop();
    this->ADQThread.quit();
    this->bufferProcessingThread.quit();
    this->ADQThread.wait();
    this->bufferProcessingThread.wait();
}

bool Digitizer::configureAcquisition(Acquisition &acq, std::list<IRecordProcessor*> &recordProcessors, CalibrationTable &calibrations)
{
    this->recordProcessors = recordProcessors;
    this->adq.StopStreaming();
    acq.log();
    if(!this->adq.SetClockSource(acq.getClockSource())) {spdlog::error("SetClockSource failed."); return false;};
    if(!this->adq.SetTriggerMode(acq.getTriggerMode())) {spdlog::error("SetTriggerMode failed."); return false;};
    if(!this->adq.SetSampleSkip(acq.getSampleSkip())) {spdlog::error("SetSampleSkip failed."); return false;};
    for(int i = 0; i<2; i++)
    {
        if(acq.getUserLogicBypassMask() & (1<<i))
        {
            if(!this->adq.BypassUserLogic(i+1, 1)) {spdlog::error("BypassUserLogic failed"); return false;};
        }
        else
        {
            if(!this->adq.BypassUserLogic(i+1, 0)) {spdlog::error("BypassUserLogic failed."); return false;};
        }
    }
    float res;
    for(int ch = 0; ch<MAX_NOF_CHANNELS; ch++)
    {
        if(acq.getChannelMask() & (1<<ch))
        {
            int adqch = ch + 1;
            if(!this->adq.SetInputRange(
                adqch,
                acq.getDesiredInputRange(ch),
                &res)
            ) {spdlog::error("SetInputRange failed."); return false;};
            acq.setObtainedInputRange(ch, res);
            acq.setAnalogOffset(ch, calibrations.analogOffset[ch][acq.getInputRange(ch)]);
            acq.setDigitalOffset(ch, calibrations.digitalOffset[ch][acq.getInputRange(ch)]);
            int unclippedDCShift;
            int clippedDCShift = acq.getTotalDCShift(ch, unclippedDCShift);
            if(clippedDCShift != unclippedDCShift) spdlog::warn("DC shift was clipped.");
            if(!this->adq.SetAdjustableBias(
                adqch,
                clippedDCShift)
            ) {spdlog::error("SetAdjustableBias failed."); return false;};
            if(!this->adq.SetGainAndOffset(
                adqch,
                acq.getDigitalGain(ch),
                acq.getDigitalOffset(ch)
            )) {spdlog::error("SetGainAndOffset failed."); return false;};
        }
    }
    //spdlog::debug("Trying setinputrange ch{}={}", adqChannelIndex, providedConfig->getCurrentChannelConfig().inputRangeFloat);

    if(!this->adq.SetTransferBuffers(
        acq.getTransferBufferCount(),
        acq.getTransferBufferSize())
    ) {spdlog::critical("SetTransferBuffers failed."); return false;};
    this->bufferProcessorHandler->changeStreamingType(acq.getIsContinuous());

    if(acq.getIsContinuous()) // continuous
    {
        /*
        this->bufferProcessor->reallocateBuffers(
                providedConfig.deviceConfig.transferBufferSize/sizeof(short)
        );*/
        spdlog::info("Configuring continuous streaming with mask {:#b}.", acq.getChannelMask());
        if(!this->adq.ContinuousStreamingSetup(acq.getChannelMask())) {spdlog::error("ContinuousStreamingSetup failed."); return false;};

    }
    else
    {
        this->bufferProcessor->reallocateBuffers(acq.getRecordLength());
        spdlog::info("Configuring triggered streaming.");
        if(!this->adq.SetPreTrigSamples(acq.getPretrigger())) {spdlog::error("SetPreTrigSamples failed."); return false;};
        if(!this->adq.SetLvlTrigLevel(acq.getTriggerLevel())) {spdlog::error("SetLvlTrigLevel failed."); return false;};
        if(!this->adq.SetLvlTrigChannel(acq.getTriggerMask())) {spdlog::error("SetLvlTrigChannel failed."); return false;};
        if(!this->adq.SetLvlTrigEdge(acq.getTriggerEdge())) {spdlog::error("SetLvlTrigEdge failed"); return false;};
        if(!this->adq.TriggeredStreamingSetup(
            acq.getRecordCount(),
            acq.getRecordLength(),
            acq.getPretrigger(),
            acq.getTriggerDelay(),
            acq.getChannelMask()
        )) {spdlog::error("TriggeredStreamingSetup failed."); return false;};
    }
    this->writeBuffers.reconfigure(
        acq.getTransferBufferCount(),
        acq.getTransferBufferSize(),
        acq.getChannelMask(),
        acq.getRecordLength()
    );
    this->dmaChecker->setTransferBufferCount(acq.getTransferBufferCount());
    this->bufferProcessor->resetRecordsToStore(acq.getRecordCount()==Acquisition::INFINITE_RECORDS?0:acq.getRecordCount());
    for(auto rp : this->recordProcessors)
    {
        rp->startNewAcquisition(acq);
    }
    spdlog::info("Configured acquisition successfully.");
    this->dmaChecker->setTransferBufferCount(this->getTransferBufferCount());
    return true;
}

void Digitizer::finishRecordProcessors()
{
    for(auto rp : this->recordProcessors)
    {
        rp->finish();
    }
}

void Digitizer::handleAcquisitionFullyStopped()
{
    this->finishRecordProcessors();
    this->changeDigitizerState(DIGITIZER_STATE::READY);
}

Digitizer::Digitizer(ADQInterfaceWrapper &digitizerWrapper) :
    defaultRecordProcessors(),
    recordProcessors(defaultRecordProcessors),
    adq(digitizerWrapper),
    defaultAcquisition(),
    writeBuffers(
        defaultAcquisition.getTransferBufferCount(),
        defaultAcquisition.getTransferBufferSize(),
        defaultAcquisition.getChannelMask(),
        defaultAcquisition.getRecordLength()
        ),
    bufferProcessor(new BaseBufferProcessor(recordProcessors, defaultAcquisition.getRecordLength())),
    bufferProcessorHandler(new LoopBufferProcessor(this->writeBuffers, *this->bufferProcessor.get())),
    dmaChecker(new DMAChecker(this->writeBuffers, this->adq, this->defaultAcquisition.getTransferBufferCount())),
    bufferProcessingThread(),
    ADQThread()
{
    this->bufferProcessorHandler->moveToThread(&this->bufferProcessingThread);
    this->dmaChecker->moveToThread(&this->ADQThread);
    connect(this, &Digitizer::acquisitionStarted, bufferProcessorHandler.get(), &LoopBufferProcessor::runLoop, Qt::ConnectionType::QueuedConnection);
    connect(bufferProcessorHandler.get(), &LoopBufferProcessor::onLoopStopped, this, &Digitizer::loopStopped);
    connect(this, &Digitizer::acquisitionStarted, dmaChecker.get(), &DMAChecker::runLoop, Qt::ConnectionType::QueuedConnection);
    connect(dmaChecker.get(), &DMAChecker::onLoopStopped, this, &Digitizer::loopStopped);

    this->bufferProcessingThread.start();
    this->ADQThread.start();
    this->acquisitionTimer.setSingleShot(true);
    connect(&this->acquisitionTimer, &QTimer::timeout, this, &Digitizer::stopAcquisition);
}

Digitizer::~Digitizer()
{
    this->joinThreads();
}

bool Digitizer::stopAcquisition()
{
    if(this->currentState == DIGITIZER_STATE::READY) return false;
    else if(this->currentState == DIGITIZER_STATE::STOPPING && this->isStreamFullyStopped())
    {
        this->handleAcquisitionFullyStopped();
        return true;
    }
    this->changeDigitizerState(DIGITIZER_STATE::STOPPING);
    this->dmaChecker->stopLoop();
    this->bufferProcessorHandler->stopLoop();
    if(this->isStreamFullyStopped())
    {
        this->handleAcquisitionFullyStopped();
    }
    return true;
}

bool Digitizer::runAcquisition()
{
    return this->runOverridenAcquisition(this->defaultAcquisition, this->defaultRecordProcessors, this->defaultCalibrationTable);
}

void Digitizer::loopStopped()
{
    this->stopAcquisition();
}

bool Digitizer::runOverridenAcquisition(Acquisition &acq, std::list<IRecordProcessor*> &recordProcessors, CalibrationTable &calibrations)
{
    if(this->currentState != DIGITIZER_STATE::READY) return false;
    if(!this->configureAcquisition(acq, recordProcessors, calibrations)) goto START_FAILED;
    this->changeDigitizerState(DIGITIZER_STATE::ACTIVE);
    if(!this->adq.StartStreaming())
    {
        goto START_FAILED;
    }
    if(acq.getIsContinuous()) this->adq.SWTrig();
    emit this->acquisitionStarted();
    if(acq.getDuration())
    {
        this->acquisitionTimer.setInterval(acq.getDuration());
        this->acquisitionTimer.start();
    }
    spdlog::info("Acquisition started!");
    return true;
START_FAILED:
    this->stopAcquisition();
    spdlog::error("Stream failed to start!");
    this->acquisitionTimer.stop();
    return false;
}

bool Digitizer::setAcquisition(const Acquisition acq)
{
    this->defaultAcquisition = acq;
    for(int ch=0; ch< MAX_NOF_CHANNELS; ch++)
    {
        this->setInputRange(ch, this->getInputRange(ch));
    }
    return true;
}

void Digitizer::appendRecordProcessor(IRecordProcessor *rp)
{
    this->defaultRecordProcessors.push_back(rp);
}

void Digitizer::removeRecordProcessor(IRecordProcessor *rp)
{
    this->defaultRecordProcessors.remove(rp);
}

Digitizer::DIGITIZER_STATE Digitizer::getDigitizerState()
{
    return this->currentState;
}

Digitizer::DIGITIZER_TRIGGER_MODE Digitizer::getTriggerMode()
{
    if(this->defaultAcquisition.getIsContinuous()) return DIGITIZER_TRIGGER_MODE::CONTINUOUS;
    switch(this->defaultAcquisition.getTriggerMode())
    {
        case TRIGGER_MODES::SOFTWARE:   return DIGITIZER_TRIGGER_MODE::SOFTWARE;
        case TRIGGER_MODES::LEVEL:      return DIGITIZER_TRIGGER_MODE::LEVEL;
        case TRIGGER_MODES::INTERNAL:   return DIGITIZER_TRIGGER_MODE::INTERNAL;
        case TRIGGER_MODES::EXTERNAL:   return DIGITIZER_TRIGGER_MODE::EXTERNAL;
        case TRIGGER_MODES::EXTERNAL_2: return DIGITIZER_TRIGGER_MODE::EXTERNAL2;
        case TRIGGER_MODES::EXTERNAL_3: return DIGITIZER_TRIGGER_MODE::EXTERNAL3;
    }
    return DIGITIZER_TRIGGER_MODE::CONTINUOUS;
}

unsigned long Digitizer::getDuration()
{
    return this->defaultAcquisition.getDuration();
}

unsigned long Digitizer::getTransferBufferSize()
{
    return this->defaultAcquisition.getTransferBufferSize();
}

unsigned long Digitizer::getTransferBufferCount()
{
    return this->defaultAcquisition.getTransferBufferCount();
}

unsigned long Digitizer::getTransferBufferQueueSize()
{
    return this->defaultAcquisition.getTransferBufferQueueSize();
}

unsigned long long Digitizer::getFileSizeLimit()
{
    return this->defaultAcquisition.getFileSizeLimit();
}

unsigned char Digitizer::getUserLogicBypass()
{
    return this->defaultAcquisition.getUserLogicBypassMask();
}

CLOCK_SOURCES Digitizer::getClockSource()
{
    return this->defaultAcquisition.getClockSource();
}

TRIGGER_EDGES Digitizer::getTriggerEdge()
{
    return this->defaultAcquisition.getTriggerEdge();
}

unsigned char Digitizer::getTriggerMask()
{
    return this->defaultAcquisition.getTriggerMask();
}

int Digitizer::getTriggerLevel()
{
    return this->defaultAcquisition.getTriggerLevel();
}

int Digitizer::getTriggerReset()
{
    return this->defaultAcquisition.getTriggerReset();
}

unsigned short Digitizer::getPretrigger()
{
    return this->defaultAcquisition.getPretrigger();
}

unsigned short Digitizer::getTriggerDelay()
{
    return this->defaultAcquisition.getTriggerDelay();
}

unsigned long Digitizer::getRecordCount()
{
    return this->defaultAcquisition.getRecordCount();
}

unsigned long Digitizer::getRecordLength()
{
    return this->defaultAcquisition.getRecordLength();
}

unsigned char Digitizer::getChannelMask()
{
    return this->defaultAcquisition.getChannelMask();
}

unsigned int Digitizer::getSampleSkip()
{
    return this->defaultAcquisition.getSampleSkip();
}

INPUT_RANGES Digitizer::getInputRange(int ch)
{
    return this->defaultAcquisition.getInputRange(ch);
}

int Digitizer::getDCBias(int ch)
{
    return this->defaultAcquisition.getDcBias(ch);
}

int Digitizer::getDigitalGain(int ch)
{
    return this->defaultAcquisition.getDigitalGain(ch);
}

int Digitizer::getDigitalOffset(int ch, int ir)
{
    return this->defaultCalibrationTable.digitalOffset[ch][ir];
}

int Digitizer::getAnalogOffset(int ch, int ir)
{
    return this->defaultCalibrationTable.analogOffset[ch][ir];
}

double Digitizer::getObtainedRange(int ch)
{
    if(!this->defaultAcquisition.getObtainedInputRange(ch))
        return INPUT_RANGE_VALUES[this->defaultAcquisition.getInputRange(ch)];
    return this->defaultAcquisition.getObtainedInputRange(ch);
}

std::string Digitizer::getAcquisitionTag()
{
    return this->defaultAcquisition.getTag();
}

unsigned long Digitizer::getSamplesPerRecordComplete()
{
    if(this->defaultAcquisition.getIsContinuous())
    {
        return this->defaultAcquisition.getTransferBufferSize()/sizeof(short);
    }
    else
    {
        return this->defaultAcquisition.getRecordLength();
    }
}


unsigned long long Digitizer::getLastBuffersFill()
{
    return this->dmaChecker->getLastFilledBufferCount();
}

unsigned long long Digitizer::getQueueFill()
{
    return this->writeBuffers.getReadCount();
}

void Digitizer::setTriggerMode(Digitizer::DIGITIZER_TRIGGER_MODE triggerMode)
{
    switch(triggerMode)
    {
        case DIGITIZER_TRIGGER_MODE::CONTINUOUS: this->defaultAcquisition.setTriggerMode(TRIGGER_MODES::SOFTWARE); this->defaultAcquisition.setIsContinuous(true );   break;
        case DIGITIZER_TRIGGER_MODE::SOFTWARE:   this->defaultAcquisition.setTriggerMode(TRIGGER_MODES::SOFTWARE); this->defaultAcquisition.setIsContinuous(false);  break;
        case DIGITIZER_TRIGGER_MODE::LEVEL:      this->defaultAcquisition.setTriggerMode(TRIGGER_MODES::LEVEL   ); this->defaultAcquisition.setIsContinuous(false);  break;
        case DIGITIZER_TRIGGER_MODE::INTERNAL:   this->defaultAcquisition.setTriggerMode(TRIGGER_MODES::INTERNAL); this->defaultAcquisition.setIsContinuous(false);  break;
        case DIGITIZER_TRIGGER_MODE::EXTERNAL:   this->defaultAcquisition.setTriggerMode(TRIGGER_MODES::EXTERNAL); this->defaultAcquisition.setIsContinuous(false);  break;
        case DIGITIZER_TRIGGER_MODE::EXTERNAL2:  this->defaultAcquisition.setTriggerMode(TRIGGER_MODES::EXTERNAL_2); this->defaultAcquisition.setIsContinuous(false); break;
        case DIGITIZER_TRIGGER_MODE::EXTERNAL3:  this->defaultAcquisition.setTriggerMode(TRIGGER_MODES::EXTERNAL_3); this->defaultAcquisition.setIsContinuous(false); break;
    }
    emit this->triggerModeChanged(triggerMode);
}

void Digitizer::setDuration(unsigned long duration)
{
    this->defaultAcquisition.setDuration(duration);
    emit this->durationChanged(duration);
}

void Digitizer::setTransferBufferSize(unsigned long bufferSize)
{
    this->defaultAcquisition.setTransferBufferSize(bufferSize);
    emit this->transferBufferSizeChanged(bufferSize);
}

void Digitizer::setTransferBufferCount(unsigned long bufferCount)
{
    this->defaultAcquisition.setTransferBufferCount(bufferCount);
    emit this->transferBufferCountChanged(bufferCount);
}

void Digitizer::setTransferBufferQueueSize(unsigned long queueSize)
{
    this->defaultAcquisition.setTransferBufferQueueSize(queueSize);
    emit this->transferBufferQueueSizeChanged(queueSize);
}

void Digitizer::setFileSizeLimit(unsigned long long size)
{
    this->defaultAcquisition.setFileSizeLimit(size);
    emit this->fileSizeLimitChanged(size);
}

void Digitizer::setUserLogicBypass(unsigned char ulBypass)
{
    this->defaultAcquisition.setUserLogicBypassMask(ulBypass);
    for(int ul=0; ul < 2; ul++)
    {
        this->adq.BypassUserLogic(ul+1, (ulBypass&(1<<ul))?1:0);
    }
    emit this->userLogicBypassChanged(ulBypass);
}

void Digitizer::setClockSource(CLOCK_SOURCES clockSource)
{
    this->defaultAcquisition.setClockSource(clockSource);
    emit this->clockSourceChanged(clockSource);
}

void Digitizer::setTriggerEdge(TRIGGER_EDGES edge)
{
    this->defaultAcquisition.setTriggerEdge(edge);
    this->adq.SetLvlTrigEdge(edge);
    emit this->triggerEdgeChanged(edge);
}

void Digitizer::setTriggerMask(unsigned char mask)
{
    this->defaultAcquisition.setTriggerMask(mask);
    emit this->triggerMaskChanged(mask);
}

void Digitizer::setTriggerLevel(int lvl)
{
    int clipped = clip(lvl, CODE_MIN, CODE_MAX);
    if(clipped != lvl) spdlog::warn("Trigger level clipped to {} from {}.", clipped, lvl);
    this->defaultAcquisition.setTriggerLevel(clipped);
    this->adq.SetLvlTrigLevel(clipped);
    emit this->triggerLevelChanged(clipped);
}

void Digitizer::setTriggerReset(int rst)
{
    this->defaultAcquisition.setTriggerReset(rst);
    this->adq.SetTrigLevelResetValue(rst);
    emit this->triggerResetChanged(rst);
}

void Digitizer::setPretrigger(unsigned short pretrigger)
{
    this->defaultAcquisition.setPretrigger(pretrigger);
    emit this->pretriggerChanged(pretrigger);
}

void Digitizer::setTriggerDelay(unsigned short delay)
{
    this->defaultAcquisition.setTriggerDelay(delay);
    emit this->triggerDelayChanged(delay);
}

void Digitizer::setRecordCount(unsigned long count)
{
    this->defaultAcquisition.setRecordCount(count);
    emit this->recordCountChanged(count);
}

void Digitizer::setRecordLength(unsigned long length)
{
    this->defaultAcquisition.setRecordLength(length);
    emit this->recordLengthChanged(length);
}

void Digitizer::setChannelMask(unsigned char mask)
{
    this->defaultAcquisition.setChannelMask(mask);
    emit this->channelMaskChanged(mask);
}

void Digitizer::setSampleSkip(unsigned int sampleSkip)
{
    this->defaultAcquisition.setSampleSkip(sampleSkip);
    emit this->sampleSkipChanged(sampleSkip);
}

void Digitizer::setInputRange(int ch, INPUT_RANGES range)
{
    this->defaultAcquisition.setInputRange(ch, range);
    float obtained;
    this->adq.SetInputRange(ch+1, INPUT_RANGE_VALUES[range], &obtained);
    this->defaultAcquisition.setObtainedInputRange(ch, obtained);
    emit this->inputRangeChanged(ch, range);
}

void Digitizer::setDCBias(int ch, int bias)
{
    int clipped = clip(bias, CODE_MIN, CODE_MAX);
    if(clipped != bias) spdlog::warn("DC bias for CH{} clipped to {} from {}.", ch+1, clipped, bias);
    this->defaultAcquisition.setDcBias(ch, clipped);
    this->adq.SetAdjustableBias(ch+1, this->defaultAcquisition.getTotalDCShift(ch, bias));
    emit this->dcBiasChanged(ch, clipped);
}

void Digitizer::setDigitalGain(int ch, int gain)
{

    this->defaultAcquisition.setDigitalGain(ch, gain);
    emit this->digitalGainChanged(ch, gain);
}

void Digitizer::setDigitalOffset(int ch, int ir, int offset)
{
    int clipped = clip(offset, CODE_MIN, CODE_MAX);
    if(clipped != offset) spdlog::warn("DC digital offset for CH{} clipped to {} from {}.", ch+1, clipped, offset);
    this->defaultCalibrationTable.digitalOffset[ch][ir] = clipped;
    if(ir == this->defaultAcquisition.getInputRange(ch))
    {
        this->defaultAcquisition.setDigitalOffset(ch, offset);
        emit this->digitalOffsetChanged(ch, offset);
    }
}

void Digitizer::setAnalogOffset(int ch, int ir, int offset)
{
    int clipped = clip(offset, CODE_MIN, CODE_MAX);
    if(clipped != offset) spdlog::warn("DC analog offset for CH{} clipped to {} from {}.", ch+1, clipped, offset);
    this->defaultCalibrationTable.analogOffset[ch][ir] = clipped;
    if(ir == this->defaultAcquisition.getInputRange(ch))
    {
        this->defaultAcquisition.setAnalogOffset(ch, clipped);
        this->adq.SetAdjustableBias(ch+1, this->defaultAcquisition.getTotalDCShift(ch, offset));
        emit this->analogOffsetChanged(ch, clipped);
    }
}

void Digitizer::setAcquisitionTag(string tag)
{
    this->defaultAcquisition.setTag(tag);
    emit this->acquisitionTagChanged(tag);
}
