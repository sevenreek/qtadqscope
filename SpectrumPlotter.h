#ifndef SPECTRUMPLOTTER_H
#define SPECTRUMPLOTTER_H

#include "AcquisitionConfiguration.h"
#include "ScopeUpdater.h"

class SpectrumPlotter : public ScopeUpdater
{
    Q_OBJECT
public:
    SpectrumPlotter(unsigned long long sampleCount);
    STATUS processRecord(ADQRecord* record, size_t bufferSize) override;
    const char* getName() override;
    bool startNewAcquisition(AcquisitionConfiguration* config) override;
signals:
    void updateSpectrumCalculatedParams(unsigned long long totalCount);
};

#endif // SPECTRUMPLOTTER_H
