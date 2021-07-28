#ifndef ADQDEVICECONFIGURATION_H
#define ADQDEVICECONFIGURATION_H
#define MAX_NOF_CHANNELS 4
#define MAX_SAMPLING_RATE (1000000000)
#define MAX_FINITE_RECORD_COUNT (0x7FFFFFFF)
#define DEFAULT_DIGITAL_USER_GAIN (1024)
#define DEFAULT_DIGITAL_USER_OFFSET (0)



const char UNIT_PREFIXES[4]  = {' ', 'k', 'M', 'G'};
#include "json.hpp"
using json = nlohmann::json;


enum LOGGING_LEVELS
{
    DISABLED = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4
};
enum CLOCK_SOURCES
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

enum TRIGGER_MODES
{
    SOFTWARE = 1,
    EXTERNAL = 2,
    LEVEL = 3,
    INTERNAL = 4,
    EXTERNAL_2 = 7,
    EXTERNAL_3 = 8
};

enum TRIGGER_EDGES
{
    FALLING = 0,
    RISING = 1,
    BOTH = 2
};

enum INPUT_RANGES
{
    MV_100 = 0,
    MV_250 = 1,
    MV_500 = 2,
    MV_1000 = 3,
    MV_2000 = 4,
    MV_5000 = 5,
    MV_10000 = 6
};
const int INPUT_RANGE_COUNT = (7);
const float INPUT_RANGE_VALUES[INPUT_RANGE_COUNT] = {100, 250, 500, 1000, 2000, 5000, 10000};
class ChannelConfiguration {
public:
    static const unsigned int INFINITE_RECORDS = -1;
    unsigned char userLogicBypass = 0b11;
    unsigned short sampleSkip = 10000;
    int baseDcBiasOffset[INPUT_RANGE_COUNT] = {0};
    int dcBiasCode = 0;
    float dcBias = 0;
    bool triggerOffsetFromBias = false;
    INPUT_RANGES inputRangeEnum = INPUT_RANGES::MV_5000;
    float inputRangeFloat = 5000;
    float triggerLevel = 0;
    int triggerLevelCode = 0;
    int triggerLevelReset = 0;
    TRIGGER_EDGES triggerEdge = TRIGGER_EDGES::RISING;
    TRIGGER_MODES triggerMode = TRIGGER_MODES::SOFTWARE;
    unsigned int recordLength = 128;
    unsigned int recordCount = INFINITE_RECORDS;  // -1 is infty
    unsigned long pretrigger = 0;
    unsigned long triggerDelay = 0;
    int digitalOffset[INPUT_RANGE_COUNT] = {DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET,DEFAULT_DIGITAL_USER_OFFSET};
    int digitalGain = DEFAULT_DIGITAL_USER_GAIN;
    bool updateScope = true;
    bool isContinuousStreaming = true;
    void log();
    json toJSON();
    void loadFromJSON(json data);
    short getDCOffsetTriggerValue();
    short getCurrentBaseDCOffset();
    short getCurrentDigitalOffset();
    void setInputRange(INPUT_RANGES e);
    void setCurrentBaseDCOffset(int v);
    void setCurrentDigitalOffset(int v);
    int getTotalDCBias();
};
float ADCCodeToMV(float inputRange, int code);
int mvToADCCode(float inputRange, float value);
#endif // ADQDEVICECONFIGURATION_H
