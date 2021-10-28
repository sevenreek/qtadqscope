#ifndef RECORDPROCESSOR_H
#define RECORDPROCESSOR_H
#include "StreamingHeader.h"
#include "DigitizerConfiguration.h"
class IRecordProcessor {
public:
    enum STATUS {
      OK=0,
      LIMIT_REACHED=1,
      ERROR=2
    };
    virtual bool startNewAcquisition(Acquisition& config) = 0;
    virtual STATUS processRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, int channel) = 0;
    virtual unsigned long long finish() = 0;
    virtual const char* getName() = 0;
    virtual ~IRecordProcessor() {}
};
class FileWriter : public IRecordProcessor{
public:
   virtual unsigned long long getProcessedBytes() = 0;
};

#endif
