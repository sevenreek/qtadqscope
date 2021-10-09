#ifndef BINARYFILEWRITER_H
#define BINARYFILEWRITER_H
#include "RecordProcessor.h"
#include "StreamingHeader.h"
#include <iostream>
#include <fstream>

class BinaryFileWriter: public FileWriter {
protected:
    bool isContinuousStream;
    unsigned char channelMask;
    unsigned long long bytesSaved = 0;
    unsigned long long sizeLimit;
    std::ofstream dataStream[MAX_NOF_CHANNELS];
public:
    static const char ILLEGAL_CHAR_REPLACE = '_';
    explicit BinaryFileWriter(unsigned long long sizeLimit);
    ~BinaryFileWriter();
    void startNewAcquisition(Acquisition& config);
    STATUS processRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, int channel);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
};

class VerboseBinaryWriter : public BinaryFileWriter
{
public:
    explicit VerboseBinaryWriter(unsigned long long sizeLimit);
    STATUS processRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, int channel);
    ~VerboseBinaryWriter();
    const char* getName();
    void startNewAcquisition(Acquisition& config);
};

class BufferedBinaryFileWriter: public BinaryFileWriter {
    protected:
    short * dataBuffer[MAX_NOF_CHANNELS];
    unsigned long long samplesSaved[MAX_NOF_CHANNELS] = {0};
    public:
    explicit BufferedBinaryFileWriter(unsigned long long sizeLimit);
    virtual ~BufferedBinaryFileWriter();
    void startNewAcquisition(Acquisition& config);
    STATUS processRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, int channel);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
};

class VerboseBufferedBinaryWriter : public BufferedBinaryFileWriter {
public:
    explicit VerboseBufferedBinaryWriter(unsigned long long sizeLimit);
    STATUS processRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, int channel);
    ~VerboseBufferedBinaryWriter();
    const char* getName();
    void startNewAcquisition(Acquisition& config);

};

#endif // BINARYFILEWRITER_H
