#pragma once
#include "streaming_headers.hpp"
class RecordProcessor {
public:
    virtual bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount) = 0;
    virtual unsigned long long finish() = 0;
    virtual ~RecordProcessor() {}
};