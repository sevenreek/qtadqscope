#ifndef BINARYFILEWRITER_H
#define BINARYFILEWRITER_H
#include "RecordProcessor.h"
#include <iostream>
#include <fstream>



class BinaryFileWriter: public FileWriter {
    private:
    unsigned char channelMask;
    std::ofstream dataStream[MAX_NOF_CHANNELS];
    unsigned long long bytesSaved;
    unsigned long long sizeLimit;
    public:
    explicit BinaryFileWriter(unsigned long long sizeLimit);
    ~BinaryFileWriter();
    void startNewStream(ApplicationConfiguration& config);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
};

class BufferedBinaryFileWriter: public FileWriter {
    private:
    unsigned char channelMask;
    std::ofstream dataStream[MAX_NOF_CHANNELS];
    unsigned long long bytesSaved;
    unsigned long long sizeLimit;
    short * dataBuffer[MAX_NOF_CHANNELS];
    unsigned long long samplesSaved[MAX_NOF_CHANNELS] = {0};
    public:
    explicit BufferedBinaryFileWriter(unsigned long long sizeLimit);
    ~BufferedBinaryFileWriter();
    void startNewStream(ApplicationConfiguration& config);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
};
#endif // BINARYFILEWRITER_H
