#include "Digitizer.h"
#include "AcquisitionHandler.h"
#include "ApplicationContext.h"
#include "QAcquisitionHandler.h"
#include "RecordProcessor.h"
#include "RegisterConstants.h"
#include "util.h"
#include <chrono>

Digitizer::Digitizer(ADQInterface *digitizerWrapper) : adq(digitizerWrapper) {
  this->acquisitionHandler =
      std::unique_ptr<QAcquisitionHandlerGen3>(new QAcquisitionHandlerGen3());
  this->acquisitionHandler->connect(this->acquisitionHandler.get(),
                                    &QAcquisitionHandlerGen3::stateChanged,
                                    this, &Digitizer::acquisitionStateChanged);
}

void Digitizer::appendRecordProcessor(IRecordProcessor *rp) {
  this->recordProcessors.push_back(rp);
}

void Digitizer::removeRecordProcessor(IRecordProcessor *rp) {
  void(std::remove(this->recordProcessors.begin(), this->recordProcessors.end(), rp));
}

bool Digitizer::startCustomAcquisition(
    AcquisitionConfiguration *config,
    std::vector<IRecordProcessor *> *recordProcessors) {
  return this->acquisitionHandler->requestStart(config, recordProcessors);
}

bool Digitizer::startAcquisition() {
  auto &context = ApplicationContext::get();
  return this->acquisitionHandler->requestStart(&context.config()->acq(),
                                         &this->recordProcessors);
}

bool Digitizer::stopAcquisition() { return this->acquisitionHandler->requestStop(); }

float Digitizer::dmaUsage() { return this->acquisitionHandler->dmaUsage(); }

AcquisitionStates Digitizer::state() {
  return this->acquisitionHandler->state();
}

float Digitizer::ramFill() { return this->acquisitionHandler->ramFill(); }

std::chrono::milliseconds Digitizer::durationRemaining()
{
  return this->acquisitionHandler->remainingDuration();
}