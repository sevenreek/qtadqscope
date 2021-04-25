#include "ADQDeviceConfiguration.h"
#include "spdlog/spdlog.h"

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
        dcBiasCode, baseDcBiasOffset,
        inputRangeFloat,
        triggerLevelCode,
        triggerEdge,
        triggerMode,
        recordLength,
        recordCount,
        pretrigger,
        triggerDelay,
        digitalGain, digitalOffset,
        isContinuousStreaming
    );
}
json ChannelConfiguration::toJSON()
{
    json j = {
        {"ul_bypass",           this->userLogicBypass},
        {"sample_skip",         this->sampleSkip},
        {"base_offset",         this->baseDcBiasOffset},
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
        {"digital_offset",      this->digitalOffset},
        {"digital_gain",        this->digitalGain},
        {"stream_type",         this->isContinuousStreaming}

    };
    return j;
}
void ChannelConfiguration::loadFromJSON(json j)
{
    this->userLogicBypass =     j["ul_bypass"];
    this->sampleSkip =          j["sample_skip"];
    this->baseDcBiasOffset =    j["base_offset"];
    this->dcBiasCode =          j["offset"];
    this->inputRangeEnum =      j["input_range"];
    this->triggerMode =         j["trigger_mode"];
    this->triggerLevelCode =    j["trigger_level"];
    this->triggerEdge =         j["trigger_edge"];
    this->triggerLevelReset =   j["trigger_reset"];
    this->recordLength =        j["record_length"];
    this->recordCount =         j["record_count"];
    this->pretrigger =          j["pretrigger"];
    this->triggerDelay =        j["trigger_delay"];
    this->digitalOffset =       j["digital_offset"];
    this->digitalGain =         j["digital_gain"];
    this->isContinuousStreaming = j["stream_type"];
}
