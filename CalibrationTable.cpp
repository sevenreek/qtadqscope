#include "json.hpp"
#include "CalibrationTable.h"
using json = nlohmann::json;

QJsonObject CalibrationTable::toJson()
{
    QJsonArray digitalData;
    QJsonArray analogData;
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        QJsonArray digCh;
        QJsonArray anaCh;
        digitalData.append(digCh);
        analogData.append(anaCh);
        for(int ir = 0; ir < INPUT_RANGE_COUNT; ir++)
        {
            digCh.append(this->digitalOffset[ch][ir]);
            anaCh.append(this->analogOffset[ch][ir]);
        }
    }
    QJsonObject j = {
        {"digital", digitalData},
        {"analog",  analogData}
    };
    return j;
}
CalibrationTable CalibrationTable::fromJson(const QJsonObject &o)
{
    CalibrationTable table;
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        for(int ir = 0; ir < INPUT_RANGE_COUNT; ir++)
        {
            table.digitalOffset[ch][ir] = o["digital"].toArray()[ch].toArray()[ir].toInt(0);
            table.analogOffset[ch][ir] =  o["analog"].toArray()[ch].toArray()[ir].toInt(0);
        }
    }
    return table;
}

