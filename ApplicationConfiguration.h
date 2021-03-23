#ifndef APPLICATIONCONFIGURATION_H
#define APPLICATIONCONFIGURATION_H

#include "ADQDeviceConfiguration.h"


class ApplicationConfiguration
{
private:
    unsigned int channel = 0; // 0-indexed; 0 is channel 1/A
public:
    unsigned int writeBufferCount = 8;
    unsigned int deviceNumber = 1;
    bool settingsValid = true;
    ChannelConfiguration channelConfig[MAX_NOF_CHANNELS];
    ChannelConfiguration& getCurrentChannelConfig();
    ADQDeviceConfiguration deviceConfig;
    LOGGING_LEVELS uiLoggingLevel = LOGGING_LEVELS::DEBUG;
    LOGGING_LEVELS fileLoggingLevel = LOGGING_LEVELS::DEBUG;
    CLOCK_SOURCES clockSource = CLOCK_SOURCES::INTSRC_INTREF_10MHZ;
    void setCurrentChannel(unsigned int ch);
    unsigned int getCurrentChannel();

};

#endif // APPLICATIONCONFIGURATION_H
