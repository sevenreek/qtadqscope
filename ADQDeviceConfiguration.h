#ifndef ADQDEVICECONFIGURATION_H
#define ADQDEVICECONFIGURATION_H
#define MAX_NOF_CHANNELS 4
#define MAX_SAMPLING_RATE (1000000000)
#define MAX_FINITE_RECORD_COUNT (0x7FFFFFFF)
#define DEFAULT_DIGITAL_USER_GAIN (1024)
#define DEFAULT_DIGITAL_USER_OFFSET (0)
const char UNIT_PREFIXES[4]  = {' ', 'k', 'M', 'G'};

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

const float INPUT_RANGE_VALUES[] = {100, 250, 500, 1000, 2000, 5000, 10000};
class ChannelConfiguration {
public:
    unsigned char userLogicBypass = 0b11;
    unsigned short sampleSkip = 10000;
    int baseDcBiasOffset = 0;
    int dcBiasCode = 0;
    float dcBias = 0;
    INPUT_RANGES inputRangeEnum = INPUT_RANGES::MV_5000;
    float inputRangeFloat = 5000;
    float triggerLevel = 0;
    int triggerLevelCode = 0;
    TRIGGER_EDGES triggerEdge = TRIGGER_EDGES::RISING;
    TRIGGER_MODES triggerMode = TRIGGER_MODES::SOFTWARE;
    unsigned long recordLength = 128;
    unsigned int recordCount = -1;  // -1 is infty
    unsigned long pretrigger = 0;
    unsigned long triggerDelay = 0;
    int digitalOffset = DEFAULT_DIGITAL_USER_OFFSET;
    int digitalGain = DEFAULT_DIGITAL_USER_GAIN;
    bool updateScope = true;
    bool isContinuousStreaming = true;
    void log();
};

class ADQDeviceConfiguration
{
public:

    unsigned long long fileSizeLimit = 16ull*1024ull*1024ull*1024ull; //16*1024ull;  // bytes
    unsigned long transferBufferCount = 64;
    unsigned long transferBufferSize = 1ul*1024ul;//1024ul * 1024ul;
    CLOCK_SOURCES clockSource = CLOCK_SOURCES::INTSRC_INTREF_10MHZ;

    LOGGING_LEVELS adqLoggingLevel = LOGGING_LEVELS::DEBUG;
};


#endif // ADQDEVICECONFIGURATION_H
