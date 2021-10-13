#include "AcquisitionThreads.h"
#include "spdlog/spdlog.h"
#include <QThread>
unsigned int DMAChecker::getFlushTimeout() const
{
    return flushTimeout;
}

void DMAChecker::setFlushTimeout(unsigned int value)
{
    flushTimeout = value;
}

bool DMAChecker::isLoopStopped() const
{
    return loopStopped;
}

unsigned int DMAChecker::getLastFilledBufferCount() const
{
    return lastFilledBufferCount;
}

DMAChecker::DMAChecker(WriteBuffers &writeBuffers, ADQInterfaceWrapper &adqDevice, unsigned long transferBufferCount):
    writeBuffers(writeBuffers),  transferBufferCount(transferBufferCount), adqDevice(adqDevice)
{
}

void DMAChecker::runLoop()
{
    this->totalRecordsGathered = 0;
    if(this->shouldLoopRun)
    {
        spdlog::critical("DMA Checker loop already active.");
        return;
    }
    this->shouldLoopRun = true;
    spdlog::debug("DMA thread active");
    this->loopStopped = false;
    while(this->shouldLoopRun)
    {
        unsigned int buffersFilled = 0;

        if(!this->adqDevice.GetTransferBufferStatus(&buffersFilled)) {
            spdlog::error("Could not get transfer buffer status. Stopping.");
            emit this->onError(); // stop if error
            break;
        }
        /*
        if(buffersFilled)
        {

        }
        */
        if(buffersFilled >= this->transferBufferCount-1) {
            if(this->adqDevice.GetStreamOverflow()) {
                spdlog::error("STREAM OVERFLOW! Some samples will be lost!");
            }
            else {
                spdlog::warn("Filled {}/{} DMA buffers. Overflow likely soon!", buffersFilled, this->transferBufferCount);
            }
        }
        if(buffersFilled)
        {
            emit this->onBuffersFilled(buffersFilled);
            this->nextBufferCheckTime = std::chrono::system_clock::now() + std::chrono::milliseconds(this->flushTimeout);
        } else if(std::chrono::system_clock::now()
            > this->nextBufferCheckTime)
        {
           //spdlog::debug("Flushing DMA buffers.");
           this->adqDevice.FlushDMA();
           this->nextBufferCheckTime = std::chrono::system_clock::now() + std::chrono::milliseconds(this->flushTimeout);
        }
        for(unsigned int b = 0; b < buffersFilled; b++) // if no buffers are filled the for loop will not start
        {
            StreamingBuffers* sbuf = nullptr;
            do{
                if(!this->shouldLoopRun) // inverse the order if things break
                {
                    //this->writeBuffers.notifyWritten(); maybe?
                    //if(sbuf != nullptr) this->writeBuffers.notifyRead();
                    goto DMA_CHECKER_LOOP_EXIT;
                }
                sbuf = this->writeBuffers.awaitWrite(250);
            } while(sbuf == nullptr);
            //spdlog::debug("Got write lock on {}", fmt::ptr(sbuf));
            for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
            {
                if(!(sbuf->channelMask & (1<<ch))) continue;
                if(!this->lastStatus[ch])
                    sbuf->headers[ch][0] = this->lastHeaders[ch];
                // copy last header
            }
            if(!this->adqDevice.GetDataStreaming(
                (void**)(sbuf->data),
                (void**)(sbuf->headers),
                sbuf->channelMask,
                sbuf->nof_samples,
                sbuf->nof_headers,
                sbuf->header_status
            )) {
                if(this->shouldLoopRun)
                {
                    spdlog::error("Could not get data stream. Stopping.");
                    emit this->onError(); // stop if error
                }
                break;
            }
            for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
            {
                if(!(sbuf->channelMask & (1<<ch))) continue;
                if(!sbuf->header_status[ch] && sbuf->nof_headers[ch] > 0) {
                    ADQRecordHeader *headerPtr = reinterpret_cast<ADQRecordHeader*>(sbuf->headers[ch]);
                    this->lastHeaders[ch] = headerPtr[sbuf->nof_headers[ch] - 1];
                }
                this->lastStatus[ch] = sbuf->header_status[ch];
            }
            //spdlog::debug("Wrote buffers. Notifying.");
            this->writeBuffers.notifyWritten();
            emit this->onBufferWritten(this->writeBuffers.getWriteCount());
            //spdlog::debug("Notify written");
        }
        this->lastFilledBufferCount = buffersFilled;
    }
DMA_CHECKER_LOOP_EXIT:
    this->shouldLoopRun = false;
    this->loopStopped = true;
    emit this->onLoopStopped();
    spdlog::debug("DMA loop exit");
}

void DMAChecker::stopLoop()
{
    this->shouldLoopRun = false;
    //this->writeBuffers.notifyRead();
}

void DMAChecker::setTransferBufferCount(unsigned long count)
{
    this->transferBufferCount = count;
}

bool LoopBufferProcessor::isLoopStopped() const
{
    return loopStopped;
}

LoopBufferProcessor::LoopBufferProcessor(WriteBuffers &writeBuffers, IBufferProcessor &processor) :
    writeBuffers(writeBuffers), processor(processor)
{
}
void LoopBufferProcessor::runLoop()
{

    if(this->shouldLoopRun)
    {
        spdlog::critical("Buffer Processor loop already active.");
        return;
    }
    this->shouldLoopRun = true;
    spdlog::debug("Processor thread active");
    this->loopStopped = false;
    while(this->shouldLoopRun || this->writeBuffers.getReadCount()) // process all that are still left in memory
    {
        StreamingBuffers * b = nullptr;
        do{
            b = this->writeBuffers.awaitRead(1000);
            if(!b && !this->shouldLoopRun) // inverse the order of this if and b=.. if shit breaks
            {
                goto BUFFER_PROCESSOR_LOOP_EXIT; // break if we want to join the thread
            }
        } while(b==nullptr);

        //spdlog::debug("Got read lock on {}", fmt::ptr(b));
        if(!this->processor.processBuffers(*b, this->isTriggeredStreaming)) {
            if(this->processor.getStatus() & IRecordProcessor::STATUS::ERROR)
            {
                spdlog::error("Error while processing buffers. Stopping stream.");
            }
            else if(this->processor.getStatus() & IRecordProcessor::STATUS::LIMIT_REACHED)
            {
                spdlog::info("Record processor at limit. Stopping stream.");
            }
            else
            {
                spdlog::error("Stream stopped due to an exception.");
            }
            emit this->onError();
            break;
        }

        //spdlog::debug("Read succeful");
        this->writeBuffers.notifyRead();
        //spdlog::debug("Read notify");
    }
BUFFER_PROCESSOR_LOOP_EXIT:
    this->shouldLoopRun = false;
    this->loopStopped = true;
    emit this->onLoopStopped();
    spdlog::debug("Buffer loop exit");
}
void LoopBufferProcessor::stopLoop()
{
    this->shouldLoopRun = false;
    //this->writeBuffers.notifyWritten();
}
void LoopBufferProcessor::changeStreamingType(bool isContinuous)
{
    this->isTriggeredStreaming = !isContinuous;
}
