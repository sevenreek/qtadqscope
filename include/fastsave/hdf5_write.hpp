#include "H5Cpp.h"
#include "record_processor.hpp"
#include <iostream>
#include <fstream>
class HDF5Writer: public RecordProcessor {
public:
    HDF5Writer(char* filename);
    bool writeRecord(StreamingHeader_t* header, short* buffer, unsigned int length);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    bool writeContinuousBuffer(short* buffer, unsigned int length);
    bool writeToDataset();
    unsigned long long finish();
};

class BinaryWriter: public RecordProcessor {
    private:
    std::ofstream continuousFile;
    std::ofstream triggeredFile;
    unsigned long long bytesSaved;
    unsigned long long sizeLimit;
    public:
    BinaryWriter(const char* cFilename, const char* tFilename, unsigned long long sizeLimit);
    ~BinaryWriter();
    bool writeRecord(StreamingHeader_t* header, short* buffer, unsigned int length);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    bool writeContinuousBuffer(short* buffer, unsigned int length); 
    unsigned long long finish();
};

class RamBufferBinaryWriter: public RecordProcessor {
private:
    std::ofstream continuousFile;
    std::ofstream triggeredFile;
    unsigned long long samplesSaved;
    unsigned long long sizeLimit;
    unsigned long long sampleLimit;
    short* storage;
public:
    RamBufferBinaryWriter(const char* cFilename, const char* tFilename, unsigned long long sizeLimit);
    ~RamBufferBinaryWriter();
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    unsigned long long finish();
};


class NullRecordProcessor: public RecordProcessor {
    public:
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    unsigned long long finish();
};

