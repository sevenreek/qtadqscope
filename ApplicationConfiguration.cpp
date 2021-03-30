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
        {"device_config",       this->deviceConfig.toJSON()}
    };
    configStream << j;
    configStream.close();
}
void ApplicationConfiguration::fromFile(const char* str)
{
    std::ifstream configStream;

}
