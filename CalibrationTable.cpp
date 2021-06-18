#include "json.hpp"
#include "CalibrationTable.h"
#include <fstream>
bool CalibrationTable::toJSON(const char* path)
{
    std::ofstream tableStream(path, std::ios_base::out);
    std::vector<std::vector<int>> digitalData(MAX_NOF_CHANNELS);
    std::vector<std::vector<int>> analogData(MAX_NOF_CHANNELS);
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        digitalData[ch] = std::vector<int>(std::begin(this->digitalOffset[ch]), std::end(this->digitalOffset[ch]));
        analogData[ch] = std::vector<int>(std::begin(this->analogOffset[ch]), std::end(this->analogOffset[ch]));
    }
    json j = {
        {"digital", digitalData},
        {"analog",  analogData}
    };
    tableStream << j;
    tableStream.close();
    return true;
}
bool CalibrationTable::fromJSON(const char* path)
{
    std::ifstream configStream(path, std::ios_base::in);
    if(!configStream.good()) return false;
    json j;
    configStream >> j;
    int channelIndex = 0;
    for(auto &chd : j["digital"])
    {
        int inputRangeIndex = 0;
        for(auto &ird : chd)
        {
            this->digitalOffset[channelIndex][inputRangeIndex] = ird.get<int>();
            inputRangeIndex++;
        }
        channelIndex++;
    }
    channelIndex = 0;
    for(auto &cha : j["analog"])
    {
        int inputRangeIndex = 0;
        for(auto &ira : cha)
        {
            this->analogOffset[channelIndex][inputRangeIndex] = ira.get<int>();
            inputRangeIndex++;
        }
        channelIndex++;
    }
    return true;
}
