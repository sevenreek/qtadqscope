#include <cstdint>
#include <memory>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>

struct MinifiedRecordHeader {
    uint32_t recordLength;
    uint32_t recordNumber;
    uint64_t timestamp;
};

struct MinifiedChannelConfiguration {
    unsigned char userLogicBypass;
    unsigned short sampleSkip;
    float inputRangeFloat;
    unsigned char triggerEdge;
    unsigned char triggerMode;
    unsigned char isStreamContinuous;
    short triggerLevelCode;
    short triggerLevelReset;

    short digitalOffset;
    short analogOffset;
    short digitalGain;
    short dcBias;

    unsigned int recordLength;
    unsigned int recordCount;
    unsigned short pretrigger;
    unsigned short triggerDelay;

};

static const std::string INPUT_FILE = "0625_201018_ch1.dat";

int main (int argc, char *argv[]) 
{ 
    std::ifstream input(INPUT_FILE, std::ios::binary);
    MinifiedChannelConfiguration mincfg;
    input.read((char*)&mincfg, sizeof(MinifiedChannelConfiguration));
    std::cout << mincfg.recordLength;
}
