#pragma once
#include "ADQAPI.h"
#include <getopt.h>
#include "spdlog/spdlog.h"
enum LOGGING_LEVEL
{
    DISABLED = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4,
};
enum CLOCK_SOURCE
{
    INTSRC_INTREF_10MHZ = 0,
    INTSRC_EXTREF_10MHZ = 1,
    EXTSRC = 2,
    INTSRC_PXIsync_10MHZ = 3,
    INTSRC_TCLKA = 4,
    INTSRC_TCLKB = 5,
    INTSRC_PXIe_100MHZ = 6,
    RESERVED = 7
};
enum TRIGGER_MODE
{
    SOFTWARE = 1,
    EXTERNAL = 2,
    LEVEL = 3,
    INTERNAL = 4,
    EXTERNAL_2 = 7,
    EXTERNAL_3 = 8
};
enum TRIGGER_EDGE
{
    FALLING = 0,
    RISING = 1
};
const short BIAS_OFFSETS_CH[4] = {260, 278, 226, 184};
extern const struct option LONG_OPTIONS[];
extern const char *SHORT_OPTIONS;
class ApplicationSettings
{
public:
    unsigned char channel = 1;
    unsigned char channelMask = 0b0001;
    unsigned char userLogicBypass = 0b10;
    int dcBiasCode = 0;
    float dcBias = 0;
    float inputRange = 2000;
    unsigned long long fileSizeLimit = 16ull*1024ull*1024ull*1024ull; //16*1024ull;  // bytes
    unsigned short sampleSkip = 1;
    unsigned long streamDuration = 2000; // ms
    unsigned long transferBufferCount = 64;
    unsigned long transferBufferSize = 1024ul*1024ul;//1024ul * 1024ul;
    unsigned long recordLength = 128; // 0 is continous stream
    unsigned int recordCount = -1;  // -1 is infty
    unsigned long pretrigger = 32;
    unsigned long writeBufferCount = 4;
    long triggerLevel = -5;
    int triggerLevelCode;
    TRIGGER_EDGE triggerEdge = TRIGGER_EDGE::RISING;
    TRIGGER_MODE triggerMode = TRIGGER_MODE::LEVEL;
    CLOCK_SOURCE clockSource = CLOCK_SOURCE::INTSRC_INTREF_10MHZ;
    LOGGING_LEVEL loggingLevel = LOGGING_LEVEL::DEBUG;
    void parseSysArgs(int argc, char *argv[]);
    void printSettings();
};

unsigned long parseBinary(char *a);
unsigned long parseDecimal(char *a);
TRIGGER_EDGE parseTriggerEdge(char *a);
TRIGGER_MODE parseTriggerMode(char *a);
CLOCK_SOURCE parseClockSource(char *a);
LOGGING_LEVEL parseLoggingLevel(char *a);
int mvToADCCode(float inputRange, float dcBiasFloat);