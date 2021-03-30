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
        {"trigger_edge",        this->triggerEdge},
        {"record_length",       this->recordLength},
        {"record_count",        this->recordCount},
        {"pretrigger",          this->pretrigger},
        {"trigger_delay",       this->triggerDelay},
        {"digital_offset",      this->digitalOffset},
        {"digital_gain",        this->digitalGain},
        {"stream_type",         this->isContinuousStreaming},
        {"file_size_limit",     this->fileSizeLimit}

    };
    return j;
}
json ADQDeviceConfiguration::toJSON()
{
    json j = {
        {"buffer_count", this->transferBufferCount},
        {"buffer_size",  this->transferBufferSize}

    };
    return j;
}
