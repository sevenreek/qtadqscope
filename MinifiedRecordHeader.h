#ifndef MINIFIEDRECORDHEADER_H
#define MINIFIEDRECORDHEADER_H
#include "StreamingHeader.h"
#include "ADQDeviceConfiguration.h"
#include <cinttypes>
struct MinifiedRecordHeader {
    uint32_t recordLength;
    uint32_t recordNumber;
    uint64_t timestamp;
};

MinifiedRecordHeader minifyRecordHeader(const ADQRecordHeader &h);
MinifiedRecordHeader minifyRecordHeader(const ADQRecordHeader &h)
{
    return MinifiedRecordHeader {.recordLength = h.RecordLength, .recordNumber = h.RecordNumber, .timestamp = h.Timestamp};
}
const unsigned int MAX_TAG_LENGTH = 128;
struct MinifiedChannelConfiguration {
    char fileTag[MAX_TAG_LENGTH];
    unsigned char userLogicBypass;
    unsigned short sampleSkip;
    float inputRangeFloat;
    unsigned char triggerEdge;
    unsigned char triggerMode;
    unsigned char isStreamContinuous;
    short triggerLevelCode;
    short triggerLevelReset;

    short digitalOffset;
    short analogOffset;
    short digitalGain;
    short dcBias;

    unsigned int recordLength;
    unsigned int recordCount;
    unsigned short pretrigger;
    unsigned short triggerDelay;

};
MinifiedChannelConfiguration minifyChannelConfiguration(const ChannelConfiguration &c);
MinifiedChannelConfiguration minifyChannelConfiguration(const ChannelConfiguration &c)
{
    MinifiedChannelConfiguration returnValue;
    returnValue.userLogicBypass = c.userLogicBypass;
    returnValue.sampleSkip = c.sampleSkip;
    returnValue.inputRangeFloat = c.inputRangeFloat;
    returnValue.triggerEdge = (unsigned char)c.triggerEdge;
    returnValue.triggerMode = (unsigned char)c.triggerMode;
    returnValue.isStreamContinuous = (unsigned char)c.isContinuousStreaming;
    returnValue.triggerLevelCode = (short)c.triggerLevelCode;
    returnValue.triggerLevelReset = (short)c.triggerLevelReset;
    returnValue.digitalOffset = (short)c.digitalOffset[c.inputRangeEnum];
    returnValue.analogOffset = (short)c.baseDcBiasOffset[c.inputRangeEnum];
    returnValue.digitalGain = (short)c.digitalGain;
    returnValue.dcBias = (short)c.dcBiasCode;
    returnValue.recordLength = c.recordLength;
    returnValue.recordCount = c.recordCount;
    returnValue.pretrigger = (unsigned short)c.pretrigger;
    returnValue.triggerDelay = (unsigned short)c.triggerDelay;
    std::strncpy(returnValue.fileTag, c.fileTag.c_str(), MAX_TAG_LENGTH-1);
    returnValue.fileTag[MAX_TAG_LENGTH-1] = '\0';
    return returnValue;
}

#endif // MINIFIEDRECORDHEADER_H
