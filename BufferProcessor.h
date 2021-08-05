#ifndef BUFFERPROCESSOR_H
#define BUFFERPROCESSOR_H
#include "RecordProcessor.h"
#include "ADQDeviceConfiguration.h"
#include "StreamingBuffers.h"
#include <vector>
#include <list>
class IBufferProcessor {
public:
    virtual bool processBuffers(StreamingBuffers &buffers, bool isTriggeredStreaming) = 0;
    virtual ~IBufferProcessor() = 0;
    virtual bool reallocateBuffers(unsigned long recordLength) = 0;
    virtual void resetBuffers() = 0;
    virtual void resetRecordsToStore(unsigned long long recordsToStore) = 0;
};

class BaseBufferProcessor : public IBufferProcessor {
private:
    // buffer for storing incomplete record's samples
    short* recordBuffer[MAX_NOF_CHANNELS] = {nullptr};

    // count of samples stored from the last incomplete header
    unsigned long recordBufferLength[MAX_NOF_CHANNELS] = {0};
    // expected record length, used for sanity checks and debugging only, the real record length is pulled from headers
    unsigned long recordLength;
    // record completion listeners
    std::list<std::reference_wrapper<IRecordProcessor>> &recordProcessors;
    // notify record listeners (processors)
    bool completeRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, char channel);
    unsigned long long recordsStored = 0;
    unsigned long long recordsToStore = 0;
public:
    BaseBufferProcessor(std::list<std::reference_wrapper<IRecordProcessor>> &recordProcessors, unsigned long recordLength);
    ~BaseBufferProcessor();
    bool processBuffers(StreamingBuffers &buffers, bool isTriggeredStreaming);
    // call if the record length changes, as the internal buffers for storing incomplete records must be reallocated
    bool reallocateBuffers(unsigned long recordLength);
    // should be called in between acquisitions;
    // if an incomplete record was stored in the buffer in between the next record could potentially exceed the allocated buffers;
    // reallocateBuffers() calls this function, so no need to call it manually if the record length changes
    void resetBuffers();
    void resetRecordsToStore(unsigned long long recordsToStore);
};


#endif // BUFFERPROCESSOR_H
