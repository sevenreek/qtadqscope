#ifndef MINIFIEDRECORDHEADER_H
#define MINIFIEDRECORDHEADER_H
#include "AcquisitionConfiguration.h"
#include "ADQAPIIncluder.h"
#include <cinttypes>
#include <string>
#include "version.h"

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
    uint64_t version = APP_VERSION;
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
    int32_t horizontalShift;
    uint64_t duration;

};
MinifiedAcquisitionConfiguration minifyAcquisitionConfiguration(const AcquisitionConfiguration &c, int channel);
MinifiedAcquisitionConfiguration minifyAcquisitionConfiguration(const AcquisitionConfiguration &c, int channel)
{
    MinifiedAcquisitionConfiguration returnValue;
    returnValue.version = APP_VERSION;
    returnValue.channel = channel;
    returnValue.channelMask = c.collection.channelMask();
    returnValue.userLogicBypass = c.spectroscope.userLogicBypassMask();
    returnValue.sampleSkip = c.collection.sampleSkip();
    returnValue.obtainedInputRange = c.AFEs.at(channel).obtainedInputRange();
    returnValue.triggerEdge = (unsigned char)c.triggers.at(channel).edge();
    returnValue.triggerMode = (unsigned char)c.triggers.at(channel).mode();
    returnValue.isContinuous = (unsigned char)c.collection.isContinuous();
    returnValue.triggerLevelCode = (short)c.triggers.at(channel).level();
    returnValue.triggerLevelReset = (short)c.triggers.at(channel).reset();
    returnValue.digitalOffset = (short)c.calibrations.at(channel).digitalOffset();
    returnValue.analogOffset = (short)c.calibrations.at(channel).analogOffset();
    returnValue.digitalGain = (short)c.calibrations.at(channel).digitalGain();
    returnValue.dcBias = (short)c.AFEs.at(channel).dcBias();
    returnValue.recordLength = c.records.at(channel).recordLength();
    returnValue.recordCount = c.records.at(channel).recordCount();
    returnValue.horizontalShift = c.triggers.at(channel).horizontalShift();
    returnValue.duration = c.collection.duration();
    strncpy(returnValue.fileTag, c.storage.tag().c_str(), MAX_TAG_LENGTH-1);
    returnValue.fileTag[MAX_TAG_LENGTH-1] = '\0';
    return returnValue;
}

#endif // MINIFIEDRECORDHEADER_H
