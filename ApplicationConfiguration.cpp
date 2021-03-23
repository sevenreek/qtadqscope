#include "ApplicationConfiguration.h"

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
