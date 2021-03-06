#ifndef APPLICATIONCONFIGURATION_H
#define APPLICATIONCONFIGURATION_H

#include "ADQDeviceConfiguration.h"


#define CHANNEL_DISABLED (-1)
class ApplicationConfiguration
{
private:
    unsigned int channel = 0; // 0-indexed; 0 is channel 1/A
public:
    signed int secondChannel = CHANNEL_DISABLED;
    ChannelConfiguration channelConfig[MAX_NOF_CHANNELS];
    ChannelConfiguration& getCurrentChannelConfig();
    unsigned int writeBufferCount = 64;
    unsigned int deviceNumber = 1;
    bool settingsValid = true;
    unsigned int periodicUpdatePeriod = 100;
    unsigned long long fileSizeLimit = 1ull*1024ull*1024ull*1024ull; //16*1024ull;  // bytes

    unsigned long transferBufferCount = 32;
    unsigned long transferBufferSize = 2ul*1024ul;//1024ul * 1024ul;

    unsigned long timedRunValue = 0;

    unsigned long dmaCheckMinimumTimeout = 500;

    LOGGING_LEVELS adqLoggingLevel = LOGGING_LEVELS::DEBUG;
    LOGGING_LEVELS uiLoggingLevel = LOGGING_LEVELS::DEBUG;
    LOGGING_LEVELS fileLoggingLevel = LOGGING_LEVELS::DEBUG;
    CLOCK_SOURCES clockSource = CLOCK_SOURCES::INTSRC_INTREF_10MHZ;
    void setCurrentChannel(unsigned int ch);
    unsigned int getCurrentChannel();
    void toFile(const char* str);
    bool fromFile(const char* str);

};

#endif // APPLICATIONCONFIGURATION_H
