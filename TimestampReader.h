#ifndef TIMESTAMPREADER_H
#define TIMESTAMPREADER_H

#include "RecordProcessor.h"
#include "ADQAPIIncluder.h"

class TimestampReader : public IRecordProcessor
{

public:
    TimestampReader();
    bool startNewAcquisition(Acquisition *acq) override;
    STATUS processRecord(ADQRecord *record, size_t bufferSize) override;
    unsigned long long finish() override;
    const char *getName() override;
    unsigned long long getProcessedBytes() override;
    unsigned long long getLastTimestamp() const;

private:
    unsigned long long lastTimestamp = 0;
};

#endif // TIMESTAMPREADER_H
