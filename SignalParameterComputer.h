#ifndef SIGNALPARAMETERCOMPUTER_H
#define SIGNALPARAMETERCOMPUTER_H
#include "RecordProcessor.h"
struct SignalParameters {
    double average;
    double rms;
};

class SignalParameterComputer : public IRecordProcessor
{
private:
    unsigned long long bytesSaved;
    unsigned long long sizeLimit;
    short* dataBuffer;
    unsigned long long samplesSaved = 0;
    bool finished;
public:
    explicit SignalParameterComputer(unsigned long long sizeLimit);
    ~SignalParameterComputer();
    void startNewAcquisition(Acquisition& config);
    bool writeRecord(ADQRecordHeader* header, short* buffer, unsigned int length);
    bool processRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, int channel);
    bool writeContinuousBuffer(short* buffer, unsigned int length);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
    std::unique_ptr<SignalParameters> getResults();
};



#endif // SIGNALPARAMETERCOMPUTER_H
