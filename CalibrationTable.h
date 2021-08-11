#ifndef CALIBRATIONTABLE_H
#define CALIBRATIONTABLE_H
#include "DigitizerConstants.h"

class CalibrationTable {
public:
    int analogOffset[MAX_NOF_CHANNELS][INPUT_RANGE_COUNT] = {{0}};
    int digitalOffset[MAX_NOF_CHANNELS][INPUT_RANGE_COUNT] = {{0}};
    bool fromJSON(const char* file);
    bool toJSON(const char* file);
};

#endif // CALIBRATIONTABLE_H
