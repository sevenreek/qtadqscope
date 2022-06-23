#ifndef BUFFERPROCESSOR_H
#define BUFFERPROCESSOR_H

#include "AcquisitionConfiguration.h"
#include "RecordProcessor.h"
typedef std::function<void(AcquisitionStates, AcquisitionStates)>
    AcquisitionStateChangeCallback;
class BufferProcessor {
public:
    const static float RAM_FILL_LEVELS[];
    enum class Errors {
        NO_ERROR = 0,
        INVALID_ARGUMENTS = 1,
        NO_ACQUISITION = 2,
        INTERRUPTED = 3,
        EXTERNAL = 4
    };
  BufferProcessor(ADQInterface &adq, const AcquisitionConfiguration &config,
                  const std::vector<IRecordProcessor *> &recordProcessors,
                  AcquisitionStateChangeCallback stateChangeCallback)
      : config(config), recordProcessors(recordProcessors), adq(adq),
        stateChangeCallback(stateChangeCallback) {}
  virtual ~BufferProcessor() {};
  virtual void startLoop() = 0;
  virtual void stop() = 0;
  virtual AcquisitionStates state() { return this->mState; };
  virtual float dmaUsage() = 0;
  virtual float ramUsage() = 0;
  virtual Errors errorCode() {return this->mErrorCode;};
protected:
  const AcquisitionConfiguration &config;
  const std::vector<IRecordProcessor *> &recordProcessors;
  ADQInterface &adq;
  AcquisitionStateChangeCallback stateChangeCallback;
  AcquisitionStates mState;
  Errors mErrorCode;

};

class BufferProcessorGen3 : public BufferProcessor {
public:
  using BufferProcessor::BufferProcessor;
  void startLoop() override;
  void stop() override;
  float dmaUsage() override;
  float ramUsage() override;

private:
  int lastRAMFillLevel = 0;
  std::chrono::time_point<std::chrono::high_resolution_clock> lastStarved;
  unsigned long long recordsStored = 0;
  unsigned long long lastRecordNumber = 0;
  bool completeRecord(ADQRecord *record, size_t bufferSize);
  bool handleWaitForRecordErrors(long long returnValue);
  void changeState(AcquisitionStates newState);
};

#endif