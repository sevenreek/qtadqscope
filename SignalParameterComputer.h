#ifndef SIGNALPARAMETERCOMPUTER_H
#define SIGNALPARAMETERCOMPUTER_H
#include "RecordProcessor.h"
#include <memory>
struct SignalParameters {
    double average;
    double rms;
};
extern const double MAX_SAMPLE;
class SignalParameterComputer : public IRecordProcessor
{
private:
    size_t bytesSaved;
    size_t sizeLimit;
    unsigned long long samplesSaved = 0;
    bool finished;
    double avg = 0;
    double rms = 0;
public:
    explicit SignalParameterComputer(unsigned long long sizeLimit);
    ~SignalParameterComputer();
    bool startNewAcquisition(Acquisition* config) override;
    STATUS processRecord(ADQRecord* record, size_t bufferSize) override;
    unsigned long long finish() override;
    const char* getName() override;
    unsigned long long getProcessedBytes() override;
    std::unique_ptr<SignalParameters> getResults();
};



#endif // SIGNALPARAMETERCOMPUTER_H
