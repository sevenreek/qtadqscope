#ifndef BINARYFILEWRITER_H
#define BINARYFILEWRITER_H
#include "RecordProcessor.h"
#include <iostream>
#include <fstream>



class BinaryFileWriter: public FileWriter {
    private:
    std::ofstream dataStream;
    unsigned long long bytesSaved;
    unsigned long long sizeLimit;
    public:
    BinaryFileWriter(unsigned long long sizeLimit);
    ~BinaryFileWriter();
    void startNewStream(ApplicationConfiguration& config);
    bool writeRecord(StreamingHeader_t* header, short* buffer, unsigned int length);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    bool writeContinuousBuffer(short* buffer, unsigned int length);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
};

class BufferedBinaryFileWriter: public FileWriter {
    private:
    std::ofstream dataStream;
    unsigned long long bytesSaved;
    unsigned long long sizeLimit;
    short* dataBuffer;
    unsigned long long samplesSaved = 0;
    public:
    BufferedBinaryFileWriter(unsigned long long sizeLimit);
    ~BufferedBinaryFileWriter();
    void startNewStream(ApplicationConfiguration& config);
    bool writeRecord(StreamingHeader_t* header, short* buffer, unsigned int length);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    bool writeContinuousBuffer(short* buffer, unsigned int length);
    unsigned long long finish();
    const char* getName();
    unsigned long long getProcessedBytes();
};
#endif // BINARYFILEWRITER_H
