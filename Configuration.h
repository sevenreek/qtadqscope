#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "DigitizerConstants.h"
#include <string>
#include <array>
#include <QJsonObject>
#include "spdlog/spdlog.h"


class IConfiguration {
public:
    virtual QJsonObject toJSON() = 0;
};

class ChannelConfiguration : public IConfiguration {
protected:
    int channelIndex = -1;
public:
    const static int DEFAULT_NO_CHANNEL; // constant to signify that the channel config applies to a default/all channels
    ChannelConfiguration(int channelIndex);
};

#endif