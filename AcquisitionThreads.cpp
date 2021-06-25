#include "AcquisitionThreads.h"
#include "spdlog/spdlog.h"
#include <QThread>
DMAChecker::DMAChecker(std::shared_ptr<WriteBuffers> writeBuffers, std::shared_ptr<ADQInterface> adqDevice, unsigned long transferBufferCount)
{
    this->writeBuffers = writeBuffers;
    this->adqDevice = adqDevice;
    this->transferBufferCount = transferBufferCount;
}

void DMAChecker::runLoop()
{
    this->lastBuffers = this->writeBuffers->buffers[0];
    if(this->loopActive)
    {
        spdlog::critical("DMA Checker loop already active.");
        return;
    }
    this->loopActive = true;
    while(this->loopActive)
    {
        unsigned int buffersFilled = 0;

        if(!this->adqDevice->GetTransferBufferStatus(&buffersFilled)) {
            spdlog::error("Could not get transfer buffer status. Stopping.");
            emit this->onError(); // stop if error
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
        if(buffersFilled >= this->transferBufferCount-1) {
            if(this->adqDevice->GetStreamOverflow()) {
                spdlog::error("STREAM OVERFLOWING! Some samples will be lost!", buffersFilled);
            }
            else {
                spdlog::warn("Filled {}/{} DMA buffers. Overflow likely soon!", buffersFilled, this->transferBufferCount);
            }
        }
        if(buffersFilled)
        {
            emit this->onBuffersFilled(buffersFilled);
        } else if(this->lastFilledBufferCount == 0)
        {
            QThread::msleep(SLEEP_TIME);
            //this->adqDevice->FlushDMA();
        }
        for(unsigned int b = 0; b < buffersFilled; b++) // if no buffers are filled the for loop will not start
        {
            StreamingBuffers* sbuf = nullptr;
            do{
             sbuf = this->writeBuffers->awaitWrite(1000);
             if(!this->loopActive)
             {
                 //this->writeBuffers.notifyWritten(); maybe?
                 goto DMA_CHECKER_LOOP_EXIT;
             }
            } while(sbuf == nullptr);
            //spdlog::debug("Got write lock on {}", fmt::ptr(sbuf));
            for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
            {
                if(!(sbuf->channelMask & (1<<ch))) continue;
                sbuf->headers[ch][0] = this->lastHeaders[ch];
                // copy last header
            }
            if(!this->adqDevice->GetDataStreaming(
                (void**)(sbuf->data),
                (void**)(sbuf->headers),
                sbuf->channelMask,
                sbuf->nof_samples,
                sbuf->nof_headers,
                sbuf->header_status
            )) {
                if(this->loopActive)
                {
                    spdlog::error("Could not get data stream. Stopping.");
                    emit this->onError(); // stop if error
                }
                break;
            }
            for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
            {
                if(!(sbuf->channelMask & (1<<ch))) continue;
                //spdlog::debug("Copying headers from channel {} should have {} headers", ch, sbuf->nof_headers[ch]);
                this->lastHeaders[ch] = sbuf->headers[ch][sbuf->nof_headers[ch]==0?0:(sbuf->nof_headers[ch]-1)];
            }
            //spdlog::debug("Wrote buffers. Notifying.");
            this->writeBuffers->notifyWritten();
            emit this->onBufferWritten(this->writeBuffers->sWrite.getCount());
            //spdlog::debug("Notify written");
        }
        this->lastFilledBufferCount = buffersFilled;
    }
DMA_CHECKER_LOOP_EXIT:
    this->loopActive = false;
    emit this->onLoopStopped();
    spdlog::debug("DMA loop exit");
}

void DMAChecker::stopLoop()
{
    this->loopActive = false;
    //this->writeBuffers.notifyRead();
}

void DMAChecker::setTransferBufferCount(unsigned long count)
{
    this->transferBufferCount = count;
}

LoopBufferProcessor::LoopBufferProcessor(std::shared_ptr<WriteBuffers> writeBuffers, std::shared_ptr<BufferProcessor> processor)
{
    this->writeBuffers = writeBuffers;
    this->processor = processor;
}
void LoopBufferProcessor::runLoop()
{

    if(this->loopActive)
    {
        spdlog::critical("Buffer Processor loop already active.");
        return;
    }
    this->loopActive = true;
    spdlog::debug("Processor thread active");
    while(this->loopActive)
    {
        StreamingBuffers * b = nullptr;
        do{
            b = this->writeBuffers->awaitRead(1000);
            if(!this->loopActive)
            {
                //this->writeBuffers.notifyRead(); maybe?
                goto BUFFER_PROCESSOR_LOOP_EXIT; // break if we want to join the thread
            }
        } while(b==nullptr);

        //spdlog::debug("Got read lock on {}", fmt::ptr(b));
        if(!this->processor->processBuffers(*b, this->isTriggeredStreaming)) {
            spdlog::error("Could not process buffers. Stopping.");
            emit this->onError();
            break;
        }

        //spdlog::debug("Read succeful");
        this->writeBuffers->notifyRead();
        //spdlog::debug("Read notify");
    }
BUFFER_PROCESSOR_LOOP_EXIT:
    this->loopActive = false;
    emit this->onLoopStopped();
    spdlog::debug("Buffer loop exit");
}
void LoopBufferProcessor::stopLoop()
{
    this->loopActive = false;
    //this->writeBuffers.notifyWritten();
}
void LoopBufferProcessor::changeStreamingType(bool isTriggered)
{
    this->isTriggeredStreaming = isTriggered;
}
