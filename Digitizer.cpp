#include "Digitizer.h"
#include "util.h"
#include "RegisterConstants.h"
const unsigned long Digitizer::ACQUISITION_DELAY = 2500;
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

void Digitizer::SWTrig()
{
    this->adq.SWTrig();
}

void Digitizer::setDefaultCalibrationTable(const CalibrationTable &value)
{
    defaultCalibrationTable = value;
}

bool Digitizer::isStreamFullyStopped()
{
    return this->bufferProcessorHandler->getLoopState() == BufferProcessor::STATE::INACTIVE ||
           this->bufferProcessorHandler->getLoopState() == BufferProcessor::STATE::SERROR;
}

void Digitizer::joinThreads()
{
    this->bufferProcessorHandler->stop();
    this->ADQThread.quit();
    this->ADQThread.wait();
}

Digitizer::TriggerConfiguration Digitizer::createTriggerConfig(Acquisition *acq)
{
    TriggerConfiguration tcfg;
    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        tcfg.levelArray[ch] = acq->getTriggerLevel();
        tcfg.resetArray[ch] = acq->getTriggerReset();

       // if(this->getTriggerMask()&(1<<ch))
        {
            tcfg.edgeArray[ch] = static_cast<ADQEdge>(acq->getTriggerEdge());
            tcfg.sourceArray[ch] = static_cast<ADQEventSource>(acq->getTriggerMode());
        }
    }


    return tcfg;
}
bool Digitizer::configureAcquisition(
        Acquisition *acq, std::list<IRecordProcessor*> &recordProcessors, CalibrationTable &calibrations
)
{
    int result;
    this->recordProcessors = recordProcessors;
    this->adq.StopDataAcquisition();
    acq->log();
    //if(!this->adq.SetClockSource(acq.getClockSource())) {spdlog::error("SetClockSource failed."); return false;};

    for(int i = 0; i<2; i++)
    {
        if(acq->getUserLogicBypassMask() & (1<<i))
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
        if(acq->getChannelMask() & (1<<ch))
        {
            int adqch = ch + 1;
            if(!this->adq.SetInputRange(
                adqch,
                acq->getDesiredInputRange(ch),
                &res)
            ) {spdlog::error("SetInputRange failed."); return false;};
            acq->setObtainedInputRange(ch, res);
            acq->setAnalogOffset(ch, calibrations.analogOffset[ch][acq->getInputRange(ch)]);
            acq->setDigitalOffset(ch, calibrations.digitalOffset[ch][acq->getInputRange(ch)]);
            int unclippedDCShift;
            int clippedDCShift = acq->getTotalDCShift(ch, unclippedDCShift);
            if(clippedDCShift != unclippedDCShift) spdlog::warn("DC shift was clipped.");
            if(!this->adq.SetAdjustableBias(
                adqch,
                clippedDCShift)
            ) {spdlog::error("SetAdjustableBias failed."); return false;};
            if(!this->adq.SetGainAndOffset(
                adqch,
                acq->getDigitalGain(ch),
                acq->getDigitalOffset(ch)
            )) {spdlog::error("SetGainAndOffset failed."); return false;};
        }
    // GEN 3:
        if(!this->adq.SetChannelSampleSkip(ch+1, acq->getSampleSkip())) {spdlog::error("SetChannelSampleSkip failed."); return false;};
    }
    // still GEN 3:
    struct ADQDataAcquisitionParameters acqParams;
    if(this->adq.InitializeParameters(ADQParameterId::ADQ_PARAMETER_ID_DATA_ACQUISITION, &acqParams) != sizeof(acqParams)) // this is an error check
    {spdlog::error("InitializeAcquisitionParameters failed."); return false;};

    TriggerConfiguration triggerConfig = this->createTriggerConfig(acq);
    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        acqParams.channel[ch].trigger_source = static_cast<ADQEventSource>(triggerConfig.sourceArray[ch]);
        acqParams.channel[ch].horizontal_offset = -acq->getPretrigger() + acq->getTriggerDelay();
        acqParams.channel[ch].trigger_edge = static_cast<ADQEdge>(triggerConfig.edgeArray[ch]);

        if(acq->getIsContinuous())
        {
            if(acq->getChannelMask() & (1<<ch)) {
                acqParams.channel[ch].nof_records = ADQ_INFINITE_NOF_RECORDS;
            }
            else {
                acqParams.channel[ch].nof_records = 0; // disables channel
            }
            acqParams.channel[ch].record_length = ADQ_INFINITE_RECORD_LENGTH;
        }
        else
        {
            if(acq->getChannelMask() & (1<<ch)) {
                acqParams.channel[ch].nof_records =
                        acq->getRecordCount()==Acquisition::INFINITE_RECORDS?
                            ADQ_INFINITE_NOF_RECORDS:acq->getRecordCount();
            }
            else {
                acqParams.channel[ch].nof_records = 0; // disables channel
            }
            acqParams.channel[ch].record_length = acq->getRecordLength();
        }

    }
    result = this->adq.SetParameters(&acqParams);
    if(result == ADQ_EINVAL) {spdlog::error("Invalid ADQDataAcquisitionParameters paramaters. Could not configure acquisition."); return false;}
    else if(result == ADQ_EIO) {spdlog::error("Could not configure ADQDataAcquisitionParameters parameters due to an I/O error."); return false;}

    struct ADQDataTransferParameters dtParams;
    if(this->adq.InitializeParameters(ADQParameterId::ADQ_PARAMETER_ID_DATA_TRANSFER, &dtParams) != sizeof(dtParams))
    {spdlog::error("InitializeDataTransferParameters failed."); return false;};
    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        dtParams.channel[ch].nof_buffers = acq->getTransferBufferCount();
        dtParams.channel[ch].record_buffer_size = acq->getTransferBufferSize();
    }
    result = this->adq.SetParameters(&dtParams);
    if(result == ADQ_EINVAL) {spdlog::error("Invalid ADQDataTransferParameters paramaters. Could not configure acquisition."); return false;}
    else if(result == ADQ_EIO) {spdlog::error("Could not configure ADQDataTransferParameters parameters due to an I/O error."); return false;}

    struct ADQDataReadoutParameters roParams;
    if(this->adq.InitializeParameters(ADQParameterId::ADQ_PARAMETER_ID_DATA_READOUT, &roParams) != sizeof(roParams))
    {spdlog::error("InitializeDataReadoutParameters failed."); return false;};
    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        roParams.channel[ch].nof_record_buffers_max = acq->getTransferBufferQueueSize();
        roParams.channel[ch].incomplete_records_enabled = acq->getIsContinuous(); // for continuous streaming there is just one header that never ends
    }
    result = this->adq.SetParameters(&roParams);
    if(result == ADQ_EINVAL) {spdlog::error("Invalid ADQDataReadoutParameters paramaters. Could not configure acquisition."); return false;}
    else if(result == ADQ_EIO) {spdlog::error("Could not configure ADQDataReadoutParameters parameters due to an I/O error."); return false;}
    if(acq->getTriggerMode() == TRIGGER_MODES::LEVEL) {

        if(!this->adq.SetupLevelTrigger(
            triggerConfig.levelArray,
            triggerConfig.edgeArray,
            triggerConfig.resetArray,
            0,//0,// the example uses 0 for some reason, acq.getChannelMask(),
            1 // indvidual mode == true,
        )){spdlog::error("SetupLevelTrigger failed."); return false;}
    }

    this->bufferProcessorHandler->configureNewAcquisition(acq);
    for(auto rp: this->recordProcessors)
    {
        rp->startNewAcquisition(acq);
    }
    this->acquisitionToStart = acq;
    unsigned int retval;
    this->writeUserRegister(UL_TARGET, DC_SHIFT_REGISTER, 0, acq->getDcBias(CHANNEL_SOURCE), &retval);
    spdlog::debug("0x{:X} {}", DC_SHIFT_REGISTER, retval);
    const TimestampSyncConfig &tsSync = acq->getTimestampSyncConfig();
    if(tsSync.rearmOnAcquisitionStart) {
        this->adq.DisarmTimestampSync();
        this->adq.SetupTimestampSync(tsSync.syncOnMultipleEvents, tsSync.source);
    }
    spdlog::info("Configured acquisition successfully.");
    return true;
}

void Digitizer::finishRecordProcessors()
{
    for(auto rp : this->recordProcessors)
    {
        auto processedBytes = rp->finish();
        double dataRate = (processedBytes/1.0e6) / (this->defaultAcquisition.getDuration()/1.0e3);
        if(processedBytes)
            spdlog::info("{} processed {:.4f} GBs. Data rate {:.4f} MB/s.", rp->getName(), processedBytes/1.0e9, dataRate);
    }
}

void Digitizer::handleAcquisitionFullyStopped()
{
    if(this->currentState == DIGITIZER_STATE::READY) return;
    spdlog::info("Completing acquisition.");
    this->finishRecordProcessors();
    this->bufferProcessorHandler->reset();
    if(this->adq.GetStreamOverflow())
    {
        spdlog::warn("Overflow occured during acquisition! Some data might be missing.");
    }
    this->changeDigitizerState(DIGITIZER_STATE::READY);

}

Digitizer::Digitizer(ADQInterfaceWrapper &digitizerWrapper) :
    adq(digitizerWrapper),
    defaultRecordProcessors(),
    recordProcessors(defaultRecordProcessors),
    defaultAcquisition(),
    bufferProcessorHandler(new BufferProcessor(this->recordProcessors, digitizerWrapper)),
    ADQThread()
{
    //qRegisterMetaType<BufferProcessor::STATE>();
    this->bufferProcessorHandler->moveToThread(&this->ADQThread);
    connect(
        this,                           &Digitizer::acquisitionStarted,
        bufferProcessorHandler.get(),   &BufferProcessor::startBufferProcessLoop,
        Qt::ConnectionType::QueuedConnection
    );
    connect(bufferProcessorHandler.get(), &BufferProcessor::stateChanged, this, &Digitizer::processorLoopStateChanged);
    connect(bufferProcessorHandler.get(), &BufferProcessor::ramFillChanged, this, &Digitizer::ramFillChanged);
    this->ADQThread.start();
    this->acquisitionTimer.setSingleShot(true);
    connect(
        &this->acquisitionTimer,    &QTimer::timeout,
        this,                       &Digitizer::stopAcquisition,
        Qt::ConnectionType::QueuedConnection
    );
    this->acquisitionStartDelayTimer.setInterval(Digitizer::ACQUISITION_DELAY);
    this->acquisitionStartDelayTimer.setSingleShot(true);
    this->acquisitionStartDelayTimer.connect(
        &this->acquisitionStartDelayTimer,  &QTimer::timeout,
        this,                               &Digitizer::onAcquisitionDelayEnd
    );
}

Digitizer::~Digitizer()
{
    this->joinThreads();
}

std::chrono::milliseconds Digitizer::getMillisFromLastStarve()
{
    return this->bufferProcessorHandler->getMillisFromLastStarve();
}

float Digitizer::getDeviceRamFillLevel()
{
    return this->bufferProcessorHandler->getRamFillLevel();
}

bool Digitizer::stopAcquisition()
{
    int result;
    this->acquisitionStartDelayTimer.stop();
    this->acquisitionTimer.stop();
    this->bufferProcessorHandler->stop();
    result = this->adq.StopDataAcquisition();
    if(result == ADQ_EAGAIN)
    {
        spdlog::warn("Tried to stop acquisition. Acqusition is not running.");
    }
    else if(result == ADQ_EINTERRUPTED)
    {
        spdlog::warn("Ended acquisition prematurely.");
    }
    else if(result == ADQ_EIO)
    {
        spdlog::error("I/O error while stopping acquisition.");
    }
    if(this->isStreamFullyStopped())
    {
        this->handleAcquisitionFullyStopped();
    }
    else
    {
        this->changeDigitizerState(DIGITIZER_STATE::STOPPING);
    }
    unsigned int retval;
    this->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~ACTIVE_SPECTRUM_BIT, 0, &retval); // disable pha
    return true;
}

bool Digitizer::runAcquisition()
{
    return this->runOverridenAcquisition(
        &this->defaultAcquisition,
        this->defaultRecordProcessors,
        this->defaultCalibrationTable
    );
}
void Digitizer::processorLoopStateChanged(BufferProcessor::STATE newState)
{
    if(newState == BufferProcessor::STATE::SERROR)
    {
        this->stopAcquisition();
    }
    else if(newState == BufferProcessor::STATE::INACTIVE)
    {
        this->handleAcquisitionFullyStopped();
    }
}
void Digitizer::onAcquisitionDelayEnd()
{
    int result;
    result = this->adq.StartDataAcquisition();
    if(result == ADQ_ENOTREADY)
    {
     spdlog::error("Acqusition already active. Cannot start.");
     goto START_FAILED;
    }
    else if(result == ADQ_EINTERRUPTED)
    {
     spdlog::error("Could not start acqusisition thread.");
     goto START_FAILED;
    }
    else if(result != ADQ_EOK)
    {
     spdlog::critical("Could not start acquisition. Reason unknown.");
     goto START_FAILED;
    }
    if(this->acquisitionToStart->getIsContinuous()) this->adq.SWTrig();
    emit this->acquisitionStarted();
    if(this->acquisitionToStart->getDuration())
    {
     this->acquisitionTimer.setInterval(this->acquisitionToStart->getDuration());
     this->acquisitionTimer.start();
    }
    unsigned int retval;

    this->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~ACTIVE_SPECTRUM_BIT, this->acquisitionToStart->getSpectroscopeEnabled()?ACTIVE_SPECTRUM_BIT:0, &retval);
    spdlog::info("Acquisition started!");
    this->changeDigitizerState(DIGITIZER_STATE::ACTIVE);
    return;
START_FAILED:
    this->stopAcquisition();
    spdlog::error("Stream failed to start!");
    this->acquisitionTimer.stop();

}

bool Digitizer::runOverridenAcquisition(
    Acquisition *acq, std::list<IRecordProcessor*> &recordProcessors, CalibrationTable &calibrations
)
{
    if(this->currentState != DIGITIZER_STATE::READY) return false;
    this->changeDigitizerState(DIGITIZER_STATE::STARTING);

    if(!this->configureAcquisition(acq, recordProcessors, calibrations)) return false;
    this->changeDigitizerState(DIGITIZER_STATE::STABILIZING);
    spdlog::debug("Stabilizing... Acquisition will start in {} ms.", Digitizer::ACQUISITION_DELAY);
    if(acq->getTimestampSyncConfig().rearmOnAcquisitionStart) {
        this->adq.ArmTimestampSync();
    }
    this->acquisitionStartDelayTimer.start();
    return true;
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

bool Digitizer::writeUserRegister(unsigned int ul, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int *returval)
{
    return this->adq.WriteUserRegister(ul, regnum, mask, data, returval);
}

bool Digitizer::readBlockUserRegister(unsigned int ul, unsigned int start, unsigned int *data, unsigned int numBytes, unsigned int options)
{
    return this->adq.ReadBlockUserRegister(ul, start, data, numBytes, options);
}

bool Digitizer::readUserRegister(unsigned int ul, unsigned int regnum, unsigned int *returval)
{
    return this->adq.ReadUserRegister(ul, regnum, returval);
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

int Digitizer::durationRemaining()
{
    return this->acquisitionTimer.remainingTime();
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
    return 0;
}

unsigned long long Digitizer::getQueueFill()
{
    return this->bufferProcessorHandler->getRamFillLevel();
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
    /*
    this->adq.SetLvlTrigEdge(edge);
    */
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
    /*
    this->adq.SetLvlTrigLevel(clipped);
    */
    emit this->triggerLevelChanged(clipped);
}

void Digitizer::setTriggerReset(int rst)
{
    this->defaultAcquisition.setTriggerReset(rst);
    /*
    this->adq.SetTrigLevelResetValue(rst);
    */
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

void Digitizer::setAcquisitionTag(std::string tag)
{
    this->defaultAcquisition.setTag(tag);
    emit this->acquisitionTagChanged(tag);
}

void Digitizer::setTriggerApproach(TRIGGER_APPROACHES approach)
{
    this->defaultAcquisition.setTriggerApproach(approach);
}

TRIGGER_APPROACHES Digitizer::getTriggerApproach()
{
    return this->defaultAcquisition.getTriggerApproach();
}
void Digitizer::setSpectroscopeEnabled(bool enabled)
{
    this->defaultAcquisition.setSpectroscopeEnabled(enabled);
}
bool Digitizer::getSpectroscopeEnabled()
{
    return this->defaultAcquisition.getSpectroscopeEnabled();
}

bool Digitizer::setDirectionGPIOPort(unsigned int port, unsigned int direction, unsigned int mask)
{
    return this->adq.SetDirectionGPIOPort(port, direction, mask);
}

bool Digitizer::writeGPIOPort(unsigned int port, unsigned int data, unsigned int mask)
{
    return this->adq.WriteGPIOPort(port, data, mask);
}

void Digitizer::setTimestampSyncConfig(const TimestampSyncConfig &config)
{
    this->defaultAcquisition.setTimestampSyncConfig(config);
}

const TimestampSyncConfig &Digitizer::getTimestampSyncConfig()
{
    return this->defaultAcquisition.getTimestampSyncConfig();
}

unsigned int Digitizer::readGPIOPort(unsigned int port)
{
    unsigned int retval;
    this->adq.ReadGPIOPort(port, &retval);
    return retval;
}
bool Digitizer::enableGPIOSupplyOutput(bool enable)
{
    return this->adq.EnableGPIOSupplyOutput(enable);
}
