#include "qadqdevice.h"


QADQDevice::QADQDevice(void* adqapi, unsigned int deviceNumber, ApplicationConfiguration &conf, QThread *thread):
    deviceNumber(deviceNumber),
    adqCU(adqapi),
    device(std::shared_ptr<ADQInterface>(ADQControlUnit_GetADQ(adqCU, deviceNumber))),
    appConfig(conf),
    writeBuffers(
        conf.transferBufferCount,
        conf.transferBufferSize,
        conf.getChannelMask()
    ),
    acquisition(*this, writeBuffers)
{
    this->device->StopStreaming();
    if(thread != nullptr)
    {
        this->moveToThread(thread);
        acquisition.moveToThread(thread);
    }
    for(int ch=0; ch<MAX_NOF_CHANNELS; ch++)
    {
        float res;
        if(this->device->SetInputRange(ch+1, conf.channelConfig[ch].inputRangeFloat, &res))
        {
            this->appConfig.channelConfig[ch].inputRangeFloat = res;
        }
    }
}

bool QADQDevice::configureAcquisition()
{
    this->overrideAcquisition(this->appConfig);
}


bool QADQDevice::startAcquisition(unsigned int bufferCount, unsigned int dmaFlush, unsigned int duration)
{
    if(!this->configured) return false;
    return this->acquisition.run(bufferCount, dmaFlush, duration);
}

bool QADQDevice::stopAcquisition(bool dumpData)
{
    return this->acquisition.stop(dumpData);
}

QAcquisition::STATES QADQDevice::getAcquisitionState()
{
    return this->acquisition.getState();
}

TRIGGER_MODES QADQDevice::getTriggerMode()
{
    return static_cast<TRIGGER_MODES>(this->device->GetTriggerMode());
}

bool QADQDevice::setTriggerMode(TRIGGER_MODES mode)
{
    return this->device->SetTriggerMode(mode);
}

int QADQDevice::getTriggerLevel()
{
    return this->device->GetLvlTrigLevel();
}

bool QADQDevice::setTriggerLevel(int level)
{
    bool result = this->device->SetLvlTrigLevel(level);
    return result;
}

TRIGGER_EDGES QADQDevice::getTriggerEdge()
{
    return static_cast<TRIGGER_EDGES>(this->device->GetLvlTrigEdge());
}

bool QADQDevice::setTriggerEdge(TRIGGER_EDGES edges)
{
    bool result = this->device->SetLvlTrigEdge(edges);
    return result;
}
bool QADQDevice::setTriggerReset(int reset)
{
    bool result = this->device->SetTrigLevelResetValue(reset);
    return result;
}

unsigned char QADQDevice::getTriggerMask()
{
    return this->device->GetLvlTrigChannel();
}

bool QADQDevice::setTriggerMask(unsigned char mask)
{
    bool result = this->device->SetLvlTrigChannel(mask);
    return result;
}

bool QADQDevice::SWTrig()
{
    return this->device->SWTrig();
}

bool QADQDevice::flushDMA()
{
    return this->device->FlushDMA();
}

unsigned int QADQDevice::getFilledBuffersCount()
{
    unsigned int filled;
    if(this->device->GetTransferBufferStatus(&filled))
        return filled;
    else return 0;
}

bool QADQDevice::isOverflowed()
{
    return this->device->GetStreamOverflow();
}

bool QADQDevice::acquireBuffer(StreamingBuffers &to)
{
    return this->device->GetDataStreaming(
        (void**)(to.data),
        (void**)(to.headers),
        to.channelMask,
        to.nof_samples,
        to.nof_headers,
        to.header_status
    );
}

float QADQDevice::getInputRangeMilvolts(int ch)
{
    float ir;
    if(this->device->GetInputRange(ch, &ir))
        return ir;
    else return 0;
}

bool QADQDevice::setInputRange(int ch, float target, float &result)
{
    float obtained;
    for(int ch=0; ch < MAX_NOF_CHANNELS; ch++)
    {
        if(this->device->SetInputRange(ch+1, target, &obtained))
        {
            result = obtained;
            this->appConfig.channelConfig[ch].inputRangeFloat = result;
        }
    }
    return true;
}

int QADQDevice::getDCBias(int ch)
{
    int result;
    if(this->device->GetAdjustableBias(ch, &result))
        return result;
    else return 0;
}

bool QADQDevice::setDCBias(int ch, int offset)
{
    return this->device->SetAdjustableBias(ch, offset);
}

bool QADQDevice::setGainAndOffset(int ch, int gain, int offset)
{
    return this->device->SetGainAndOffset(ch+1, gain, offset);
}

bool QADQDevice::writeUserRegister(unsigned int target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int &retval)
{
    return this->device->WriteUserRegister(target, regnum, mask, data, &retval);
}

int QADQDevice::milivoltsToCodes(int ch, float mv)
{
    return std::round ( mv / ( this->appConfig.channelConfig[ch].inputRangeFloat / 2 ) * std::pow(2,15) );
}

float QADQDevice::codesToMilivolts(int ch, int code)
{
    return (float)code * (this->appConfig.channelConfig[ch].inputRangeFloat/2) / std::pow(2,15);
}

std::unique_ptr<QAcquisition::SmartBuffer> QADQDevice::requestBuffer(unsigned int timeout)
{
    return this->acquisition.requestBuffer(timeout);
}

bool QADQDevice::overrideAcquisition(ApplicationConfiguration &providedConfig)
{
    int adqChannelIndex = providedConfig.getCurrentChannel()+1;
    unsigned int channelMask = 1<<providedConfig.getCurrentChannel();
    unsigned int trigChannelMask = channelMask;
    if(providedConfig.secondChannel != CHANNEL_DISABLED)
    {
        channelMask = 1<<providedConfig.getCurrentChannel() | 1<<providedConfig.secondChannel;
    }
    if(!this->device->SetClockSource(providedConfig.clockSource)) {spdlog::error("SetClockSource failed."); return false;};
    if(!this->device->SetTriggerMode(providedConfig.getCurrentChannelConfig().triggerMode)) {spdlog::error("SetTriggerMode failed."); return false;};
    if(!this->device->SetSampleSkip(providedConfig.getCurrentChannelConfig().sampleSkip)) {spdlog::error("SetSampleSkip failed."); return false;};
    for(int i = 0; i<2; i++)
    {
        if(providedConfig.getCurrentChannelConfig().userLogicBypass & (1<<i))
        {
            if(!this->device->BypassUserLogic(i+1, 1)) {spdlog::error("BypassUserLogic failed"); return false;};
        }
        else
        {
            if(!this->device->BypassUserLogic(i+1, 0)) {spdlog::error("BypassUserLogic failed."); return false;};
        }
    }
    if(!this->device->SetInputRange(
        adqChannelIndex,
        providedConfig.getCurrentChannelConfig().inputRangeFloat,
        &(providedConfig.getCurrentChannelConfig().inputRangeFloat))
    ) {spdlog::error("SetInputRange failed."); return false;};
    int totalBias = providedConfig.getCurrentChannelConfig().dcBiasCode + providedConfig.getCurrentChannelConfig().getCurrentBaseDCOffset();
    int clampedBias = std::max(SHRT_MIN, std::min(totalBias, SHRT_MAX));
    if(!this->device->SetAdjustableBias(
        adqChannelIndex,
        clampedBias)
    ) {spdlog::error("SetAdjustableBias failed."); return false;};
    if(!this->device->SetTransferBuffers(
        providedConfig.transferBufferCount,
        providedConfig.transferBufferSize)
    ) {spdlog::critical("SetTransferBuffers failed."); return false;};
    if(providedConfig.getCurrentChannelConfig().isContinuousStreaming) // continuous
    {
        /*
        this->bufferProcessor->reallocateBuffers(
                providedConfig.deviceConfig.transferBufferSize/sizeof(short)
        );*/
        spdlog::info("Configuring continuous streaming with mask {:#b}.", channelMask);
        if(!this->device->ContinuousStreamingSetup(channelMask)) {spdlog::error("ContinuousStreamingSetup failed."); return false;};
    }
    else
    {
        spdlog::info("Configuring triggered streaming.");
        if(!this->device->SetPreTrigSamples(providedConfig.getCurrentChannelConfig().pretrigger)) {spdlog::error("SetPreTrigSamples failed."); return false;};
        if(!this->device->SetLvlTrigLevel(providedConfig.getCurrentChannelConfig().getDCBiasedTriggerValue())) {spdlog::error("SetLvlTrigLevel failed."); return false;};
        if(!this->device->SetLvlTrigChannel(trigChannelMask)) {spdlog::error("SetLvlTrigChannel failed."); return false;};
        if(!this->device->SetLvlTrigEdge(providedConfig.getCurrentChannelConfig().triggerEdge)) {spdlog::error("SetLvlTrigEdge failed"); return false;};
        if(!this->device->TriggeredStreamingSetup(
            providedConfig.getCurrentChannelConfig().recordCount,
            providedConfig.getCurrentChannelConfig().recordLength,
            providedConfig.getCurrentChannelConfig().pretrigger,
            providedConfig.getCurrentChannelConfig().triggerDelay,
            channelMask
        )) {spdlog::error("TriggeredStreamingSetup failed."); return false;};
    }
    spdlog::info("Configured acquisition successfully.");
    this->writeBuffers.reconfigure(
        providedConfig.writeBufferCount,
        providedConfig.transferBufferSize,
        channelMask
    );

    this->configured = true;
    return this->configured;
}

//////////// QAcquisition /////////////

QAcquisition::SmartBuffer::SmartBuffer(WriteBuffers &wb, StreamingBuffers &sb):
wb(wb), sb(sb)
{

}
QAcquisition::SmartBuffer::~SmartBuffer()
{
    this->wb.notifyRead();
}

QAcquisition::QAcquisition(QADQDevice &device, WriteBuffers &writeBuffers):
device(device), writeBuffers(writeBuffers)
{

}

std::unique_ptr<QAcquisition::SmartBuffer> QAcquisition::requestBuffer(unsigned int timeout)
{
    StreamingBuffers * sb = this->writeBuffers.awaitRead(timeout);
    if(sb) return std::unique_ptr<QAcquisition::SmartBuffer>(new SmartBuffer(this->writeBuffers, *sb));
    else return std::unique_ptr<QAcquisition::SmartBuffer>(nullptr);
}

bool QAcquisition::run(unsigned int bufferCount, unsigned int dmaFlushDuration, unsigned int duration)
{
    if(this->loopDMA)
    {
        spdlog::critical("Acquisition already running.");
    }
    if(this->device.startStreaming())
    {
        this->loopDMA = true;
        if(duration) {
            this->acquisitionTimer.start();
        }
        while(this->loopDMA)
        {
            if(duration!=0 && this->acquisitionTimer.hasExpired(duration)) {
                this->stop(false);
                break;
            }
            unsigned int filledBuffers = this->device.getFilledBuffersCount();
            if(filledBuffers == 0)
            {
                if(this->dmaFlushTimer.hasExpired(dmaFlushDuration))
                {
                    this->device.flushDMA();
                    this->dmaFlushTimer.start();
                }
                continue;
            }
            else if(filledBuffers == bufferCount - 1)
            {
                if(this->device.isOverflowed())
                {
                    spdlog::error("Stream overflow!");
                    this->stop(false);
                    break;
                }
            }
            emit this->onBuffersFilled(filledBuffers);
            this->dmaFlushTimer.start();

            StreamingBuffers* sbuf = nullptr;
            do{
             sbuf = this->writeBuffers.awaitWrite(1000);
             if(!this->loopDMA)
             {
                 this->writeBuffers.notifyRead(); // rerelease the acquired buffer
                 goto DMA_CHECKER_LOOP_EXIT;
             }
            } while(sbuf == nullptr);
            for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
            {
                if(!(sbuf->channelMask & (1<<ch))) continue;
                sbuf->headers[ch][0] = this->incompleteHeaders[ch];
                // copy last header
            }
            if(!this->device.acquireBuffer(*sbuf)) {
                this->stop(false);
                break;
            }
            for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
            {
                if(!(sbuf->channelMask & (1<<ch))) continue;
                //spdlog::debug("Copying headers from channel {} should have {} headers", ch, sbuf->nof_headers[ch]);
                this->incompleteHeaders[ch] = sbuf->headers[ch][sbuf->nof_headers[ch]==0?0:(sbuf->nof_headers[ch]-1)];
            }
            //spdlog::debug("Wrote buffers. Notifying.");
            this->writeBuffers.notifyWritten();
            //spdlog::debug("Notify written");
        } DMA_CHECKER_LOOP_EXIT:

        return true;
    }
    else return false;
}

bool QAcquisition::stop(bool dumpUnprocessed)
{
    this->loopDMA = false;
    this->setState(STATES::STOPPED);
    return this->device.stopStreaming();
}
void QAcquisition::setState(STATES state)
{
    this->state = state;
    emit this->onStateChanged(state);
}
QAcquisition::STATES QAcquisition::getState() const
{
    return state;
}
StreamingBuffers &QAcquisition::SmartBuffer::data()
{
    return this->sb;
}

QADQStreamProcessor::QADQStreamProcessor(WriteBuffers &writeBuffers, BufferProcessor &processor):
writeBuffers(writeBuffers),
processor(processor)
{

}

void QADQStreamProcessor::run(bool triggered)
{
    if(this->loopActive)
    {
        spdlog::critical("Buffer Processor loop already active.");
        return;
    }
    this->loopActive = true;
    //spdlog::debug("Processor thread active");
    while(this->loopActive)
    {
        StreamingBuffers * b = nullptr;
        do{
            b = this->writeBuffers.awaitRead(1000);
            if(!this->loopActive)
            {
                goto BUFFER_PROCESSOR_LOOP_EXIT; // break if we want to join the thread
            }
        } while(b==nullptr);

        //spdlog::debug("Got read lock on {}", fmt::ptr(b));
        if(!this->processor.processBuffers(*b, triggered)) {
            spdlog::error("Could not process buffers. Stopping.");
            this->stop();
            break;
        }

        //spdlog::debug("Read succeful");
        this->writeBuffers.notifyRead();
        //spdlog::debug("Read notify");
    }
BUFFER_PROCESSOR_LOOP_EXIT:
    this->loopActive = false;
    emit this->onStopped();
    spdlog::debug("Buffer loop exit");
}

void QADQStreamProcessor::stop()
{

}
