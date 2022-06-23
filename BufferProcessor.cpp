#include "BufferProcessor.h"
#include "DigitizerConstants.h"
#include <algorithm>

const float BufferProcessor::RAM_FILL_LEVELS[] = {
    0, 1.0 / 8, 2.0 / 8, 3.0 / 8, 4.0 / 8, 5.0 / 8, 6.0 / 8, 7.0 / 8};

void BufferProcessorGen3::startLoop() {
  if (this->mState != AcquisitionStates::INACTIVE) {
    spdlog::critical("Buffer Processor loop already active.");
    return;
  }
  this->changeState(AcquisitionStates::ACTIVE);
  while (this->mState == AcquisitionStates::ACTIVE) {
    long long bufferPayloadSize;
    int channel =
        ADQ_ANY_CHANNEL; // must pass the channel to get its buffers, use
                         // ADQ_ANY_CHANNEL to capture all channels; is passed
                         // as a pointer, so the actual channel is returned
    ADQDataReadoutStatus status = {0};
    ADQRecord *record = nullptr;

#ifdef DEBUG_DMA_DELAY
#if DEBUG_DMA_DELAY > 0
    /////////////////////////////////////// // THIS IS AN ARTIFIICAL DELAY FOR
    /// TESTING OVERFLOWS.
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(
        std::chrono::duration<std::chrono::milliseconds>(DEBUG_DMA_DELAY));
    /////////////////////////////////////// //  ENSURE THAT IT IS DISABLED IN
    /// PRODUCTION!
#endif
#endif
    bufferPayloadSize = this->adq.WaitForRecordBuffer(
        &channel, reinterpret_cast<void **>(&record), 500, &status);
    if (status.flags & ADQ_DATA_READOUT_STATUS_FLAGS_STARVING) {
      this->lastStarved = std::chrono::high_resolution_clock::now();
    }
    if (bufferPayloadSize < 0) {
      if (!this->handleWaitForRecordErrors(bufferPayloadSize) &&
          this->mState != AcquisitionStates::STOPPING) {
        this->changeState(AcquisitionStates::STOPPING_ERROR);
        continue;
      }
    } else if (bufferPayloadSize == 0) {
      if (status.flags & ADQ_DATA_READOUT_STATUS_FLAGS_INCOMPLETE) {
        spdlog::warn("Record is incomplete and empty??");
      }
      continue;
    } else {
      // spdlog::debug("Got {} buffer", bufferPayloadSize);
      if (this->mState == AcquisitionStates::ACTIVE) {
        if (!this->completeRecord(record, bufferPayloadSize)) {
          changeState(AcquisitionStates::STOPPING_ERROR);
        }
      }
      this->adq.ReturnRecordBuffer(channel, record);
    }
  }
  this->changeState(AcquisitionStates::INACTIVE);
}
void BufferProcessorGen3::stop() {
  this->changeState(AcquisitionStates::STOPPING);
}
float BufferProcessorGen3::dmaUsage() {
  auto lastStarve = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - this->lastStarved);
  return std::min((100LL - lastStarve.count()), 0LL) / 100.0f;
}
float BufferProcessorGen3::ramUsage() {
  return RAM_FILL_LEVELS[this->lastRAMFillLevel];
}

bool BufferProcessorGen3::completeRecord(ADQRecord *record, size_t bufferSize) {
  int ramFill = (record->header->RecordStatus & 0b01110000) >> 4;
  int lostData = (record->header->RecordStatus & 0b00001111) >> 0;
  bool returnValue = true;
  if (ramFill != lastRAMFillLevel) {
    spdlog::debug("DRAM fill > {:.2f}%.", this->ramUsage() * 100.0f);
  }
  lastRAMFillLevel = ramFill;
  if (lostData) {
    spdlog::warn("Obtained record(#{}) with missing data - status={:#B}",
                 record->header->RecordNumber, record->header->RecordStatus);
    if (lostData & 0b1) // lost whole records before this one
    {
      spdlog::warn("Lost {} full records",
                   record->header->RecordNumber - this->lastRecordNumber);
    }
    spdlog::warn("Overflow: {}", this->adq.GetStreamOverflow());
    returnValue = true;
    // record->header->RecordLength = this->recordLength;
  }
  for (auto rp : this->recordProcessors) {
    if (rp->processRecord(record, bufferSize))
      returnValue = false;
  }
  this->lastRecordNumber = record->header->RecordNumber;
  return returnValue;
}
bool BufferProcessorGen3::handleWaitForRecordErrors(long long returnValue) {
  switch (returnValue) {
  case ADQ_EINVAL:
    spdlog::error("Invalid arguments pased to WaitForRecordBuffer");
    return false;
    break;
  case ADQ_EAGAIN:
    // spdlog::info("WaitForRecordBuffer timed out.");
    break;
  case ADQ_ENOTREADY:
    spdlog::error("WaitForRecordBuffer called despite no acquisition running.");
    return false;
    break;
  case ADQ_EINTERRUPTED:
    spdlog::error("WaitForRecordBuffer interrupted.");
    break;
  case ADQ_EEXTERNAL:
    spdlog::error("External error when calling WaitForRecordBuffer.");
    return false;
    break;
  }
  return true;
}
void BufferProcessorGen3::changeState(AcquisitionStates newState) {
  AcquisitionStates lastState = this->mState;
  this->mState = newState;
  this->stateChangeCallback(lastState, newState);
}