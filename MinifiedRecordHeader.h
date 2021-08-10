#ifndef MINIFIEDRECORDHEADER_H
#define MINIFIEDRECORDHEADER_H
#include "StreamingHeader.h"
#include "DigitizerConfiguration.h"
#include <cinttypes>
#include <string>
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
struct MinifiedAcquisitionConfiguration {
    char fileTag[MAX_TAG_LENGTH];
    unsigned char isContinuous;
    unsigned char userLogicBypass;
    unsigned char channelMask;
    unsigned char channel;
    unsigned short sampleSkip;
    unsigned long duration;
    float obtainedInputRange;

    unsigned char triggerEdge;
    unsigned char triggerMode;
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
MinifiedAcquisitionConfiguration minifyAcquisitionConfiguration(const Acquisition &c, int channel);
MinifiedAcquisitionConfiguration minifyAcquisitionConfiguration(const Acquisition &c, int channel)
{
    MinifiedAcquisitionConfiguration returnValue;
    returnValue.channel = channel;
    returnValue.channelMask = c.getChannelMask();
    returnValue.userLogicBypass = c.getUserLogicBypassMask();
    returnValue.sampleSkip = c.getSampleSkip();
    returnValue.obtainedInputRange = c.getObtainedInputRange(channel);
    returnValue.triggerEdge = (unsigned char)c.getTriggerEdge();
    returnValue.triggerMode = (unsigned char)c.getTriggerMode();
    returnValue.isContinuous = (unsigned char)c.getIsContinuous();
    returnValue.triggerLevelCode = (short)c.getTriggerLevel();
    returnValue.triggerLevelReset = (short)c.getTriggerReset();
    returnValue.digitalOffset = (short)c.getDigitalOffset(channel);
    returnValue.analogOffset = (short)c.getAnalogOffset(channel);
    returnValue.digitalGain = (short)c.getDigitalGain(channel);
    returnValue.dcBias = (short)c.getDcBias(channel);
    returnValue.recordLength = c.getRecordLength();
    returnValue.recordCount = c.getRecordCount();
    returnValue.pretrigger = (unsigned short)c.getPretrigger();
    returnValue.triggerDelay = (unsigned short)c.getTriggerDelay();
    strncpy(returnValue.fileTag, c.getTag().c_str(), MAX_TAG_LENGTH-1);
    returnValue.fileTag[MAX_TAG_LENGTH-1] = '\0';
    return returnValue;
}

#endif // MINIFIEDRECORDHEADER_H
