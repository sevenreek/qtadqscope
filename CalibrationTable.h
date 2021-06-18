#ifndef CALIBRATIONTABLE_H
#define CALIBRATIONTABLE_H
#include "ADQDeviceConfiguration.h"

class CalibrationTable {
public:
    int analogOffset[MAX_NOF_CHANNELS][INPUT_RANGE_COUNT];
    int digitalOffset[MAX_NOF_CHANNELS][INPUT_RANGE_COUNT];
    bool fromJSON(const char* file);
    bool toJSON(const char* file);
};

#endif // CALIBRATIONTABLE_H
