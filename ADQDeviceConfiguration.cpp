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
