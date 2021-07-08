#include "ADQDeviceConfiguration.h"
#include "spdlog/spdlog.h"
#include <climits>

void ChannelConfiguration::log()
{
    spdlog::debug(
        "Channel configuration:\n\r"
        "\tULBYPASS: {:#b}\n\r"
        "\tSAMPLE SKIP: {}\n\r"
        "\tDC OFFSET: {}+({})\n\r"
        "\tINPUT RANGE: {}\n\r"
        "\tTRIGLVL: {}\n\r"
        "\tTRIGEDGE: {}\n\r"
        "\tTRIGMODE: {}\n\r"
        "\tRECORD LENGTH: {}\n\r"
        "\tRECORD COUNT: {}\n\r"
        "\tPRETRIGGER: {}\n\r"
        "\tTRIGDELAY: {}\n\r"
        "\tGAIN/OFFSET: {} {}\n\r"
        "\tSTREAM CONTINUOUS: {}\n\r",
        userLogicBypass,
        sampleSkip,
        dcBiasCode, getCurrentBaseDCOffset(),
        inputRangeFloat,
        triggerLevelCode,
        triggerEdge,
        triggerMode,
        recordLength,
        recordCount,
        pretrigger,
        triggerDelay,
        digitalGain, getCurrentDigitalOffset(),
        isContinuousStreaming
    );
}
json ChannelConfiguration::toJSON()
{
    std::vector<int> vBaseOffset(std::begin(this->baseDcBiasOffset), std::end(this->baseDcBiasOffset));
    std::vector<int> vDigitalOffset(std::begin(this->digitalOffset), std::end(this->digitalOffset));
    json j = {
        {"ul_bypass",           this->userLogicBypass},
        {"sample_skip",         this->sampleSkip},
        {"base_offset",         vBaseOffset},
        {"offset",              this->dcBiasCode},
        {"input_range",         this->inputRangeEnum},
        {"trigger_mode",        this->triggerMode},
        {"trigger_level",       this->triggerLevelCode},
        {"trigger_reset",       this->triggerLevelReset},
        {"trigger_edge",        this->triggerEdge},
        {"record_length",       this->recordLength},
        {"record_count",        this->recordCount},
        {"pretrigger",          this->pretrigger},
        {"trigger_delay",       this->triggerDelay},
        {"digital_offset",      vDigitalOffset},
        {"digital_gain",        this->digitalGain},
        {"stream_type",         this->isContinuousStreaming}

    };
    return j;
}
void ChannelConfiguration::loadFromJSON(json j)
{
    this->userLogicBypass =     j["ul_bypass"];
    this->sampleSkip =          j["sample_skip"];
    int inputRangeIndex = 0;
    for(auto &el : j["base_offset"])
    {
        this->baseDcBiasOffset[inputRangeIndex] = el.get<int>();
        inputRangeIndex++;
    }
    this->dcBiasCode =          j["offset"];
    this->dcBias = ADCCodeToMV(this->inputRangeFloat, this->dcBiasCode);
    this->inputRangeEnum =      j["input_range"];
    this->inputRangeFloat = INPUT_RANGE_VALUES[this->inputRangeEnum];
    this->triggerMode =         j["trigger_mode"];
    this->triggerLevelCode =    j["trigger_level"];
    this->triggerLevel = ADCCodeToMV(this->inputRangeFloat, this->triggerLevelCode);
    this->triggerEdge =         j["trigger_edge"];
    this->triggerLevelReset =   j["trigger_reset"];
    this->recordLength =        j["record_length"];
    this->recordCount =         j["record_count"];
    this->pretrigger =          j["pretrigger"];
    this->triggerDelay =        j["trigger_delay"];
    inputRangeIndex = 0;
    for(auto &el : j["digital_offset"])
    {
       this->digitalOffset[inputRangeIndex] = el.get<int>();
       inputRangeIndex++;
    }
    this->digitalGain =         j["digital_gain"];
    this->isContinuousStreaming = j["stream_type"];
}
short ChannelConfiguration::getCurrentBaseDCOffset()
{
    return this->baseDcBiasOffset[this->inputRangeEnum];
}
short ChannelConfiguration::getCurrentDigitalOffset()
{
    return this->digitalOffset[this->inputRangeEnum];
}
void ChannelConfiguration::setCurrentBaseDCOffset(int v)
{
    this->baseDcBiasOffset[this->inputRangeEnum] = v;
}
void ChannelConfiguration::setCurrentDigitalOffset(int v)
{
    this->digitalOffset[this->inputRangeEnum] = v;
}
void ChannelConfiguration::setInputRange(INPUT_RANGES e)
{
    this->inputRangeEnum = e;
    this->inputRangeFloat = INPUT_RANGE_VALUES[e];
}
short ChannelConfiguration::getDCBiasedTriggerValue()
{

    int value =  this->getCurrentBaseDCOffset()+this->dcBiasCode+this->triggerLevelCode;
    int clamped = std::max(SHRT_MIN, std::min(value, SHRT_MAX));
    if(clamped != value)
    {
        spdlog::warn("The specified trigger level is impossible to achieve. {} was clamped to {}.", value, clamped);
    }
    return (short)clamped;
}
float ADCCodeToMV(float inputRange, int code)
{
    return (float)code * (inputRange/2) / std::pow(2,15);
}


int mvToADCCode(float inputRange, float value)
{
    return std::round ( value / ( inputRange / 2 ) * std::pow(2,15) );
}
