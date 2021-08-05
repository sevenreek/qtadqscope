#ifndef DIGITIZERCONSTANTS_H
#define DIGITIZERCONSTANTS_H
#include <climits>
const int MAX_NOF_CHANNELS = 4;
const long long MAX_SAMPLING_RATE = 1000000000;
const unsigned int MAX_FINITE_RECORD_COUNT = 0x7FFFFFFF;
const int DEFAULT_DIGITAL_USER_GAIN = 1024;
const int DEFAULT_DIGITAL_USER_OFFSET = 0;
const char UNIT_PREFIXES[4]  = {' ', 'k', 'M', 'G'};
const unsigned int INFINITE_RECORDS = -1;
const int CODE_MAX = SHRT_MAX;
const int CODE_MIN = SHRT_MIN;
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

#endif // DIGITIZERCONSTANTS_H
