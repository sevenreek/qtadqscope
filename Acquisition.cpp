#include "Acquisition.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <chrono>
#include <algorithm>
Acquisition::Acquisition(
    std::shared_ptr<ApplicationConfiguration> appConfig,
    std::shared_ptr<ADQInterface> adqDevice)
{
    this->appConfig = appConfig;
    this->adqDevice = adqDevice;
    this->writeBuffers = std::shared_ptr<WriteBuffers>(new WriteBuffers(
        appConfig->writeBufferCount,
        appConfig->transferBufferSize,
        (unsigned char)(1<<(appConfig->getCurrentChannel())))
    );
    this->bufferProcessor = std::shared_ptr<BaseBufferProcessor>(new BaseBufferProcessor(
        this->recordProcessors,
        appConfig->transferBufferSize/sizeof(short)
    ));
    bufferProcessorHandler = std::unique_ptr<LoopBufferProcessor>(new LoopBufferProcessor(writeBuffers, this->bufferProcessor));
    bufferProcessorHandler->moveToThread(&this->bufferProcessingThread);
    dmaChecker = std::unique_ptr<DMAChecker>(new DMAChecker(writeBuffers, adqDevice, appConfig->transferBufferCount));
    this->initialize();
}
void Acquisition::initialize()
{
    connect(&this->bufferProcessingThread, &QThread::finished, bufferProcessorHandler.get(), &QObject::deleteLater);
    connect(this, &Acquisition::onStart, bufferProcessorHandler.get(), &LoopBufferProcessor::runLoop, Qt::ConnectionType::QueuedConnection);
    connect(bufferProcessorHandler.get(), &LoopBufferProcessor::onLoopStopped, this, &Acquisition::onProcessingThreadStopped);
    connect(bufferProcessorHandler.get(), &LoopBufferProcessor::onError, this, &Acquisition::error, Qt::ConnectionType::BlockingQueuedConnection);

    dmaChecker->moveToThread(&this->dmaCheckingThread);
    connect(&this->dmaCheckingThread, &QThread::finished, dmaChecker.get(), &QObject::deleteLater);
    connect(this, &Acquisition::onStart, dmaChecker.get(), &DMAChecker::runLoop, Qt::ConnectionType::QueuedConnection);
    connect(dmaChecker.get(), &DMAChecker::onLoopStopped, this, &Acquisition::onAcquisitionThreadStopped);
    connect(dmaChecker.get(), &DMAChecker::onError, this, &Acquisition::error, Qt::ConnectionType::BlockingQueuedConnection);
    connect(dmaChecker.get(), &DMAChecker::onBuffersFilled, this, &Acquisition::buffersFilled, Qt::ConnectionType::DirectConnection);

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
bool Acquisition::configure(std::shared_ptr<ApplicationConfiguration> providedConfig = nullptr, std::list<std::shared_ptr<RecordProcessor>> recordProcessors=std::list<std::shared_ptr<RecordProcessor>>())
{
    this->recordProcessors = recordProcessors;
    if(providedConfig == nullptr)
        providedConfig = this->appConfig;
    int adqChannelIndex = providedConfig->getCurrentChannel()+1;
    unsigned int channelMask = 1<<providedConfig->getCurrentChannel();
    if(!this->adqDevice->SetClockSource(providedConfig->clockSource)) {spdlog::error("SetClockSource failed."); return false;};
    if(!this->adqDevice->SetTriggerMode(providedConfig->getCurrentChannelConfig().triggerMode)) {spdlog::error("SetTriggerMode failed."); return false;};
    if(!this->adqDevice->SetSampleSkip(providedConfig->getCurrentChannelConfig().sampleSkip)) {spdlog::error("SetSampleSkip failed."); return false;};
    for(int i = 0; i<2; i++)
    {
        if(providedConfig->getCurrentChannelConfig().userLogicBypass & (1<<i))
        {
            if(!this->adqDevice->BypassUserLogic(i+1, 1)) {spdlog::error("BypassUserLogic failed"); return false;};
        }
        else
        {
            if(!this->adqDevice->BypassUserLogic(i+1, 0)) {spdlog::error("BypassUserLogic failed."); return false;};
        }
    }
    if(!this->adqDevice->SetInputRange(
        adqChannelIndex,
        providedConfig->getCurrentChannelConfig().inputRangeFloat,
        &(providedConfig->getCurrentChannelConfig().inputRangeFloat))
    ) {spdlog::error("SetInputRange failed."); return false;};
    if(!this->adqDevice->SetAdjustableBias(
        adqChannelIndex,
        providedConfig->getCurrentChannelConfig().dcBiasCode + providedConfig->getCurrentChannelConfig().baseDcBiasOffset)
    ) {spdlog::error("SetAdjustableBias failed."); return false;};
    if(!this->adqDevice->SetTransferBuffers(
        providedConfig->transferBufferCount,
        providedConfig->transferBufferSize)
    ) {spdlog::critical("SetTransferBuffers failed."); return false;};
    this->bufferProcessorHandler->changeStreamingType(!providedConfig->getCurrentChannelConfig().isContinuousStreaming);
    for(auto rp : this->recordProcessors)
    {
        rp->startNewStream(*providedConfig.get());
    }
    if(providedConfig->getCurrentChannelConfig().isContinuousStreaming) // continuous
    {
        /*
        this->bufferProcessor->reallocateBuffers(
                providedConfig.deviceConfig.transferBufferSize/sizeof(short)
        );*/
        spdlog::info("Configuring continuous streaming with mask {:#b}.", channelMask);
        if(!this->adqDevice->ContinuousStreamingSetup(channelMask)) {spdlog::error("ContinuousStreamingSetup failed."); return false;};
    }
    else
    {
        this->bufferProcessor->reallocateBuffers(providedConfig->getCurrentChannelConfig().recordLength);
        spdlog::info("Configuring triggered streaming.");
        if(!this->adqDevice->SetPreTrigSamples(providedConfig->getCurrentChannelConfig().pretrigger)) {spdlog::error("SetPreTrigSamples failed."); return false;};
        if(!this->adqDevice->SetLvlTrigLevel(providedConfig->getCurrentChannelConfig().getDCBiasedTriggerValue())) {spdlog::error("SetLvlTrigLevel failed."); return false;};
        if(!this->adqDevice->SetLvlTrigChannel(channelMask)) {spdlog::error("SetLvlTrigChannel failed."); return false;};
        if(!this->adqDevice->SetLvlTrigEdge(providedConfig->getCurrentChannelConfig().triggerEdge)) {spdlog::error("SetLvlTrigEdge failed"); return false;};
        if(!this->adqDevice->TriggeredStreamingSetup(
            providedConfig->getCurrentChannelConfig().recordCount,
            providedConfig->getCurrentChannelConfig().recordLength,
            providedConfig->getCurrentChannelConfig().pretrigger,
            providedConfig->getCurrentChannelConfig().triggerDelay,
            channelMask
        )) {spdlog::error("TriggeredStreamingSetup failed."); return false;};
    }
    spdlog::info("Configured acquisition successfully.");
    this->writeBuffers->reconfigure(
        providedConfig->writeBufferCount,
        providedConfig->transferBufferSize,
        channelMask
    );

    this->configured = true;
    return this->configured;
}
bool Acquisition::startTimed(unsigned long msDuration, bool needSwTrig)
{
    this->acqusitionTimer->setInterval(msDuration);
    this->acqusitionTimer->start();
    bool result = this->start(needSwTrig);
    if(!result) this->acqusitionTimer->stop();
    return result;
}
bool Acquisition::start(bool needSwTrig)
{
    this->setState(ACQUISITION_STATES::RUNNING);
    /*
    if(!this->adqDevice->FlushDMA())
    {
        spdlog::debug("Flush DMA failed.");
    }
    if(!this->adqDevice->CollectDataNextPage())
    {
        spdlog::debug("Collect next page failed.");
    }*/
    this->writeBuffers->resetSemaphores();
    this->appConfig->getCurrentChannelConfig().log();
    spdlog::info("API: Stream start");
    this->timeStarted = std::chrono::high_resolution_clock::now();
    if(!this->adqDevice->StartStreaming())
    {
        spdlog::error("Stream failed to start!");
        return false;
    }
    this->dmaLoopStopped = false;
    this->processingLoopStopped = false;
    emit this->onStart();
    if(needSwTrig)
    {
        spdlog::debug("SWTRIG");
        this->adqDevice->SWTrig();
    }
    return true;
}
bool Acquisition::stop()
{
    this->adqDevice->StopStreaming();
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
    this->finishRecordProcessors();
    this->setState(ACQUISITION_STATES::STOPPED);
}

unsigned long Acquisition::getBuffersFill()
{
    return this->lastBuffersFilled;
}
int Acquisition::getReadQueueFill()
{
    return this->appConfig->writeBufferCount - this->writeBuffers->getReadCount();
}
int Acquisition::getWriteQueueFill()
{
    return this->appConfig->writeBufferCount - this->writeBuffers->getWriteCount();
}
