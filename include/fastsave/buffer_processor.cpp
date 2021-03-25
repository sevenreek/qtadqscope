#include "buffer_processor.hpp"
#include <cstring>
#include "spdlog/spdlog.h"
BufferProcessor::~BufferProcessor() = default;
RecordStoringProcessor::RecordStoringProcessor(unsigned long recordLength, RecordProcessor* rp) : recordProcessor(rp)
{
    this->recordLength = recordLength;
    for(int i = 0; i < MAX_NOF_CHANNELS; i++)
    {
        //spdlog::debug("Allocating RecordStoringProcessor's buffer {}", i);
        recordBuffer[i] = (short*)std::malloc(recordLength*sizeof(short));
        if(recordBuffer[i] == nullptr)
        {
            spdlog::critical("Out of memory for RecordStoringProcessor's recordbuffers.");
        }
    }
}
bool RecordStoringProcessor::processBuffers(StreamingBuffers *buffers, bool isTriggeredStreaming)
{
    bool success = true;
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        if((buffers->channelMask & (1<<ch)) != (1<<ch)) continue;
        
        if(buffers->nof_samples[ch] == 0)
            return true;
        unsigned long unparsedSamplesInBuffer = buffers->nof_samples[ch];
        if(unparsedSamplesInBuffer == 0)
            return true;
        if(!isTriggeredStreaming)
        {
            return this->recordProcessor->processRecord(NULL, buffers->data[ch], buffers->nof_samples[ch]);
        }
        unsigned long completedHeaders = 0;
        if(buffers->nof_headers[ch]>0) {
            completedHeaders = (buffers->header_status[ch] ?
                buffers->nof_headers[ch] : buffers->nof_headers[ch] - 1);
        }
        if(buffers->nof_samples[ch]>0)
        {
            if(completedHeaders > 0)
            {
                unsigned long samplesToCompleteRecord = buffers->headers[ch][0].RecordLength - this->recordBufferLength[ch];
                std::memcpy(
                    (void*)&(recordBuffer[ch][this->recordBufferLength[ch]]),
                    buffers->data[ch],
                    samplesToCompleteRecord*sizeof(short)
                );
                success &= recordProcessor->processRecord(
                    &(buffers->headers[ch][0]),
                    this->recordBuffer[ch],
                    buffers->headers[ch][0].RecordLength
                );
                unparsedSamplesInBuffer -= samplesToCompleteRecord;
                this->recordBufferLength[ch] = 0;
            }
            for(unsigned int recordIndex = 1; recordIndex < completedHeaders; recordIndex++)
            {
                success &= recordProcessor->processRecord(
                    &(buffers->headers[ch][recordIndex]),
                    (short*)&(buffers->data[ch][buffers->nof_samples[ch]-unparsedSamplesInBuffer]),
                    buffers->headers[ch][recordIndex].RecordLength
                );
                unparsedSamplesInBuffer -= buffers->headers[ch][recordIndex].RecordLength;
            }
            if(unparsedSamplesInBuffer>0)
            {
                std::memcpy(
                    (void*)&(this->recordBuffer[ch][this->recordBufferLength[ch]]), 
                    &(buffers->data[ch][buffers->nof_samples[ch]-unparsedSamplesInBuffer]), 
                    unparsedSamplesInBuffer*sizeof(short)
                );
                this->recordBufferLength[ch] += unparsedSamplesInBuffer;
                unparsedSamplesInBuffer = 0;
            }
            if(completedHeaders<buffers->nof_headers[ch])
            {
                std::memcpy(
                    &(buffers->headers[ch][0]), 
                    &(buffers->headers[ch][completedHeaders]), 
                    sizeof(StreamingHeader_t)
                );
            }
        }

        /*
        spdlog::debug("Channel {}. Received {} samples in {}/{} headers.", ch, buffers->nof_samples[ch], completedHeaders, buffers->nof_headers[ch]);
        
        if(completedHeaders>0)
        {
            // Handle first record:
            if(recordBufferLength[ch]>0) // some samples from incomplete record are present
            {
                unsigned long samplesRemainingToCompleteRecord = 
                    buffers->headers[ch][0].RecordLength - recordBufferLength[ch];
                std::memcpy(
                    &recordBuffer[ch][recordBufferLength[ch]],
                    samplePointer,
                    samplesRemainingToCompleteRecord*sizeof(short)
                );
                success &= recordProcessor->processRecord(
                    &buffers->headers[ch][0],
                    this->recordBuffer[ch],
                    buffers->headers[ch][0].RecordLength
                );
                samplePointer = &samplePointer[samplesRemainingToCompleteRecord]; // set pointer to after the parsed samples
                recordBufferLength[ch] = 0; // reset the counter; record completed
                unparsedSamplesInBuffer -= samplesRemainingToCompleteRecord;
                spdlog::debug("Completed 1st record {} with {} samples.", buffers->headers[ch][0].RecordNumber, samplesRemainingToCompleteRecord);
            }
            else // The first record received in a single call; might happen
            {
                success &= recordProcessor->processRecord(
                    &buffers->headers[ch][0],
                    samplePointer,
                    buffers->headers[ch][0].RecordLength
                );
                samplePointer = &samplePointer[buffers->headers[ch][0].RecordLength];
                unparsedSamplesInBuffer -= buffers->headers[ch][0].RecordLength;
                recordBufferLength[ch] = 0; // reset the counter; record completed
                spdlog::debug("Got full 1st record {} with {} samples.", buffers->headers[ch][0].RecordNumber, buffers->headers[ch][0].RecordLength);
            }
            
            // Handle other records:
            for(int recordIndex = 1; recordIndex < completedHeaders; recordIndex++)
            {
                success &= recordProcessor->processRecord(
                    &buffers->headers[ch][recordIndex],
                    samplePointer,
                    buffers->headers[ch][recordIndex].RecordLength
                );
                samplePointer = &samplePointer[buffers->headers[ch][recordIndex].RecordLength];
                unparsedSamplesInBuffer -= buffers->headers[ch][recordIndex].RecordLength;
                spdlog::debug("Got full record {} with {} samples.", buffers->headers[ch][recordIndex].RecordNumber, buffers->headers[ch][recordIndex].RecordLength);
            }
        }
        // Handle incomplete record:
        if(unparsedSamplesInBuffer>0) 
        {
            std::memcpy(&(this->recordBuffer[ch][this->recordBufferLength[ch]]), samplePointer, unparsedSamplesInBuffer*sizeof(short));
            this->recordBufferLength[ch] += unparsedSamplesInBuffer;
            spdlog::debug("Leftover {} samples. Buffer now {}.", unparsedSamplesInBuffer, this->recordBufferLength[ch]);
        }
        if(!buffers->header_status[ch]) // copy incomplete header to the beginning of buffer
        {
            spdlog::debug("Copying incomplete header.");
            std::memcpy(
                &buffers->headers[ch][0], 
                &buffers->headers[ch][completedHeaders], 
                sizeof(StreamingHeader_t)
            );
        }*/
        
    }
    return success;
}
RecordStoringProcessor::~RecordStoringProcessor()
{
    for(int i = 0; i < MAX_NOF_CHANNELS; i++)
    {
        //spdlog::debug("Freeing RecordStoringProcessor's buffer {}", i);
        std::free(recordBuffer[i]);
    }
}