#include "SpectrumPlotter.h"

SpectrumPlotter::SpectrumPlotter(unsigned long long sampleCount) : ScopeUpdater(sampleCount)
{

}
IRecordProcessor::STATUS SpectrumPlotter::processRecord(ADQRecord* record, size_t bufferSize)
{
    if(this->activeChannel != record->header->Channel)
        return STATUS::OK;
    if(bufferSize/sizeof(uint32_t) < this->sampleCount) {
        spdlog::error("Spectrum Plotter is not configured properly. Expected records larger than {} for sample count set to {} > {}", bufferSize, this->sampleCount, bufferSize/sizeof(uint32_t));
        return STATUS::ERRORED;
    }
    uint32_t* spectrumData = reinterpret_cast<uint32_t*>(record->data);
    for(unsigned long s = 0; s < this->sampleCount; s++)
    {
        this->y[s] = static_cast<double>(spectrumData[s]);
    }
    unsigned long long totalCount = *reinterpret_cast<uint64_t*>(&spectrumData[this->sampleCount]);
    spdlog::debug("Processing spectrum timestamped at {} ps.", record->header->Timestamp*125);
    emit this->onScopeUpdate(x, y); // scope must update from the GUI thread
    emit this->updateSpectrumCalculatedParams(totalCount);
    return STATUS::OK;
}

const char* SpectrumPlotter::getName()
{
    return "SpectrumPlotter";
}
bool SpectrumPlotter::startNewAcquisition(Acquisition* acq)
{
    return true;
}
