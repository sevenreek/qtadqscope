#ifndef ADQDEVICECONFIGURATION_H
#define ADQDEVICECONFIGURATION_H

#include "DeviceConstants.h"
#include "json.hpp"
using json = nlohmann::json;


class ChannelConfiguration {
public:
    unsigned char userLogicBypass = 0b11;
    unsigned short sampleSkip = 10000;
    int baseDcBiasOffset[INPUT_RANGE_COUNT] = {0};
    int dcBiasCode = 0;
    float dcBias = 0;
    INPUT_RANGES inputRangeEnum = INPUT_RANGES::MV_5000;
    float inputRangeFloat = 5000;
    float triggerLevel = 0;
    int triggerLevelCode = 0;
    int triggerLevelReset = 0;
    TRIGGER_EDGES triggerEdge = TRIGGER_EDGES::RISING;
    TRIGGER_MODES triggerMode = TRIGGER_MODES::SOFTWARE;
    unsigned int recordLength = 128;
    unsigned int recordCount = -1;  // -1 is infty
    unsigned long pretrigger = 0;
    unsigned long triggerDelay = 0;
    int digitalOffset[INPUT_RANGE_COUNT] = {DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET};
    int digitalGain = DEFAULT_DIGITAL_USER_GAIN;
    bool updateScope = true;
    bool isContinuousStreaming = true;
    void log();
    json toJSON();
    void loadFromJSON(json data);
    short getDCBiasedTriggerValue();
    short getCurrentBaseDCOffset();
    short getCurrentDigitalOffset();
    void setInputRange(INPUT_RANGES e);
    void setCurrentBaseDCOffset(int v);
    void setCurrentDigitalOffset(int v);
};
float ADCCodeToMV(float inputRange, int code);
int mvToADCCode(float inputRange, float value);
#endif // ADQDEVICECONFIGURATION_H
