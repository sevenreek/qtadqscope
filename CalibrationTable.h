#ifndef CALIBRATIONTABLE_H
#define CALIBRATIONTABLE_H
#include "DigitizerConstants.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
class CalibrationTable {
public:
    int analogOffset[MAX_NOF_CHANNELS][INPUT_RANGE_COUNT] = {{0}};
    int digitalOffset[MAX_NOF_CHANNELS][INPUT_RANGE_COUNT] = {{0}};
    static CalibrationTable fromJson(const QJsonObject &o);
    QJsonObject toJson();
};

#endif // CALIBRATIONTABLE_H
