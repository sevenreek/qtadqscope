#ifndef RECORDPROCESSOR_H
#define RECORDPROCESSOR_H
#include "StreamingHeader.h"
#include "ApplicationConfiguration.h"
class RecordProcessor {
public:
    virtual void startNewStream(ApplicationConfiguration& config) = 0;
    virtual bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount) = 0;
    virtual unsigned long long finish() = 0;
    virtual const char* getName() = 0;
    virtual ~RecordProcessor() {}
};
#endif
