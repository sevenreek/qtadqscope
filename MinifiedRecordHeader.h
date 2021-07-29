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
    MinifiedChannelConfiguration returnValue{
        .userLogicBypass = c.userLogicBypass,
        .sampleSkip = c.sampleSkip,
        .inputRangeFloat = c.inputRangeFloat,
        .triggerEdge = (unsigned char)c.triggerEdge,
        .triggerMode = (unsigned char)c.triggerMode,
        .isStreamContinuous = (unsigned char)c.isContinuousStreaming,
        .triggerLevelCode = (short)c.triggerLevelCode,
        .triggerLevelReset = (short)c.triggerLevelReset,
        .digitalOffset = (short)c.digitalOffset[c.inputRangeEnum],
        .analogOffset = (short)c.baseDcBiasOffset[c.inputRangeEnum],
        .digitalGain = (short)c.digitalGain,
        .dcBias = (short)c.dcBiasCode,
        .recordLength = c.recordLength,
        .recordCount = c.recordCount,
        .pretrigger = (unsigned short)c.pretrigger,
        .triggerDelay = (unsigned short)c.triggerDelay
    };
    std::strncpy(returnValue.fileTag, c.fileTag.c_str(), MAX_TAG_LENGTH-1);
    returnValue.fileTag[MAX_TAG_LENGTH-1] = '\0';
    return returnValue;
}

#endif // MINIFIEDRECORDHEADER_H
