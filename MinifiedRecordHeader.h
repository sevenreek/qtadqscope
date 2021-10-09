#ifndef MINIFIEDRECORDHEADER_H
#define MINIFIEDRECORDHEADER_H
#include "StreamingHeader.h"
#include "DigitizerConfiguration.h"
#include <cinttypes>
#include <string>
struct __attribute__ ((packed))  MinifiedRecordHeader {
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
struct __attribute__ ((packed)) MinifiedAcquisitionConfiguration {
    char fileTag[MAX_TAG_LENGTH];
    uint8_t isContinuous;
    uint8_t userLogicBypass;
    uint8_t channelMask;
    uint8_t channel;
    uint16_t sampleSkip;
    float_t obtainedInputRange;

    uint8_t triggerEdge;
    uint8_t triggerMode;
    int16_t triggerLevelCode;
    int16_t triggerLevelReset;

    int16_t digitalOffset;
    int16_t analogOffset;
    int16_t digitalGain;
    int16_t dcBias;

    uint32_t recordLength;
    uint32_t recordCount;
    uint16_t pretrigger;
    uint16_t triggerDelay;

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
