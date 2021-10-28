#include "ScopeUpdater.h"
#include "spdlog/spdlog.h"
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
}
IRecordProcessor::STATUS ScopeUpdater::processRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, int channel)
{
    //spdlog::debug("Updating scope with {}. Vector size {}:{}", sampleCount, this->x.size(),this->y.size());
    for(unsigned long s = 0; s < sampleCount; s++)
    {
        this->y[s] = (double)(buffer[s]);
    }
    emit this->onScopeUpdate(x, y); // scope must update from the GUI thread
    return STATUS::OK;
}
bool ScopeUpdater::startNewAcquisition(Acquisition& config)
{
    if(config.getIsContinuous()) // continuous
    {
        this->reallocate(config.getTransferBufferSize()/sizeof(short));
    }
    else
    {
        this->reallocate(config.getRecordLength());
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



