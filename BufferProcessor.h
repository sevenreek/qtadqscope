#ifndef BUFFERPROCESSOR_H
#define BUFFERPROCESSOR_H
#include "RecordProcessor.h"
#include "ADQDeviceConfiguration.h"
#include "StreamingBuffers.h"
#include <vector>
#include <list>
class BufferProcessor {
public:
    virtual bool processBuffers(StreamingBuffers &buffers, bool isTriggeredStreaming) = 0;
    virtual ~BufferProcessor() = 0;
    virtual void appendRecordProcessor(std::shared_ptr<RecordProcessor> processor) = 0;
    virtual bool removeRecordProcessor(std::shared_ptr<RecordProcessor> processor) = 0;
    virtual void clearRecordProcessors() = 0;
    virtual bool reallocateBuffers(unsigned long recordLength) = 0;
};

class BaseBufferProcessor : public BufferProcessor {
private:
    short* recordBuffer[MAX_NOF_CHANNELS] = {nullptr};
    unsigned long recordBufferLength[MAX_NOF_CHANNELS] = {0};
    unsigned long recordLength;
    std::list<std::shared_ptr<RecordProcessor>> recordProcessors;
    bool completeRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
public:
    BaseBufferProcessor(unsigned long recordLength);
    ~BaseBufferProcessor();
    bool processBuffers(StreamingBuffers &buffers, bool isTriggeredStreaming);
    void appendRecordProcessor(std::shared_ptr<RecordProcessor> processor);
    bool removeRecordProcessor(std::shared_ptr<RecordProcessor> processor);
    void clearRecordProcessors();
    bool reallocateBuffers(unsigned long recordLength);
};


#endif // BUFFERPROCESSOR_H
