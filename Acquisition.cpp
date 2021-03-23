#include "Acquisition.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <chrono>
Acquisition::Acquisition(
    ApplicationConfiguration& appConfig,
    ADQInterface& adqDevice,
    std::shared_ptr<ScopeUpdater> scopeUpdater):
        appConfig(appConfig),
        adqDevice(adqDevice),
        scopeUpdater(scopeUpdater)
{
    this->writeBuffers = std::unique_ptr<WriteBuffers>(
        new WriteBuffers(
            appConfig.writeBufferCount,
            appConfig.deviceConfig.transferBufferSize,
            1<<(appConfig.getCurrentChannel())
        )
    );
}

unsigned long Acquisition::checkDMA()
{

    return 0;
}
void Acquisition::runDMAChecker()
{
    if(this->bufferProcessor == nullptr)
    {
        spdlog::critical("BufferProcessor not found. cannot start.");
        this->setState(ACQUISITION_STATES::STOPPED);
        return;
    }
    this->dmaCheckingActive = true;
    spdlog::debug("DMA thread active");
    while(this->dmaCheckingActive)
    {
        unsigned int buffersFilled = 0;

        if(!this->adqDevice.GetTransferBufferStatus(&buffersFilled)) {
            spdlog::error("Could not get transfer buffer status. Stopping.");
            this->error(); // stop if error
            break;
        }
        /*
        if(buffersFilled)
        {
            this->lastFilledBufferReceivedOn = std::chrono::system_clock::now();
        }
        else if(std::chrono::system_clock::now()
            > this->lastFilledBufferReceivedOn + std::chrono::milliseconds(flushTimeoutMs))
        {
            spdlog::info("Flushing DMA buffers.");
            this->adqDevice->FlushDMA();
            this->lastFilledBufferReceivedOn = std::chrono::system_clock::now();
        }*/
        if(buffersFilled >= this->appConfig.deviceConfig.transferBufferCount-1) {
            if(this->adqDevice.GetStreamOverflow()) {
                spdlog::error("STREAM OVERFLOWING! Some samples will be lost!", buffersFilled);
            }
            else {
                spdlog::warn("Filled {}/{} DMA buffers. Overflow likely soon!", buffersFilled, this->appConfig.deviceConfig.transferBufferCount);
            }
        }
        if(buffersFilled)
            spdlog::debug("Filled {} buffers.", buffersFilled);
        for(int b = 0; b < buffersFilled; b++) // if no buffers are filled the for loop will not start
        {
            StreamingBuffers* sbuf = this->writeBuffers->awaitWrite();
            spdlog::debug("Got write lock on {}", fmt::ptr(sbuf));
            if(!this->dmaCheckingActive) break;
            if(!this->adqDevice.GetDataStreaming(
                (void**)(sbuf->data),
                (void**)(sbuf->headers),
                sbuf->channelMask,
                sbuf->nof_samples,
                sbuf->nof_headers,
                sbuf->header_status
            )) {
                spdlog::error("Could not get data stream. Stopping.");
                this->error(); // stop if error
                break;
            }
            spdlog::debug("Wrote buffers. Notifying.");
            this->writeBuffers->notifyWritten();
            spdlog::debug("Notify written");
        }
        if(buffersFilled == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    this->dmaThreadActive = false;
    if(!this->bufferThreadActive) {
        this->setState(ACQUISITION_STATES::STOPPED);
    }
    this->onAcquisitionThreadStopped();
}
void Acquisition::stopDMAChecker()
{
    this->dmaCheckingActive = false;
}
void Acquisition::joinDMAChecker()
{
    if(this->dmaCheckingThread != nullptr)
    {
        this->writeBuffers->notifyRead();
        spdlog::debug("Trying to join DMA");
        this->dmaCheckingThread->join();
        spdlog::debug("dma joined");
        this->dmaCheckingThread.reset();
    }
}
void Acquisition::runProcessor()
{
    if(this->bufferProcessor == nullptr)
    {
        spdlog::critical("BufferProcessor not found. cannot start.");
        this->state = ACQUISITION_STATES::STOPPED;
        return;
    }
    this->bufferProcessingActive = true;
    spdlog::debug("Processor thread active");
    while(this->bufferProcessingActive)
    {
        StreamingBuffers * b = this->writeBuffers->awaitRead();
        spdlog::debug("Got read lock on {}", fmt::ptr(b));
        if(this->writeBuffers->stopWriteThreads) break; // break if we want to join the thread
        if(!this->bufferProcessor->processBuffers(*b, this->isTriggeredStreaming)) {
            spdlog::error("Could not process buffers. Stopping.");
            this->stop();
            return;
        }
        spdlog::debug("Read succeful");
        this->writeBuffers->notifyRead();
        spdlog::debug("Read notify");
    }
    this->bufferThreadActive = false;
    if(!this->dmaThreadActive) {
        this->setState(ACQUISITION_STATES::STOPPED);
    }
    this->onAcquisitionThreadStopped();
}
void Acquisition::stopProcessor()
{
    this->bufferProcessingActive = false;
}
void Acquisition::joinProcessor()
{
    if(this->bufferProcessingThread != nullptr)
    {
        this->writeBuffers->notifyWritten();
        spdlog::debug("Trying to join processor");
        this->bufferProcessingThread->join();
        spdlog::debug("processor joined");
        this->bufferProcessingThread.reset();
    }
}
bool Acquisition::configure()
{

    int adqChannelIndex = this->appConfig.getCurrentChannel()+1;
    unsigned int channelMask = 1<<this->appConfig.getCurrentChannel();
    if(!this->adqDevice.SetClockSource(this->appConfig.clockSource)) {spdlog::error("SetClockSource failed."); return false;};
    if(!this->adqDevice.SetTriggerMode(this->appConfig.getCurrentChannelConfig().triggerMode)) {spdlog::error("SetTriggerMode failed."); return false;};
    if(!this->adqDevice.SetSampleSkip(this->appConfig.getCurrentChannelConfig().sampleSkip)) {spdlog::error("SetSampleSkip failed."); return false;};
    for(int i = 0; i<2; i++)
    {
        if(this->appConfig.getCurrentChannelConfig().userLogicBypass & (1<<i))
        {
            if(!this->adqDevice.BypassUserLogic(i+1, 1)) {spdlog::error("BypassUserLogic failed"); return false;};
        }
        else
        {
            if(!this->adqDevice.BypassUserLogic(i+1, 0)) {spdlog::error("BypassUserLogic failed."); return false;};
        }
    }
    if(!this->adqDevice.SetInputRange(
        adqChannelIndex,
        this->appConfig.getCurrentChannelConfig().inputRangeFloat,
        &(this->appConfig.getCurrentChannelConfig().inputRangeFloat))
    ) {spdlog::error("SetInputRange failed."); return false;};
    if(!this->adqDevice.SetAdjustableBias(
        adqChannelIndex,
        this->appConfig.getCurrentChannelConfig().dcBiasCode + appConfig.getCurrentChannelConfig().baseDcBiasOffset)
    ) {spdlog::error("SetAdjustableBias failed."); return false;};
    if(!this->adqDevice.SetTransferBuffers(
        this->appConfig.deviceConfig.transferBufferCount,
        this->appConfig.deviceConfig.transferBufferSize)
    ) {spdlog::error("SetTransferBuffers failed."); return false;};
    if(this->appConfig.getCurrentChannelConfig().recordLength <= 0) // continuous
    {
        this->isTriggeredStreaming = false;
        this->bufferProcessor = std::unique_ptr<BufferProcessor>(
            new BaseBufferProcessor(
                appConfig.deviceConfig.transferBufferSize/sizeof(short)
            )
        );
        spdlog::info("Configuring continuous streaming with mask {:#b}.", channelMask);
        if(!this->adqDevice.ContinuousStreamingSetup(channelMask)) {spdlog::error("ContinuousStreamingSetup failed."); return false;};
        this->scopeUpdater->reallocate(this->appConfig.deviceConfig.transferBufferSize/sizeof(short));
    }
    else
    {
        this->isTriggeredStreaming = true;
        this->bufferProcessor = std::unique_ptr<BufferProcessor>(
            new BaseBufferProcessor(
                appConfig.getCurrentChannelConfig().recordLength
            )
        );
        spdlog::info("Configuring triggered streaming.");
        if(!this->adqDevice.SetPreTrigSamples(this->appConfig.getCurrentChannelConfig().pretrigger)) {spdlog::error("SetPreTrigSamples failed."); return false;};
        if(!this->adqDevice.SetLvlTrigLevel(this->appConfig.getCurrentChannelConfig().triggerLevelCode)) {spdlog::error("SetLvlTrigLevel failed."); return false;};
        if(!this->adqDevice.SetLvlTrigChannel(channelMask)) {spdlog::error("SetLvlTrigChannel failed."); return false;};
        if(!this->adqDevice.SetLvlTrigEdge(this->appConfig.getCurrentChannelConfig().triggerEdge)) {spdlog::error("SetLvlTrigEdge failed"); return false;};
        if(!this->adqDevice.TriggeredStreamingSetup(
            this->appConfig.getCurrentChannelConfig().recordCount,
            this->appConfig.getCurrentChannelConfig().recordLength,
            this->appConfig.getCurrentChannelConfig().pretrigger,
            this->appConfig.getCurrentChannelConfig().triggerDelay,
            channelMask
        )) {spdlog::error("TriggeredStreamingSetup failed."); return false;};
        this->scopeUpdater->reallocate(this->appConfig.getCurrentChannelConfig().recordLength);
    }
    spdlog::info("Configured acquisition successfully.");
    this->writeBuffers->reconfigure(
        this->appConfig.writeBufferCount,
        this->appConfig.deviceConfig.transferBufferSize,
        channelMask
    );
    this->bufferProcessor->clearRecordProcessors();
    if(this->appConfig.getCurrentChannelConfig().updateScope)
    {
        this->bufferProcessor->appendRecordProcessor(this->scopeUpdater);
    }
    this->configured = true;
    return this->configured;
}
bool Acquisition::start()
{
    this->setState(ACQUISITION_STATES::RUNNING);
    this->joinProcessor();
    this->joinDMAChecker();
    this->writeBuffers->resetSemaphores();
    this->bufferProcessingThread = std::unique_ptr<std::thread>(
        new std::thread{[](Acquisition* acq) {
            acq->runProcessor();
        }, this}
    );
    this->dmaCheckingThread = std::unique_ptr<std::thread>(
        new std::thread{[](Acquisition* acq) {
            acq->runDMAChecker();
        }, this}
    );
    this->appConfig.getCurrentChannelConfig().log();
    spdlog::info("API: Stream start");
    if(!this->adqDevice.StartStreaming())
    {
        spdlog::error("Stream failed to start!");
        return false;
    }
    if(this->appConfig.getCurrentChannelConfig().isContinuousStreaming)
    {
        spdlog::debug("SWTRIG");
        this->adqDevice.SWTrig();
    }
    else
    {
        spdlog::debug("NOT SWTRIG");
    }
    return true;
}
bool Acquisition::stop()
{
    this->adqDevice.StopStreaming();
    spdlog::info("API: Stream stop");
    this->setState(ACQUISITION_STATES::STOPPING);
    this->stopDMAChecker();
    this->stopProcessor();
    if(!this->dmaThreadActive && !this->bufferThreadActive)
    {
        this->setState(ACQUISITION_STATES::STOPPED);
    }
    return true;
}
void Acquisition::error()
{
    this->stopProcessor();
    this->stopDMAChecker();
    this->adqDevice.StopStreaming();
    this->setState(ACQUISITION_STATES::STOPPED);
}
ACQUISITION_STATES Acquisition::getState()
{
    return this->state;
}

void Acquisition::setState(ACQUISITION_STATES state)
{
    this->state = state;
    onStateChanged(state);
}

bool Acquisition::getDMAThreadActive()
{
    return this->dmaThreadActive;
}
bool Acquisition::getProcessingThreadActive()
{
    return this->bufferThreadActive;
}
