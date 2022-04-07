#ifndef MINIFIEDRECORDHEADER_H
#define MINIFIEDRECORDHEADER_H
#include "DigitizerConfiguration.h"
#include "ADQAPIIncluder.h"
#include <cinttypes>
#include <string>

const unsigned long CURRENT_VERSION = 2;

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

struct MinifiedRecordHeader {
    uint32_t recordLength;
    uint32_t recordNumber;
    uint64_t timestamp;
    uint16_t generalPurpose[2];
};

MinifiedRecordHeader minifyRecordHeader(const ADQRecordHeader &h);
MinifiedRecordHeader minifyRecordHeader(const ADQRecordHeader &h)
{
    MinifiedRecordHeader mrh;
    mrh.recordLength = h.RecordLength;
    mrh.recordNumber = h.RecordNumber;
    mrh.timestamp = h.Timestamp;
    mrh.generalPurpose[0] = h.GeneralPurpose0;
    mrh.generalPurpose[1] = h.GeneralPurpose1;
    return mrh;
}
const unsigned int MAX_TAG_LENGTH = 128;
struct  MinifiedAcquisitionConfiguration {
    uint64_t version = CURRENT_VERSION;
    char fileTag[MAX_TAG_LENGTH];
    uint32_t isContinuous;
    uint32_t userLogicBypass;
    uint32_t channelMask;
    uint32_t channel;
    uint32_t sampleSkip;
    double_t obtainedInputRange;

    uint32_t triggerEdge;
    uint32_t triggerMode;
    int32_t triggerLevelCode;
    int32_t triggerLevelReset;

    int32_t digitalOffset;
    int32_t analogOffset;
    int32_t digitalGain;
    int32_t dcBias;

    uint32_t recordLength;
    uint32_t recordCount;
    uint32_t pretrigger;
    uint32_t triggerDelay;

};
MinifiedAcquisitionConfiguration minifyAcquisitionConfiguration(const Acquisition &c, int channel);
MinifiedAcquisitionConfiguration minifyAcquisitionConfiguration(const Acquisition &c, int channel)
{
    MinifiedAcquisitionConfiguration returnValue;
    returnValue.version = CURRENT_VERSION;
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
