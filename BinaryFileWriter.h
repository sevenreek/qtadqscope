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
    std::ofstream dataStream[MAX_NOF_CHANNELS];
    unsigned long long bytesSaved;
    unsigned long long sizeLimit;
    public:
    explicit BinaryFileWriter(unsigned long long sizeLimit);
    ~BinaryFileWriter();
    void startNewStream(ApplicationConfiguration& config);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount, int channel);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
};

class BufferedBinaryFileWriter: public FileWriter {
    protected:
    bool isContinuousStream;
    unsigned char channelMask;
    std::ofstream dataStream[MAX_NOF_CHANNELS];
    unsigned long long bytesSaved;
    unsigned long long sizeLimit;
    short * dataBuffer[MAX_NOF_CHANNELS];
    unsigned long long samplesSaved[MAX_NOF_CHANNELS] = {0};
    public:
    explicit BufferedBinaryFileWriter(unsigned long long sizeLimit);
    virtual ~BufferedBinaryFileWriter();
    void startNewStream(ApplicationConfiguration& config);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount, int channel);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
};

class VerboseBufferedBinaryWriter : public BufferedBinaryFileWriter {
public:
    explicit VerboseBufferedBinaryWriter(unsigned long long sizeLimit);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount, int channel);
    ~VerboseBufferedBinaryWriter();
    unsigned long long finish();
    void startNewStream(ApplicationConfiguration& config);

};

#endif // BINARYFILEWRITER_H
