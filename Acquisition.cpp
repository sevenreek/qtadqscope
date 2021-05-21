#include "Acquisition.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <chrono>
#include <algorithm>
Acquisition::Acquisition(
    ApplicationConfiguration& appConfig,
    ADQInterface& adqDevice):
        appConfig(appConfig),
        adqDevice(adqDevice),
        writeBuffers{
            appConfig.writeBufferCount,
            appConfig.transferBufferSize,
            (unsigned char)(1<<(appConfig.getCurrentChannel()))
        },
        bufferProcessor(
            new BaseBufferProcessor(
                this->recordProcessors,
                appConfig.transferBufferSize/sizeof(short)
                )
            )
{
    //scopeUpdater->moveToThread(&this->bufferProcessingThread);

    bufferProcessorHandler = new LoopBufferProcessor{writeBuffers, *bufferProcessor.get()};
    bufferProcessorHandler->moveToThread(&this->bufferProcessingThread);
    connect(&this->bufferProcessingThread, &QThread::finished, bufferProcessorHandler, &QObject::deleteLater);
    connect(this, &Acquisition::onStart, bufferProcessorHandler, &LoopBufferProcessor::runLoop, Qt::ConnectionType::QueuedConnection);
    connect(bufferProcessorHandler, &LoopBufferProcessor::onLoopStopped, this, &Acquisition::onProcessingThreadStopped);
    connect(bufferProcessorHandler, &LoopBufferProcessor::onError, this, &Acquisition::error, Qt::ConnectionType::BlockingQueuedConnection);

    dmaChecker = new DMAChecker{writeBuffers, adqDevice, appConfig.transferBufferCount};
    dmaChecker->moveToThread(&this->dmaCheckingThread);
    connect(&this->dmaCheckingThread, &QThread::finished, dmaChecker, &QObject::deleteLater);
    connect(this, &Acquisition::onStart, dmaChecker, &DMAChecker::runLoop, Qt::ConnectionType::QueuedConnection);
    connect(dmaChecker, &DMAChecker::onLoopStopped, this, &Acquisition::onAcquisitionThreadStopped);
    connect(dmaChecker, &DMAChecker::onError, this, &Acquisition::error, Qt::ConnectionType::BlockingQueuedConnection);
    connect(dmaChecker, &DMAChecker::onBuffersFilled, this, &Acquisition::buffersFilled, Qt::ConnectionType::DirectConnection);

    bufferProcessingThread.start();
    dmaCheckingThread.start();
    this->acqusitionTimer = std::unique_ptr<QTimer>(new QTimer(this));
    this->acqusitionTimer->setSingleShot(true);
    connect(this->acqusitionTimer.get(), &QTimer::timeout, this, [=](){this->stop();});
}
Acquisition::~Acquisition() {
    // bufferProcessorHandler and dmaChecker get deleted via a QThread::finished signal->slot
    this->joinThreads();
}

void Acquisition::stopDMAChecker()
{
    this->dmaChecker->stopLoop();
}
void Acquisition::joinThreads()
{
    this->stopDMAChecker();
    this->stopProcessor();
    this->dmaCheckingThread.quit();
    this->bufferProcessingThread.quit();

    this->bufferProcessingThread.wait();
    this->dmaCheckingThread.wait();
}
void Acquisition::stopProcessor()
{
    this->bufferProcessorHandler->stopLoop();
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
        this->appConfig.transferBufferCount,
        this->appConfig.transferBufferSize)
    ) {spdlog::critical("SetTransferBuffers failed."); return false;};
    this->bufferProcessorHandler->changeStreamingType(!this->appConfig.getCurrentChannelConfig().isContinuousStreaming);
    for(auto rp : this->recordProcessors)
    {
        rp->startNewStream(this->appConfig);
    }
    if(this->appConfig.getCurrentChannelConfig().isContinuousStreaming) // continuous
    {
        /*
        this->bufferProcessor->reallocateBuffers(
                appConfig.deviceConfig.transferBufferSize/sizeof(short)
        );*/
        spdlog::info("Configuring continuous streaming with mask {:#b}.", channelMask);
        if(!this->adqDevice.ContinuousStreamingSetup(channelMask)) {spdlog::error("ContinuousStreamingSetup failed."); return false;};
    }
    else
    {
        this->bufferProcessor->reallocateBuffers(appConfig.getCurrentChannelConfig().recordLength);
        spdlog::info("Configuring triggered streaming.");
        if(!this->adqDevice.SetPreTrigSamples(this->appConfig.getCurrentChannelConfig().pretrigger)) {spdlog::error("SetPreTrigSamples failed."); return false;};
        if(!this->adqDevice.SetLvlTrigLevel(this->appConfig.getCurrentChannelConfig().getDCBiasedTriggerValue())) {spdlog::error("SetLvlTrigLevel failed."); return false;};
        if(!this->adqDevice.SetLvlTrigChannel(channelMask)) {spdlog::error("SetLvlTrigChannel failed."); return false;};
        if(!this->adqDevice.SetLvlTrigEdge(this->appConfig.getCurrentChannelConfig().triggerEdge)) {spdlog::error("SetLvlTrigEdge failed"); return false;};
        if(!this->adqDevice.TriggeredStreamingSetup(
            this->appConfig.getCurrentChannelConfig().recordCount,
            this->appConfig.getCurrentChannelConfig().recordLength,
            this->appConfig.getCurrentChannelConfig().pretrigger,
            this->appConfig.getCurrentChannelConfig().triggerDelay,
            channelMask
        )) {spdlog::error("TriggeredStreamingSetup failed."); return false;};
    }
    spdlog::info("Configured acquisition successfully.");
    this->writeBuffers.reconfigure(
        this->appConfig.writeBufferCount,
        this->appConfig.transferBufferSize,
        channelMask
    );

    this->configured = true;
    return this->configured;
}
bool Acquisition::startTimed(unsigned long msDuration)
{
    this->acqusitionTimer->setInterval(msDuration);
    this->acqusitionTimer->start();
    this->start();
}
bool Acquisition::start()
{
    this->setState(ACQUISITION_STATES::RUNNING);
    /*
    if(!this->adqDevice.FlushDMA())
    {
        spdlog::debug("Flush DMA failed.");
    }
    if(!this->adqDevice.CollectDataNextPage())
    {
        spdlog::debug("Collect next page failed.");
    }*/
    this->writeBuffers.resetSemaphores();
    this->appConfig.getCurrentChannelConfig().log();
    spdlog::info("API: Stream start");
    this->timeStarted = std::chrono::high_resolution_clock::now();
    if(!this->adqDevice.StartStreaming())
    {
        spdlog::error("Stream failed to start!");
        return false;
    }
    this->dmaLoopStopped = false;
    this->processingLoopStopped = false;
    emit this->onStart();
    if(this->appConfig.getCurrentChannelConfig().isContinuousStreaming)
    {
        spdlog::debug("SWTRIG");
        this->adqDevice.SWTrig();
    }
    return true;
}
bool Acquisition::stop()
{
    this->adqDevice.StopStreaming();
    this->acqusitionTimer->stop();
    spdlog::info("API: Stream stop");
    this->setState(ACQUISITION_STATES::STOPPING);
    this->stopDMAChecker();
    this->stopProcessor();
    this->timeStopped = std::chrono::high_resolution_clock::now();
    if(this->dmaLoopStopped && this->processingLoopStopped)
    {
        this->setStoppedState();
    }
    return true;
}
void Acquisition::error()
{
    spdlog::error("Error in one of the acquisition threads");
    this->stop();
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
void Acquisition::onAcquisitionThreadStopped()
{
    this->dmaLoopStopped = true;
    if(this->dmaLoopStopped && this->processingLoopStopped)
    {
        this->setStoppedState();
    }
}
void Acquisition::onProcessingThreadStopped()
{
    this->processingLoopStopped = true;
    if(this->dmaLoopStopped && this->processingLoopStopped)
    {
        this->setStoppedState();
    }
}

void Acquisition::appendRecordProcessor(std::shared_ptr<RecordProcessor> rp)
{
    if(rp == nullptr)
    {
        spdlog::error("Tried appending null record processor");
        return;
    }
    spdlog::debug("Append record Processor");
    unsigned int countBefore = this->recordProcessors.size();
    if(std::find(this->recordProcessors.begin(), this->recordProcessors.end(), rp) != this->recordProcessors.end())
    {
        spdlog::warn("Tried to duplicate record processor in list.");
        return;
    }
    this->recordProcessors.push_back(rp);
    if(this->recordProcessors.size() != countBefore+1)
    {
        spdlog::warn("Adding record processor failed");
    }
}
void Acquisition::removeRecordProcessor(std::shared_ptr<RecordProcessor> rp)
{
    //spdlog::debug("Remvoe record Processor");
    if(rp == nullptr)
    {
        spdlog::error("Tried to remove null record processor in list.");
        return;
    }
    unsigned int countBefore = this->recordProcessors.size();
    this->recordProcessors.remove(rp);
    if(this->recordProcessors.size() != countBefore-1)
    {
        spdlog::warn("Removing record processor failed");
    }
}

void Acquisition::buffersFilled(unsigned long filled)
{
    this->lastBuffersFilled = filled;
}
void Acquisition::finishRecordProcessors()
{
    std::chrono::duration<double> elapsed = this->timeStopped - this->timeStarted;
    for(auto rp : this->recordProcessors)
    {
       unsigned long long processedBytes = rp->finish();
       spdlog::info(
            "{} processed {} bytes over {:.2f} seconds. Data rate {:.2f} MB/s.",
            rp->getName(), processedBytes, elapsed.count(), processedBytes/1000000.0/elapsed.count());
    }
}
void Acquisition::setStoppedState()
{
    this->setState(ACQUISITION_STATES::STOPPED);
    this->finishRecordProcessors();
}

unsigned long Acquisition::getBuffersFill()
{
    return this->lastBuffersFilled;
}
int Acquisition::getReadQueueFill()
{
    return this->appConfig.writeBufferCount - this->writeBuffers.getReadCount();
}
int Acquisition::getWriteQueueFill()
{
    return this->appConfig.writeBufferCount - this->writeBuffers.getWriteCount();
}
