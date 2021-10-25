#ifndef SIGNALPARAMETERCOMPUTER_H
#define SIGNALPARAMETERCOMPUTER_H
#include "RecordProcessor.h"
#include <memory>
struct SignalParameters {
    double average;
    double rms;
};

class SignalParameterComputer : public IRecordProcessor
{
private:
    unsigned long long bytesSaved;
    size_t sizeLimit;
    short* dataBuffer;
    unsigned long long samplesSaved = 0;
    bool finished;
public:
    explicit SignalParameterComputer(unsigned long long sizeLimit);
    ~SignalParameterComputer();
    void startNewAcquisition(Acquisition& config);
    STATUS writeRecord(ADQRecordHeader* header, short* buffer, unsigned int length);
    STATUS processRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, int channel);
    STATUS writeContinuousBuffer(short* buffer, unsigned int length);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
    std::unique_ptr<SignalParameters> getResults();
};



#endif // SIGNALPARAMETERCOMPUTER_H
