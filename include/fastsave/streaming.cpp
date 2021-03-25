#include "streaming.hpp"
#include <iostream>
//#define USEMOCKAPI

#ifdef USEMOCKAPI
#include "mockapi.hpp"
#endif
/////////////////////////////////////// SEMAPHORE STREAMING ///////////////////////

SemaphoreStreaming::SemaphoreStreaming(
    ADQInterface *adqDevice, 
    ApplicationSettings *settings, 
    RecordProcessor *recordProcessor, 
    BufferProcessor *bufferProcessor,
    WriteBuffers *writeBuffers
) : 
    adqDevice(adqDevice), 
    settings(settings), 
    recordProcessor(recordProcessor), 
    writeBuffers(writeBuffers),
    bufferProcessor(bufferProcessor) {
}
SemaphoreStreaming::~SemaphoreStreaming()
{
}
bool SemaphoreStreaming::configure() {
    spdlog::debug("Streaming::configure()");
    // maybe use ADQ_SetTransferTimeout ??
    if(!this->adqDevice->SetClockSource(this->settings->clockSource)) {spdlog::error("SetClockSource failed."); return false;};
    if(!this->adqDevice->SetTriggerMode(this->settings->triggerMode)) {spdlog::error("SetTriggerMode failed."); return false;};
    if(!this->adqDevice->SetSampleSkip(this->settings->sampleSkip)) {spdlog::error("SetSampleSkip failed."); return false;};
    for(int i = 0; i<2; i++)
    {
        if(this->settings->userLogicBypass & (1<<i))
        {
            if(!this->adqDevice->BypassUserLogic(i+1, 1)) {spdlog::error("BypassUserLogic failed"); return false;};
        }
        else 
        {
            if(!this->adqDevice->BypassUserLogic(i+1, 0)) {spdlog::error("BypassUserLogic failed."); return false;};
        }
    }
    if(!this->adqDevice->SetInputRange(this->settings->channel, this->settings->inputRange, &(this->settings->inputRange))) {spdlog::error("SetInputRange failed."); return false;};
    spdlog::info("Input range set to {}.", this->settings->inputRange);
    if(!this->adqDevice->SetAdjustableBias(this->settings->channel, this->settings->dcBiasCode+BIAS_OFFSETS_CH[this->settings->channel-1])) {spdlog::error("SetAdjustableBias failed."); return false;};
    if(!this->adqDevice->SetTransferBuffers(this->settings->transferBufferCount, this->settings->transferBufferSize)) {spdlog::error("SetTransferBuffers failed."); return false;};
    if(this->settings->recordLength == 0) // continuous
    {
        spdlog::info("Configuring continuous streaming with mask {}.", this->settings->channelMask);
        if(!this->adqDevice->ContinuousStreamingSetup(this->settings->channelMask)) {spdlog::error("ContinuousStreamingSetup failed."); return false;};
    }
    else
    {
        spdlog::info("Configuring triggered streaming.");
        if(!this->adqDevice->SetPreTrigSamples(this->settings->pretrigger)) {spdlog::error("SetPreTrigSamples failed."); return false;};
        if(!this->adqDevice->SetLvlTrigLevel(this->settings->triggerLevelCode)) {spdlog::error("SetLvlTrigLevel failed."); return false;};
        if(!this->adqDevice->SetLvlTrigChannel(this->settings->channelMask)) {spdlog::error("SetLvlTrigChannel failed."); return false;};
        if(!this->adqDevice->SetLvlTrigEdge(this->settings->triggerEdge)) {spdlog::error("SetLvlTrigEdge failed"); return false;};
        if(!this->adqDevice->TriggeredStreamingSetup(
            this->settings->recordCount,
            this->settings->recordLength,
            this->settings->pretrigger,
            0,
            this->settings->channelMask
        )) {spdlog::error("TriggeredStreamingSetup failed."); return false;};
    }
    spdlog::info("Settings changed.");
    this->settings->printSettings();
    this->configured = true;
    return this->configured;
}
void SemaphoreStreaming::start(unsigned int flushTimeoutMs)
{
    spdlog::debug("Streaming::start()");
    if(!this->configured)
    {
        spdlog::error("Cannot start an unconfigured streaming. Call Streaming::configure() first.");
        return;
    }
    this->stopped = false;
    this->writeBuffers->stopWriteThreads = false;
#ifdef USEMOCKAPI
    MOCK_BUFFER_SIZE = this->settings->transferBufferSize;
    MOCK_RECORD_LENGTH = this->settings->recordLength;
#else
    this->adqDevice->StartStreaming();

    if(this->settings->triggerMode == TRIGGER_MODE::SOFTWARE)
    {
        this->adqDevice->SWTrig();
    }
#endif
    auto timeStarted = std::chrono::system_clock::now();
    this->lastFilledBufferReceivedOn = timeStarted;
    ThreadedRecordProcessTask recordProcessTask{
        this->writeBuffers,
        this->bufferProcessor, 
        this->settings->recordLength==0?false:true
    };
    std::thread processThread{[](ThreadedRecordProcessTask &recordProcessTask) { 
        recordProcessTask.run();
    }, std::ref(recordProcessTask)};
    while(!this->stopped && // stop if streamDuration is not 0 and is over or a call to stop was made
        (std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now()-timeStarted)
        .count() < this->settings->streamDuration 
        || this->settings->streamDuration == 0)
    )
    {
        /*std::this_thread::sleep_for(std::chrono::milliseconds(this->dmaCheckWaitTime));
        printf("\rStreaming::check_dma(): %d/%d", 
        std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now()-timeStarted).count(), 
            this->settings->streamDuration);*/
        unsigned int filledBuffers = this->check_dma(flushTimeoutMs);
        if(filledBuffers == 0) { std::this_thread::sleep_for(std::chrono::milliseconds(3)); }
        /*
        if(this->dmaCheckWaitTime && filledBuffers >= this->settings->transferBufferCount-1)
        {
            this->dmaCheckWaitTime--;
            spdlog::debug("Reducing DMA wait time to {}", this->dmaCheckWaitTime);
        }*/
    }
    this->writeBuffers->stopWriteThreads = true;
    this->writeBuffers->notifyWritten(); // otherwise join might get stuck on semaphore wait()
    processThread.join();
}
void SemaphoreStreaming::stop()
{
    spdlog::debug("Streaming::stop()");
    this->writeBuffers->stopWriteThreads = true;
    this->adqDevice->StopStreaming();
    this->stopped = true;
}
unsigned int SemaphoreStreaming::check_dma(unsigned int flushTimeoutMs)
{
    unsigned int buffersFilled = 0;
#ifdef USEMOCKAPI
    buffersFilled = rand()%this->settings->transferBufferCount-1;
#else
    if(!this->adqDevice->GetTransferBufferStatus(&buffersFilled)) {
        spdlog::error("Could not get transfer buffer status. Stopping.");
        this->stop(); // stop if error
        return 0;
    }
#endif
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
    }
    if(buffersFilled >= this->settings->transferBufferCount-1) {
        if(this->adqDevice->GetStreamOverflow()) {
            spdlog::error("Filled {}/{} DMA buffers. OVERFLOWING! Some samples will be lost!", buffersFilled, this->settings->transferBufferCount);
        }
        else {
            spdlog::warn("Filled {}/{} DMA buffers. Overflow likely soon!", buffersFilled, this->settings->transferBufferCount);
        }
    }
    //spdlog::debug("Filled {} buffers.", buffersFilled);
    for(int b = 0; b < buffersFilled; b++) // if no buffers are filled the for loop will not start
    {
        StreamingBuffers* sbuf = this->writeBuffers->awaitWrite();
        //spdlog::debug("Got write lock on {}", fmt::ptr(sbuf));
#ifdef USEMOCKAPI
        MockDataStreaming(
            (void**)(sbuf->data), 
            (void**)(sbuf->headers),
            sbuf->channelMask,
            sbuf->nof_samples,
            sbuf->nof_headers,
            sbuf->header_status
        );
#else
        if(!this->adqDevice->GetDataStreaming(
            (void**)(sbuf->data), 
            (void**)(sbuf->headers),
            sbuf->channelMask,
            sbuf->nof_samples,
            sbuf->nof_headers,
            sbuf->header_status
        )) {
            spdlog::error("Could not get data stream. Stopping.");
            this->stop(); // stop if error
            return 0;
        }
#endif
        //spdlog::debug("Wrote buffers. Notifying.");
        this->writeBuffers->notifyWritten();
    }
    return buffersFilled;
}

ThreadedRecordProcessTask::ThreadedRecordProcessTask(WriteBuffers* writeBuffers, BufferProcessor* bufferProcessor, bool isTriggeredStreaming):
    writeBuffers(writeBuffers), bufferProcessor(bufferProcessor), isTriggeredStreaming(isTriggeredStreaming)
{

}
void ThreadedRecordProcessTask::stop() {
    spdlog::debug("ThreadedRecordProcessTask::stop()");
    this->writeBuffers->stopWriteThreads = true;
}   
void ThreadedRecordProcessTask::run() {
    spdlog::debug("Thread starting.");
    while(!this->writeBuffers->stopWriteThreads)
    {
        StreamingBuffers * b = this->writeBuffers->awaitRead();
        //spdlog::debug("Got read lock on {}", fmt::ptr(b));
        if(this->writeBuffers->stopWriteThreads) break; // break if we want to join the thread 
        if(!this->bufferProcessor->processBuffers(b, this->isTriggeredStreaming)) {
            spdlog::error("Could not process buffers. Stopping.");
            this->stop();
            return;
        }
        this->writeBuffers->notifyRead();
    }
    spdlog::debug("Thread ending.");
}