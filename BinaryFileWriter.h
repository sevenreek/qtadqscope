#ifndef BINARYFILEWRITER_H
#define BINARYFILEWRITER_H
#include "RecordProcessor.h"
#include "ADQAPIIncluder.h"
#include <iostream>
#include <fstream>

class BinaryFileWriter: public IRecordProcessor {
protected:
    bool isContinuousStream;
    unsigned char channelMask;
    unsigned long long bytesSaved = 0;
    unsigned long long sizeLimit;
    std::ofstream dataStream[MAX_NOF_CHANNELS];
    unsigned long expectedRecordLength = 0;
public:
    static const char ILLEGAL_CHAR_REPLACE = '_';
    explicit BinaryFileWriter(unsigned long long sizeLimit);
    ~BinaryFileWriter();
    bool startNewAcquisition(Acquisition* acq) override;
    STATUS processRecord(ADQRecord * record, size_t bufferSize) override;
    unsigned long long finish() override;
    const char* getName() override;
    unsigned long long getProcessedBytes() override;
};

class VerboseBinaryWriter : public BinaryFileWriter
{
public:
    explicit VerboseBinaryWriter(unsigned long long sizeLimit);
    STATUS processRecord(ADQRecord * record, size_t bufferSize) override;
    ~VerboseBinaryWriter();
    const char* getName() override;
    bool startNewAcquisition(Acquisition* acq) override;
};


#endif // BINARYFILEWRITER_H
