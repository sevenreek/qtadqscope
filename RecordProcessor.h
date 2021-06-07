#ifndef RECORDPROCESSOR_H
#define RECORDPROCESSOR_H
#include "StreamingHeader.h"
#include "ApplicationConfiguration.h"
class RecordProcessor {
public:
    virtual void startNewStream(ApplicationConfiguration& config) = 0;
    virtual bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount, int channel) = 0;
    virtual unsigned long long finish() = 0;
    virtual const char* getName() = 0;
    virtual ~RecordProcessor() {}
};
class FileWriter : public RecordProcessor{
public:
   virtual unsigned long long getProcessedBytes() = 0;
};

#endif
