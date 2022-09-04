#include "NullRecordProcessor.h"
#include "RecordProcessor.h"

NullRecordProcessor::NullRecordProcessor()
{

}

bool NullRecordProcessor::startNewAcquisition(AcquisitionConfiguration *acq) {
    this->bytesProcessed = 0;
    return true;
}

IRecordProcessor::STATUS NullRecordProcessor::processRecord(ADQRecord *record,
                                                         size_t bufferSize) {
    this->bytesProcessed += record->header->RecordLength * sizeof(short);
    return IRecordProcessor::STATUS::OK;
}

unsigned long long NullRecordProcessor::finish() {
  return this->bytesProcessed;
}

const char *NullRecordProcessor::getName() { return "NullRecordProcessor"; }

unsigned long long NullRecordProcessor::getProcessedBytes() {
  return this->bytesProcessed;
}
