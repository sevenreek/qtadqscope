#include "Digitizer.h"
#include "AcquisitionHandler.h"
#include "QAcquisitionHandler.h"
#include "RecordProcessor.h"
#include "RegisterConstants.h"
#include "util.h"

Digitizer::Digitizer(ADQInterface *digitizerWrapper) : adq(digitizerWrapper), {
  this->acquisitionHandler =
      std::unique_ptr<QAcquisitionHandlerGen3>(new QAcquisitionHandlerGen3());
  this->acquisitionHandler->connect(this->acquisitionHandler.get(),
                                    &QAcquisitionHandlerGen3::stateChanged,
                                    this, &Digitizer::acquisiitionStateChanged);
}

void Digitizer::appendRecordProcessor(IRecordProcessor *rp) {
  this->recordProcessors.push_back(rp);
}
void Digitizer::removeRecordProcessor(IRecordProcessor *rp) {
  std::remove(this->recordProcessors.begin(), this->recordProcessors.end(), rp);
}
bool Digitizer::startCustomAcquisition(
    AcquisitionConfiguration *config,
    std::vector<IRecordProcessor *> *recordProcessors) {
  this->acquisitionHandler->requestStart(config, recordProcessors);
}
bool Digitizer::startAcquisition() {
  this->acquisitionHandler->requestStart(&this->cfg().acq(),
                                         &this->recordProcessors)
}
bool Digitizer::stopAcquisition() { this->acquisitionHandler->requestStop(); }