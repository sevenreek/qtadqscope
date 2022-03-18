#ifndef RECORDPROCESSOR_H
#define RECORDPROCESSOR_H
#include "DigitizerConfiguration.h"
#include "ADQAPIIncluder.h"
class IRecordProcessor {
public:
    enum STATUS {
      OK=0,
      LIMIT_REACHED=1,
      ERRORED=2
    };
    virtual bool startNewAcquisition(Acquisition* acq) = 0;
    virtual STATUS processRecord(ADQRecord* record, size_t bufferSize) = 0;
    virtual unsigned long long finish() = 0;
    virtual const char* getName() = 0;
    virtual ~IRecordProcessor() {}
    virtual unsigned long long getProcessedBytes() = 0;
};
#endif
