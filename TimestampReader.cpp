#include "TimestampReader.h"

TimestampReader::TimestampReader()
{

}

bool TimestampReader::startNewAcquisition(Acquisition *acq)
{
    this->lastTimestamp = 0;
    return true;
}

IRecordProcessor::STATUS TimestampReader::processRecord(ADQRecord *record, size_t bufferSize)
{
    this->lastTimestamp = record->header->Timestamp;
    return IRecordProcessor::STATUS::OK;
}

unsigned long long TimestampReader::finish()
{
    return 0;
}

const char *TimestampReader::getName()
{
    return "TimestampReader";
}

unsigned long long TimestampReader::getProcessedBytes()
{
    return 0;
}

unsigned long long TimestampReader::getLastTimestamp() const
{
    return lastTimestamp;
}
