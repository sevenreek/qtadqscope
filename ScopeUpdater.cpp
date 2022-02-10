#include "ScopeUpdater.h"
#include "spdlog/spdlog.h"
#include <algorithm>
ScopeUpdater::ScopeUpdater(unsigned long long sampleCount)
{
    this->sampleCount = sampleCount;
    this->reallocate(sampleCount);
}
void ScopeUpdater::reallocate(unsigned long long sampleCount)
{
    this->sampleCount = sampleCount;
    this->x.clear();
    this->y.clear();
    this->x.resize(sampleCount);
    for(unsigned long s = 0; s < sampleCount; s++)
    {
        this->x[s] = (s);
    }
    this->y.resize(sampleCount);
    spdlog::debug("ScopeUpdater samlpes set to {}", sampleCount);
}
IRecordProcessor::STATUS ScopeUpdater::processRecord(ADQRecord* record, size_t bufferSize)
{
    //spdlog::debug("Updating scope with {}. Vector size {}:{}", sampleCount, this->x.size(),this->y.size());
    unsigned long maxSamples = std::min(bufferSize/sizeof(short), static_cast<size_t>(this->sampleCount));
    for(unsigned long s = 0; s < maxSamples; s++)
    {
        this->y[s] = static_cast<double>(reinterpret_cast<short*>(record->data)[s]);
    }
    emit this->onScopeUpdate(x, y); // scope must update from the GUI thread
    return STATUS::OK;
}
bool ScopeUpdater::startNewAcquisition(Acquisition* acq)
{
    if(acq->getIsContinuous()) // continuous
    {
        this->reallocate(acq->getTransferBufferSize()/sizeof(short));
    }
    else
    {
        this->reallocate(acq->getRecordLength());
    }
    return true;
}
unsigned long long ScopeUpdater::finish()
{
    return 0;
}
const char* ScopeUpdater::getName()
{
    return "ScopeUpdater";
}

unsigned long long ScopeUpdater::getProcessedBytes()
{
    return 0;
}



