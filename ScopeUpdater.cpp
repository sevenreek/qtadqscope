#include "ScopeUpdater.h"
#include "spdlog/spdlog.h"
#include <algorithm>
ScopeUpdater::ScopeUpdater(unsigned long long sampleCount) {
  this->sampleCount = sampleCount;
  this->reallocate(sampleCount);
}
void ScopeUpdater::reallocate(unsigned long long sampleCount) {
  this->sampleCount = sampleCount;
  this->x.clear();
  this->y.clear();
  this->x.resize(sampleCount);
  for (unsigned long s = 0; s < sampleCount; s++) {
    this->x[s] = (s);
  }
  this->y.resize(sampleCount);
}
IRecordProcessor::STATUS ScopeUpdater::processRecord(ADQRecord *record,
                                                     size_t bufferSize) {
  if (this->activeChannel != record->header->Channel)
    return STATUS::OK;
  size_t maxSamples = std::min(bufferSize / sizeof(short),
                               static_cast<size_t>(this->sampleCount));
  for (int s = 0; s < maxSamples; s++) {
    this->y[s] =
        static_cast<double>(reinterpret_cast<short *>(record->data)[s]);
  }
  emit this->onScopeUpdate(x, y); // scope must update from the GUI thread
  return STATUS::OK;
}
bool ScopeUpdater::startNewAcquisition(AcquisitionConfiguration *acq) {
  if (acq->collection.isContinuous()) // continuous
  {
    this->reallocate(acq->transfer.bufferSize() / sizeof(short));
  } else {
    // Find biggest record size amongst channels
    this->reallocate(acq->maxRecordLength());
  }
  return true;
}
unsigned long long ScopeUpdater::finish() { return 0; }
const char *ScopeUpdater::getName() { return "ScopeUpdater"; }

unsigned long long ScopeUpdater::getProcessedBytes() { return 0; }
void ScopeUpdater::changeChannel(int ch) { this->activeChannel = ch; }
