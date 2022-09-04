#ifndef NULLRECORDPROCESSOR_H
#define NULLRECORDPROCESSOR_H

#include "RecordProcessor.h"
#include "ADQAPIIncluder.h"

class NullRecordProcessor: public IRecordProcessor
{
private:
    unsigned long long bytesProcessed = 0;
public:
    NullRecordProcessor();
    bool startNewAcquisition(AcquisitionConfiguration* acq) override;
    STATUS processRecord(ADQRecord * record, size_t bufferSize) override;
    unsigned long long finish() override;
    const char* getName() override;
    unsigned long long getProcessedBytes() override;
};

#endif // NULLRECORDPROCESSOR_H
