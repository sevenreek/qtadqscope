#pragma once
#include "streaming_headers.hpp"
#include "record_processor.hpp"
class BufferProcessor {
public:
    virtual bool processBuffers(StreamingBuffers *buffers, bool isTriggeredStreaming) = 0;
    virtual ~BufferProcessor() = 0;
};

class RecordStoringProcessor : public BufferProcessor {
private:
    short* recordBuffer[MAX_NOF_CHANNELS];
    unsigned long recordBufferLength[MAX_NOF_CHANNELS] = {0};
    unsigned long recordLength;
    RecordProcessor* recordProcessor;
public:
    RecordStoringProcessor(unsigned long recordLength, RecordProcessor* rp);
    ~RecordStoringProcessor();
    bool processBuffers(StreamingBuffers *buffers, bool isTriggeredStreaming);
};

class RawThreadedBufferProcessor : public BufferProcessor {
private:
    short* recordBuffer[MAX_NOF_CHANNELS];
    unsigned long recordBufferLength[MAX_NOF_CHANNELS];
    unsigned long recordLength;
public:
    RawThreadedBufferProcessor();
    ~RawThreadedBufferProcessor();
    bool processBuffers(StreamingBuffers *buffers, bool isTriggeredStreaming);
};