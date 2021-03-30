#ifndef BINARYFILEWRITER_H
#define BINARYFILEWRITER_H
#include "RecordProcessor.h"
#include <iostream>
#include <fstream>
class BinaryFileWriter: public RecordProcessor {
    private:
    std::ofstream dataStream;
    unsigned long long bytesSaved;
    unsigned long long sizeLimit;
    public:
    BinaryFileWriter();
    ~BinaryFileWriter();
    void startNewStream(ApplicationConfiguration& config);
    bool writeRecord(StreamingHeader_t* header, short* buffer, unsigned int length);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    bool writeContinuousBuffer(short* buffer, unsigned int length);
    unsigned long long finish();
};
#endif // BINARYFILEWRITER_H
