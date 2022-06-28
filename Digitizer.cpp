#include "Digitizer.h"
#include "AcquisitionHandler.h"
#include "QAcquisitionHandler.h"
#include "RecordProcessor.h"
#include "util.h"
#include "RegisterConstants.h"


Digitizer::Digitizer(ADQInterface *digitizerWrapper) :
    adq(digitizerWrapper),
{
    this->acquisitionHandler = std::unique_ptr<AcquisitionHandler>(new QAcquisitionHandlerGen3());
}

void Digitizer::appendRecordProcessor(IRecordProcessor *rp)
{
    this->recordProcessors.push_back(rp);
}
void Digitizer::removeRecordProcessor(IRecordProcessor *rp)
{
    std::remove(this->recordProcessors.begin(), this->recordProcessors.end(), rp);
}
bool Digitizer::startCustomAcquisition(AcquisitionConfiguration * config, std::vector<IRecordProcessor*> *recordProcessors)
{
    this->acquisitionHandler->requestStart(config, recordProcessors);
}
bool Digitizer::startAcquisition()
{
    this->acquisitionHandler->requestStart(&this->cfg().acq(), &this->recordProcessors)
}
bool Digitizer::stopAcquisition()
{
    this->acquisitionHandler->requestStop();
}