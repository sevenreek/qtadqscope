#ifndef DIGITIZERCONSTANTS_H
#define DIGITIZERCONSTANTS_H
#include <climits>
#include <string>
#include "util.h"
const int MAX_NOF_CHANNELS = 4;
const unsigned long long MAX_SAMPLING_RATE = 1000000000UL;
const unsigned int MAX_FINITE_RECORD_COUNT = 0x7FFFFFFF;
const int DEFAULT_DIGITAL_USER_GAIN = 1024;
const int DEFAULT_DIGITAL_USER_OFFSET = 0;
const int DEFAULT_INPUT_RANGE = 5000;
const unsigned int INFINITE_RECORDS = -1;
const int CODE_MAX = SHRT_MAX;
const int CODE_MIN = SHRT_MIN;
const int RECORD_LENGTH_STEP = 32;
const int TRANSFER_BUFFER_SIZE_STEP = 1024;
const int PARALLEL_SAMPLES_STEP = 4;
const std::string CHANNEL_ACTIVE_EMOJI = "▶️";
const std::string TRIGGER_ACTIVE_EMOJI = "⚡";
const std::string LOGGER_PATTERN = "[%T] %v";
const unsigned int READ_USER_REGISTER_LIKE_RAM = 1;
const unsigned int READ_USER_REGISTER_LIKE_FIFO = 0;
enum class LOGGING_LEVELS
{
    DISABLED = 0,
    ERR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4
};
enum class CLOCK_SOURCES
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

enum class TRIGGER_MODES
{
    SOFTWARE = 1,
    EXTERNAL = 2,
    LEVEL = 3,
    INTERNAL = 4,
    EXTERNAL_2 = 7,
    EXTERNAL_3 = 8
};

enum class TRIGGER_EDGES
{
    FALLING = 0,
    RISING = 1,
    BOTH = 2
};

enum class INPUT_RANGES
{
    MV_100 = 0,
    MV_250 = 1,
    MV_500 = 2,
    MV_1000 = 3,
    MV_2000 = 4,
    MV_5000 = 5,
    MV_10000 = 6
};
enum class TRIGGER_APPROACHES
{
    SINGLE = 0,
    INDIVIDUAL = 1,
    CH1 = 2,
    CH2 = 3,
    CH3 = 4,
    CH4 = 5
};
enum class ACQUISITION_MODES {
    CONTINOUS = 0,
    TRIGGERED = 1
};

enum class AcquisitionStates {
    INACTIVE = 0,
    STARTING = 1,
    ACTIVE = 2,
    STOPPING = 3,
    STOPPING_ERROR = 4,
};
enum class UserLogic
{
    UL1 = 0,
    UL2 = 1
};
const int INPUT_RANGE_COUNT = (7);
const int INPUT_RANGE_VALUES[INPUT_RANGE_COUNT] = {100, 250, 500, 1000, 2000, 5000, 10000};
const std::string LOG_COLORS[7] = {
    "cyan",
    "purple",
    "black",
    "orange",
    "red",
    "darkred",
    "white"
};
#endif // DIGITIZERCONSTANTS_H
