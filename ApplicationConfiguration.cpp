#include "ApplicationConfiguration.h"
#include <iostream>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

void ApplicationConfiguration::setCurrentChannel(unsigned int ch)
{
    this->channel = ch;
}
ChannelConfiguration& ApplicationConfiguration::getCurrentChannelConfig()
{
    return this->channelConfig[this->channel];
}

unsigned int ApplicationConfiguration::getCurrentChannel()
{
    return this->channel;
}
void ApplicationConfiguration::toFile(const char* str)
{
    std::ofstream configStream(str, std::ios_base::out);
    json j = {
        {"channel",             this->channel},
        {"write_buffer_count",  this->writeBufferCount},
        {"device_number",       this->deviceNumber},
        {"log_level_ui",        this->uiLoggingLevel},
        {"log_level_file",      this->fileLoggingLevel},
        {"clock_source",        this->clockSource},
        {"channel_configs",
         {
            this->channelConfig[0].toJSON(),
            this->channelConfig[1].toJSON(),
            this->channelConfig[2].toJSON(),
            this->channelConfig[3].toJSON()
         }
        },
        {"buffer_count", this->transferBufferCount},
        {"buffer_size",  this->transferBufferSize}
    };
    configStream << j;
    configStream.close();
}
bool ApplicationConfiguration::fromFile(const char* str)
{
    std::ifstream configStream(str, std::ios_base::in);
    if(!configStream.good()) return false;
    json j;
    configStream >> j;
    this->channel = j["channel"];
    this->writeBufferCount = j["write_buffer_count"];
    this->deviceNumber = j["device_number"];
    this->uiLoggingLevel = j["log_level_ui"];
    this->fileLoggingLevel = j["log_level_file"];
    this->clockSource = j["clock_source"];
    this->transferBufferCount = j["buffer_count"];
    this->transferBufferSize = j["buffer_size"];
    for(int i = 0; i < MAX_NOF_CHANNELS; i++) {
        this->channelConfig[i].loadFromJSON(j["channel_configs"][i]);
    }
    configStream.close();
    return true;
}
