#ifndef CALIBRATIONTABLE_H
#define CALIBRATIONTABLE_H
#include "DigitizerConstants.h"

class CalibrationTable {
public:
    int analogOffset[MAX_NOF_CHANNELS][INPUT_RANGE_COUNT] = {{0}};
    int digitalOffset[MAX_NOF_CHANNELS][INPUT_RANGE_COUNT] = {{0}};
    bool fromJson(const char* file);
    bool toJson(const char* file);
};

#endif // CALIBRATIONTABLE_H
