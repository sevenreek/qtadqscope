#include "BufferProcessor.h"

#include <cstring>
#include "spdlog/spdlog.h"

BufferProcessor::~BufferProcessor() = default;
BaseBufferProcessor::BaseBufferProcessor(
    std::list<std::shared_ptr<RecordProcessor>> &recordProcessors,
    unsigned long recordLength
):
    recordProcessors(recordProcessors)
{
    this->reallocateBuffers(recordLength);
}
bool BaseBufferProcessor::reallocateBuffers(unsigned long recordLength)
{
    if(recordLength != this->recordLength)
    {
        this->recordLength = recordLength;
        for(int i = 0; i < MAX_NOF_CHANNELS; i++)
        {
            //spdlog::debug("Allocating RecordStoringProcessor's buffer {}", i);
            recordBuffer[i] = (short*)std::realloc(recordBuffer[i], recordLength*sizeof(short));
            if(recordBuffer[i] == nullptr)
            {
                spdlog::critical("Out of memory for RecordStoringProcessor's recordbuffers.");
                return false;
            }
        }
    }
    this->resetBuffers();
    return true;
}
bool BaseBufferProcessor::completeRecord(StreamingHeader_t *header, short *buffer, unsigned long sampleCount)
{
    bool success = true;
    for(auto rp : this->recordProcessors)
    {
        //spdlog::debug("Processing record");
        success &= rp->processRecord(header, buffer, sampleCount);
    }
    return success;
}
bool BaseBufferProcessor::processBuffers(StreamingBuffers &buffers, bool isTriggeredStreaming)
{
    bool success = true;
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        if((buffers.channelMask & (1<<ch)) != (1<<ch)) continue;

        if(buffers.nof_samples[ch] == 0)
            continue;
        unsigned long unparsedSamplesInBuffer = buffers.nof_samples[ch];
        if(!isTriggeredStreaming)
        {
            //spdlog::debug("Completing record. Samples {}. Channel {}.", unparsedSamplesInBuffer, ch);
            this->completeRecord(NULL, buffers.data[ch], buffers.nof_samples[ch]);
            continue;
        }
        unsigned long completedHeaders = 0;
        if(buffers.nof_headers[ch]>0) {
            completedHeaders = (buffers.header_status[ch] ?
                buffers.nof_headers[ch] : buffers.nof_headers[ch] - 1);
        }
        if(buffers.nof_samples[ch]>0)
        {
            if(completedHeaders > 0)
            {
                unsigned long samplesToCompleteRecord = buffers.headers[ch][0].RecordLength - this->recordBufferLength[ch];
                if(buffers.headers[ch][0].RecordLength <= this->recordLength)
                {
                    std::memcpy(
                        &(this->recordBuffer[ch][this->recordBufferLength[ch]]),
                        buffers.data[ch],
                        samplesToCompleteRecord*sizeof(short)
                    );
                    success &= this->completeRecord(
                        &(buffers.headers[ch][0]),
                        this->recordBuffer[ch],
                        buffers.headers[ch][0].RecordLength
                    );
                    unparsedSamplesInBuffer -= samplesToCompleteRecord;
                    this->recordBufferLength[ch] = 0;
                }
                else
                {
                    spdlog::warn("Recieved a record longer than buffer({}). Data mangled or buffer set incorrectly.", buffers.headers[ch][0].RecordLength);
                    return false;
                }
            }
            for(unsigned int recordIndex = 1; recordIndex < completedHeaders; recordIndex++)
            {
                success &= this->completeRecord(
                    &(buffers.headers[ch][recordIndex]),
                    (short*)&(buffers.data[ch][buffers.nof_samples[ch]-unparsedSamplesInBuffer]),
                    buffers.headers[ch][recordIndex].RecordLength
                );
                unparsedSamplesInBuffer -= buffers.headers[ch][recordIndex].RecordLength;
            }
            if(unparsedSamplesInBuffer>0)
            {
                std::memcpy(
                    (void*)&(this->recordBuffer[ch][this->recordBufferLength[ch]]),
                    &(buffers.data[ch][buffers.nof_samples[ch]-unparsedSamplesInBuffer]),
                    unparsedSamplesInBuffer*sizeof(short)
                );
                this->recordBufferLength[ch] += unparsedSamplesInBuffer;
                unparsedSamplesInBuffer = 0;
            }
            if(completedHeaders<buffers.nof_headers[ch])
            {
                std::memcpy(
                    &(buffers.headers[ch][0]),
                    &(buffers.headers[ch][completedHeaders]),
                    sizeof(StreamingHeader_t)
                );
            }
        }

    }
    return success;
}

void BaseBufferProcessor::resetBuffers()
{
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        this->recordBufferLength[ch] = 0;
    }
}
BaseBufferProcessor::~BaseBufferProcessor()
{
    for(int i = 0; i < MAX_NOF_CHANNELS; i++)
    {
        //spdlog::debug("Freeing RecordStoringProcessor's buffer {}", i);
        if(recordBuffer[i] != nullptr)
        {
            std::free(recordBuffer[i]);
        }
    }
}
