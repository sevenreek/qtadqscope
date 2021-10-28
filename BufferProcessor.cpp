#include "BufferProcessor.h"

#include <cstring>
#include "spdlog/spdlog.h"

IBufferProcessor::~IBufferProcessor() = default;
BaseBufferProcessor::BaseBufferProcessor(
    std::list<IRecordProcessor*> &recordProcessors,
    unsigned long recordLength
):
    recordLength(recordLength),
    recordProcessors(recordProcessors)
{
    for(int i = 0; i < MAX_NOF_CHANNELS; i++)
    {
        //spdlog::debug("Allocating RecordStoringProcessor's buffer {}", i);
        recordBuffer[i] = (short*)std::malloc(2UL*(size_t)recordLength*sizeof(short));
        if(recordBuffer[i] == nullptr)
        {
            spdlog::critical("Out of memory for RecordStoringProcessor's recordbuffers.");
        }
    }
}
bool BaseBufferProcessor::reallocateBuffers(unsigned long recordLength)
{
    this->recordsStored = 0;
    if(recordLength != this->recordLength)
    {
        this->recordLength = recordLength;
        for(int i = 0; i < MAX_NOF_CHANNELS; i++)
        {
            if(recordBuffer[i] != nullptr) free(recordBuffer[i]);
            //spdlog::debug("Allocating RecordStoringProcessor's buffer {}", i);
            recordBuffer[i] = (short*)std::malloc(2UL*(size_t)recordLength*sizeof(short));
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
bool BaseBufferProcessor::completeRecord(ADQRecordHeader *header, short *buffer, unsigned long sampleCount, char channel)
{
    if(recordsToStore && recordsStored >= recordsToStore)
    {
        this->status |= IRecordProcessor::STATUS::LIMIT_REACHED;
        return false;
    }
    for(auto rp : this->recordProcessors)
    {
        //spdlog::debug("Processing record");
        this->status |= rp->processRecord(header, buffer, sampleCount, channel);
    }
    recordsStored++;
    if (this->status) return false;
    return true;
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
        // if the ADC is not configured in triggered streaming mode
        // buffer processor will pass the whole buffer to the record processors
        if(!isTriggeredStreaming)
        {
            //spdlog::debug("Completing record. Samples {}. Channel {}.", unparsedSamplesInBuffer, ch);
            //buffers.headers[ch][0].Channel = ch;
            //spdlog::debug("Header in ch{}: rn:{}, ch:{}, rl:{}",ch, buffers.headers[ch][0].RecordNumber, buffers.headers[ch][0].Channel, buffers.headers[ch][0].RecordLength);
            success = this->completeRecord(nullptr, buffers.data[ch], buffers.nof_samples[ch], ch);
            if(!success) return false;
            continue;
        }
        // when triggered streaming is used the buffer has to be spliced into
        // multiple records (window of samples after trigger)
        // each record comes with a header
        unsigned long completedHeaders = 0;
        if(buffers.nof_headers[ch]>0) {
            completedHeaders = (buffers.header_status[ch] ?
                buffers.nof_headers[ch] : buffers.nof_headers[ch] - 1);
        }
        if(completedHeaders > 0)
        {
            //spdlog::debug("Samples remaining to complete record {}", samplesToCompleteRecord);
            // the sanity check below can likely be removed for a tiny performance boost, as its cause was pinpointed to be a fixed bug in WriteBuffers
            if(buffers.headers[ch][0].RecordLength != this->recordLength)
            {
                spdlog::warn("Recieved a record(#{}) longer than buffer({}). Data might be mangled. Attempting to recover.", buffers.headers[ch][0].RecordNumber, buffers.headers[ch][0].RecordLength);
                spdlog::warn("CH={}\nRNUM={}\nTSTAMP={}\nRBL={}",buffers.headers[ch][0].Channel, buffers.headers[ch][0].RecordNumber, buffers.headers[ch][0].Timestamp, this->recordBufferLength[ch]);
                buffers.headers[ch][0].RecordLength = this->recordLength;
            }
            unsigned long samplesToCompleteRecord = buffers.headers[ch][0].RecordLength - this->recordBufferLength[ch];
            std::memcpy(
                (void*)&this->recordBuffer[ch][this->recordBufferLength[ch]],
                &buffers.data[ch][0],
                samplesToCompleteRecord*sizeof(short)
            );
            //spdlog::debug("Header in ch{}: rn:{}, ch:{}, rl:{} , ts:{}",ch, buffers.headers[ch][0].RecordNumber, buffers.headers[ch][0].Channel, buffers.headers[ch][0].RecordLength, buffers.headers[ch][0].Timestamp);
            success &= this->completeRecord(
                &(buffers.headers[ch][0]),
                this->recordBuffer[ch],
                buffers.headers[ch][0].RecordLength,
                ch
            );
            unparsedSamplesInBuffer -= samplesToCompleteRecord;
            this->recordBufferLength[ch] = 0;

            for(unsigned int recordIndex = 1; recordIndex < completedHeaders; recordIndex++)
            {
                if(buffers.headers[ch][recordIndex].RecordLength != this->recordLength)
                {
                    spdlog::warn("Recieved a record(#{}) longer than buffer({}). Data might be mangled. Attempting to recover.", buffers.headers[ch][recordIndex].RecordNumber, buffers.headers[ch][recordIndex].RecordLength);
                    spdlog::warn("CH={}\nRNUM={}\nTSTAMP={}\nRSTART={}",buffers.headers[ch][recordIndex].Channel, buffers.headers[ch][recordIndex].RecordNumber, buffers.headers[ch][recordIndex].Timestamp, buffers.headers[ch][recordIndex].RecordStart);
                    buffers.headers[ch][recordIndex].RecordLength = this->recordLength;
                }

                //spdlog::debug("Header in ch{}: rn:{}, ch:{}, rl:{}",ch, buffers.headers[ch][recordIndex].RecordNumber, buffers.headers[ch][recordIndex].Channel, buffers.headers[ch][recordIndex].RecordLength);
                success &= this->completeRecord(
                    &(buffers.headers[ch][recordIndex]),
                    (short*)&(buffers.data[ch][buffers.nof_samples[ch]-unparsedSamplesInBuffer]),
                    buffers.headers[ch][recordIndex].RecordLength,
                    ch
                );
                unparsedSamplesInBuffer -= buffers.headers[ch][recordIndex].RecordLength;
            }
        }

        // it may happen that an incomplete record is obtained, e.g.
        // the record length is 100 but only 80 samples are collected
        // at the moment of the function call
        // in this case the incomplete header has to be copied to the beginning
        // of the buffer so that it can be filled on the next call(s) together with
        // the remaining 20 samples that still need to be obtained
        if(unparsedSamplesInBuffer>0)
        {
            // store the leftover samples in a buffer
            std::memcpy(
                (void*)&(this->recordBuffer[ch][this->recordBufferLength[ch]]),
                &(buffers.data[ch][buffers.nof_samples[ch]-unparsedSamplesInBuffer]),
                unparsedSamplesInBuffer*sizeof(short)
            );
            this->recordBufferLength[ch] += unparsedSamplesInBuffer;
            unparsedSamplesInBuffer = 0;
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
    this->status = IRecordProcessor::STATUS::OK;
}

void BaseBufferProcessor::resetRecordsToStore(unsigned long long recordsToStore)
{
    this->recordsStored = 0;
    this->recordsToStore = recordsToStore;
}

int BaseBufferProcessor::getStatus() const
{
    return status;
}
BaseBufferProcessor::~BaseBufferProcessor()
{
    for(int i = 0; i < MAX_NOF_CHANNELS; i++)
    {
        //spdlog::debug("Freeing RecordStoringProcessor's buffer {}", i);
        if(recordBuffer[i] != nullptr)
        {
            std::free(recordBuffer[i]);
            this->recordBuffer[i] = nullptr;
        }
    }
}
