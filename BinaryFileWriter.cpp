#include "BinaryFileWriter.h"
#include "spdlog/spdlog.h"
#include <ctime>
BinaryFileWriter::BinaryFileWriter(unsigned long long sizeLimit)
{
    this->sizeLimit = sizeLimit;
    this->bytesSaved = 0;
}
BinaryFileWriter::~BinaryFileWriter()
{

}
unsigned long long BinaryFileWriter::getProcessedBytes()
{
    return this->bytesSaved;
}
void BinaryFileWriter::startNewStream(ApplicationConfiguration& config)
{
    std::time_t t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::string s_cfg = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_cfg.json", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    int baseChannel = config.getCurrentChannel();
    std::string s_data = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, baseChannel);
    this->dataStream[baseChannel].open(s_data.c_str(), std::ios_base::binary | std::ios_base::out);
    this->channelMask = 1<<baseChannel;
    int additionalChannel = config.secondChannel;
    if(additionalChannel != CHANNEL_DISABLED)
    {
        s_data = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, baseChannel);
        this->dataStream[baseChannel].open(s_data.c_str(), std::ios_base::binary | std::ios_base::out);
        this->channelMask = 1<<baseChannel | 1<<additionalChannel;
    }
    config.toFile(s_cfg.c_str());
    this->bytesSaved = 0;
    this->sizeLimit = config.fileSizeLimit;

}

bool BinaryFileWriter::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length)
{
    if(this->bytesSaved > this->sizeLimit)
    {
        return false;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        this->dataStream[header->Channel-1].write((char*)buffer, sizeof(short)*length);
        return true;
    }
}

unsigned long long BinaryFileWriter::finish(){
    for(int i = 0; i< MAX_NOF_CHANNELS; i++)
        if(1<<i & this->channelMask)
                this->dataStream[i].close();
    return this->bytesSaved;
}
const char* BinaryFileWriter::getName()
{
    return "BinaryFileWriter";
}




BufferedBinaryFileWriter::BufferedBinaryFileWriter(unsigned long long sizeLimit)
{
    this->sizeLimit = sizeLimit;
    this->bytesSaved = 0;
}
BufferedBinaryFileWriter::~BufferedBinaryFileWriter()
{
    std::free(this->dataBuffer);
}
void BufferedBinaryFileWriter::startNewStream(ApplicationConfiguration& config)
{
    std::time_t t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::string s_cfg = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_cfg.json", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    int baseChannel = config.getCurrentChannel();
    std::string s_data = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, baseChannel);
    this->dataStream[baseChannel].open(s_data.c_str(), std::ios_base::binary | std::ios_base::out);
    this->channelMask = 1<<baseChannel;
    int additionalChannel = config.secondChannel;
    if(additionalChannel != CHANNEL_DISABLED)
    {
        s_data = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, baseChannel);
        this->dataStream[baseChannel].open(s_data.c_str(), std::ios_base::binary | std::ios_base::out);
        this->channelMask = 1<<baseChannel | 1<<additionalChannel;
    }
    config.toFile(s_cfg.c_str());
    for(int ch = 0; ch<MAX_NOF_CHANNELS; ch++)
    {
        this->samplesSaved[ch] = 0;
        if(1<<ch & this->channelMask)
        {
            if(this->dataBuffer[ch] != nullptr)
                free(this->dataBuffer[ch]);
            this->dataBuffer[ch] = (short*)std::malloc(sizeof(short)*this->sizeLimit);
        }
    }
    this->bytesSaved = 0;
    this->sizeLimit = config.fileSizeLimit;
}

bool BufferedBinaryFileWriter::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length)
{
    // record length is stored in the cfg file
    // an alternative is to prefix every buffer with the header
    // a header has a constant size of 40 bytes
    // so it would be possible to read how many samples to read
    // from the header
    if(this->bytesSaved > this->sizeLimit)
    {
        return false;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        this->samplesSaved[header->Channel] += length;
        std::memcpy(&this->dataBuffer[header->Channel][this->samplesSaved[header->Channel]], buffer, length*sizeof(short));
        return true;
    }
}

unsigned long long BufferedBinaryFileWriter::finish(){
    for(int i = 0; i< MAX_NOF_CHANNELS; i++)
    {
        if(1<<i & this->channelMask)
        {
            this->dataStream[i].write((char*)this->dataBuffer[i], this->samplesSaved[i]*sizeof(char));
            this->dataStream[i].close();
        }
    }
    return this->bytesSaved;
}

const char* BufferedBinaryFileWriter::getName()
{
    return "BufferedBinaryFileWriter";
}

unsigned long long BufferedBinaryFileWriter::getProcessedBytes()
{
    return this->bytesSaved;
}
