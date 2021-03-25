#include "application_settings.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include "util.hpp"
const struct option LONG_OPTIONS[] =
    {
        {"log_level",       required_argument, 0, 'l'},
        {"dc_bias",         required_argument, 0, 'b'},
        {"input_range",     required_argument, 0, 'r'},
        {"file_limit",      required_argument, 0, 'f'},
        {"sample_skip",     required_argument, 0, 's'},
        {"duration",        required_argument, 0, 'd'},
        {"clock_source",    required_argument, 0, 'k'},
        {"trigger_edge",    required_argument, 0, 'e'},
        {"trigger_mode",    required_argument, 0, 'm'},
        {"channel",         required_argument, 0, 'c'},
        {"ul_bypass",       required_argument, 0, 'u'},
        {"buffer_count",    required_argument, 0, 'o'},
        {"buffer_size",     required_argument, 0, 'i'},
        {"record_length",   required_argument, 0, 'h'},
        {"record_count",    required_argument, 0, 't'},
        {"pretrigger",      required_argument, 0, 'p'},
        {"trigger_level",   required_argument, 0, 'v'},
        {"write_buffers",   required_argument, 0, 'w'},
        {0,0,0,0}
    };
const char* SHORT_OPTIONS = "l:b:r:f:s:d:k:e:m:c:u:o:i:h:t:p:w:";
void ApplicationSettings::parseSysArgs(int argc, char *argv[])
{
    spdlog::debug("ApplicationSettings::parseSysArgs");
    int optionIndex = 0;
    int c;
    while(
        (c = 
            getopt_long(argc, argv, 
                SHORT_OPTIONS, 
                LONG_OPTIONS, 
                &optionIndex))
        != -1)
    {
        switch(c)
        {
            case 'l': {// log_level
                this->loggingLevel = parseLoggingLevel(optarg);
            }break;
            case 'b': {// dc_bias
                this->dcBias = parseDecimal(optarg);
            }break;
            case 'r': {// input_range
                this->inputRange = parseDecimal(optarg);
            }break;
            case 'f': {// file_limit
                this->fileSizeLimit = parseDecimal(optarg);
            }break;
            case 's': {// sample_skip
                this->sampleSkip = parseDecimal(optarg);
            }break;
            case 'd': {// duration
                this->streamDuration = parseDecimal(optarg);
            }break;
            case 'k': {// clock_source
                this->clockSource = parseClockSource(optarg);
            }break;
            case 'e': {// trigger_edge
                this->triggerEdge = parseTriggerEdge(optarg);
            }break;
            case 'm': {// trigger_mode
                this->triggerMode = parseTriggerMode(optarg);
            }break;
            case 'c': {// channel
                this->channel = parseDecimal(optarg);
            }break;
            case 'u': {// ul_bypass
                try{this->userLogicBypass = parseBinary(optarg);}
                catch(const std::invalid_argument&) {
                    spdlog::warn("Failed to parse user logic bypass. Set to 0b00");
                    this->userLogicBypass = 0b00;
                }
            }break;
            case 'o': {// buffer_count
                try{this->transferBufferCount = parseDecimal(optarg);}
                catch(const std::invalid_argument&) {
                    spdlog::warn("Failed to parse buffer count. Set to 64");
                    this->transferBufferCount = 64;
                }
            }break;
            case 'i': {// buffer_size
                try{this->transferBufferSize = parseDecimal(optarg);}
                catch(const std::invalid_argument&) {
                    spdlog::warn("Failed to parse buffer size. Set to 512*1024.");
                    this->transferBufferSize = 512*1024;
                }
                if(this->transferBufferSize % 1024 != 0) {
                    this->transferBufferSize = (unsigned long)std::ceil(this->transferBufferSize/1024.0)*1024;
                    spdlog::warn("Transfer buffer size must be a multiple of 1024. Set to {}.", this->transferBufferSize);
                }
            }break;
            case 'h': {// record_length
                try{this->recordLength = parseDecimal(optarg);}
                catch(const std::invalid_argument&) {
                    spdlog::warn("Failed to parse record length. Set to 0 (continous stream).");
                    this->recordLength = 0;
                }
            }break;
            case 't': {// record_count
                try{this->recordCount = parseDecimal(optarg);}
                catch(const std::invalid_argument&) {
                    spdlog::warn("Failed to parse record count. Set to 0 (infty).");
                    this->recordCount = 0;
                }
            }break;
            case 'p': {// pretrigger
                try{this->pretrigger = parseDecimal(optarg);}
                catch(const std::invalid_argument&) {
                    spdlog::warn("Failed to parse pretrigger. Set to 0.");
                    this->pretrigger = 0;
                }
            }break;
            case 'v': {// trigger_level
                try{this->triggerLevel = parseDecimal(optarg);}
                catch(const std::invalid_argument&) {
                    spdlog::warn("Failed to parse trigger level. Set to -100.");
                    this->triggerLevel = -100;
                }
            }break;
            case 'w': {// write_buffers
                try{this->writeBufferCount = parseDecimal(optarg);}
                catch(const std::invalid_argument&) {
                    spdlog::warn("Failed to parse write buffer count. Set to 8.");
                    this->writeBufferCount = 8;
                }
            }break;
            default:
                return;
            break;
        }
    }
    if(this->dcBias > this->inputRange/2.0 || this->dcBias < -this->inputRange/2.0)
    {
        this->dcBias = util::clamp(this->dcBias, -this->inputRange/2.0, this->inputRange/2.0);
        spdlog::warn("DC Bias was set to exceed the input range. Clamped to {}", this->dcBias);
    }
    this->dcBiasCode = mvToADCCode(this->inputRange, this->dcBias);
    this->triggerLevelCode = mvToADCCode(this->inputRange, this->triggerLevel);
    this->channelMask = 1<<(this->channel-1);
}
void ApplicationSettings::printSettings()
{
    spdlog::debug("ApplicationSettings::printSettings()");
    spdlog::info("Application settings:\n\r"
        "\t channelMask = {:#b}\n\r"
        "\t userLogicBypass = {:#b}\n\r"
        "\t dcBiasCode = {}\n\r"
        "\t dcBias = {}\n\r"
        "\t inputRange = {}\n\r"
        "\t fileSizeLimit = {}\n\r"
        "\t sampleSkip = {}\n\r"
        "\t streamDuration = {}\n\r"
        "\t transferBufferCount = {}\n\r"
        "\t transferBufferSize = {}\n\r"
        "\t triggerEdge = {}\n\r"
        "\t triggerMode = {}\n\r"
        "\t clockSource = {}\n\r"
        "\t loggingLevel = {}\n\r"
        "\t recordLength = {}\n\r"
        "\t recordCount = {}\n\r"
        "\t pretrigger = {}\n\r"
        "\t triggerLevel = {}\n\r"
        "\t triggerLevelCode = {}\n\r"
        "\t writeBufferCount = {}",
        channelMask, userLogicBypass, dcBiasCode, dcBias, inputRange, fileSizeLimit, 
        sampleSkip, streamDuration, transferBufferCount, transferBufferSize, triggerEdge,
        triggerMode, clockSource, loggingLevel, recordLength, recordCount, pretrigger, 
        triggerLevel, triggerLevelCode, writeBufferCount
    );
}
unsigned long parseBinary(char* a)
{
    return std::stoi(a, 0, 2);
}
unsigned long parseDecimal(char *a)
{
    return std::stoi(a, 0, 10);
}
TRIGGER_EDGE parseTriggerEdge(char *a)
{
    if(a[0] == 'f' || a[0] == 'F')
        return TRIGGER_EDGE::FALLING;
    else
        return TRIGGER_EDGE::RISING;
}
TRIGGER_MODE parseTriggerMode(char *a)
{
    if(std::strcmp(a, "software") == 0)
        return TRIGGER_MODE::SOFTWARE;
    else if(std::strcmp(a, "external") == 0)
        return TRIGGER_MODE::EXTERNAL;
    else if(std::strcmp(a, "level") == 0)
        return TRIGGER_MODE::LEVEL;
    else if(std::strcmp(a, "internal") == 0)
        return TRIGGER_MODE::INTERNAL;
    else
        return TRIGGER_MODE::SOFTWARE;
}
CLOCK_SOURCE parseClockSource(char *a)
{
    return CLOCK_SOURCE::INTSRC_INTREF_10MHZ; // not used for now.
}
LOGGING_LEVEL parseLoggingLevel(char *a)
{
    if(std::strcmp(a, "disabled") == 0)
        return LOGGING_LEVEL::DISABLED;
    else if(std::strcmp(a, "error") == 0)
        return LOGGING_LEVEL::ERROR;
    else if(std::strcmp(a, "WARN") == 0)
        return LOGGING_LEVEL::WARN;
    else
        return LOGGING_LEVEL::INFO;
}
int mvToADCCode(float inputRange, float value)
{
    return std::round ( value / ( inputRange / 2 ) * std::pow(2,15) );
}