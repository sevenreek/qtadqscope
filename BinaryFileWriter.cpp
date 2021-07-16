#include "BinaryFileWriter.h"
#include "spdlog/spdlog.h"
#include <ctime>
#include "MinifiedRecordHeader.h"
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
    std::string s_data = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, baseChannel+1);
    this->dataStream[baseChannel].open(s_data.c_str(), std::ios_base::binary | std::ios_base::out);
    this->channelMask = 1<<baseChannel;
    int additionalChannel = config.secondChannel;
    if(additionalChannel != CHANNEL_DISABLED)
    {
        s_data = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, baseChannel+1);
        this->dataStream[additionalChannel].open(s_data.c_str(), std::ios_base::binary | std::ios_base::out);
        this->channelMask = 1<<baseChannel | 1<<additionalChannel;
    }
    config.toFile(s_cfg.c_str());
    this->bytesSaved = 0;
    this->sizeLimit = config.fileSizeLimit;
    this->isContinuousStream = config.getCurrentChannelConfig().isContinuousStreaming;

}

bool BinaryFileWriter::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length, int channel)
{
    if(this->bytesSaved > this->sizeLimit)
    {
        return false;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        this->dataStream[channel].write((char*)buffer, sizeof(short)*length);
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
    for(int ch = 0; ch<MAX_NOF_CHANNELS; ch++)
    {
        this->dataBuffer[ch] = nullptr;
    }
}
BufferedBinaryFileWriter::~BufferedBinaryFileWriter()
{
    for(int ch = 0; ch<MAX_NOF_CHANNELS; ch++)
    {
        if(this->dataBuffer[ch] != nullptr)
        {
            std::free(this->dataBuffer[ch]);
            this->dataBuffer[ch] = nullptr;
        }
    }
}
void BufferedBinaryFileWriter::startNewStream(ApplicationConfiguration& config)
{
    std::time_t t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::string s_cfg = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_cfg.json", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    int baseChannel = config.getCurrentChannel();
    std::string s_data = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, baseChannel+1);
    this->dataStream[baseChannel].open(s_data.c_str(), std::ios_base::binary | std::ios_base::out);
    this->channelMask = 1<<baseChannel;
    int additionalChannel = config.secondChannel;
    if(additionalChannel != CHANNEL_DISABLED)
    {
        s_data = fmt::format("{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, additionalChannel+1);
        this->dataStream[additionalChannel].open(s_data.c_str(), std::ios_base::binary | std::ios_base::out);
        this->channelMask = 1<<baseChannel | 1<<additionalChannel;
    }
    config.toFile(s_cfg.c_str());
    this->sizeLimit = config.fileSizeLimit;
    for(int ch = 0; ch<MAX_NOF_CHANNELS; ch++)
    {
        if(this->dataBuffer[ch] != nullptr)
        {
            free(this->dataBuffer[ch]);
            this->dataBuffer[ch] = nullptr;
        }
        this->samplesSaved[ch] = 0;
        if(1<<ch & this->channelMask)
        {

            this->dataBuffer[ch] = (short*)std::malloc(this->sizeLimit);
            //spdlog::debug("Allocated buffer for ch{}; size:{}",ch, this->sizeLimit);
        }
    }
    this->bytesSaved = 0;
    this->isContinuousStream = config.getCurrentChannelConfig().isContinuousStreaming;
}

bool BufferedBinaryFileWriter::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length, int channel)
{
    // record length is stored in the cfg file
    // an alternative is to prefix every buffer with the header
    // a header has a constant size of 40 bytes
    // so it would be possible to read how many samples to read
    // from the header, this is done in the Verbose Writer
    if(this->bytesSaved >= this->sizeLimit)
    {
        return false;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        int ch = channel;
        //spdlog::debug("Copying data from ch{} to shift {}", ch, this->samplesSaved[ch]);
        std::memcpy(&(this->dataBuffer[ch][this->samplesSaved[ch]]), buffer, length*sizeof(short));
        this->samplesSaved[ch] += length;
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

VerboseBufferedBinaryWriter::VerboseBufferedBinaryWriter(unsigned long long sizeLimit) : BufferedBinaryFileWriter(sizeLimit)
{

}
VerboseBufferedBinaryWriter::~VerboseBufferedBinaryWriter()
{

}
bool VerboseBufferedBinaryWriter::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length, int channel)
{
    if(this->bytesSaved >= this->sizeLimit)
    {
        return false;
    }
    else if(this->isContinuousStream)
    {
        this->bytesSaved += length*sizeof(short);
        int ch = channel;
        //spdlog::debug("Copying data from ch{} to shift {}", ch, this->samplesSaved[ch]);
        std::memcpy(&(this->dataBuffer[ch][this->samplesSaved[ch]]), buffer, length*sizeof(short));
        this->samplesSaved[ch] += length;
        return true;
    }
    else
    {
        int ch = channel;
        //spdlog::debug("Copying data from ch{} to shift {}", ch, this->samplesSaved[ch]);
        MinifiedRecordHeader mh = minifyRecordHeader(*header);
        char* dataPointer = (char*)(this->dataBuffer[ch]);
        std::memcpy(dataPointer + this->bytesSaved, &mh, sizeof(MinifiedRecordHeader));
        this->bytesSaved += sizeof(MinifiedRecordHeader);

        std::memcpy(dataPointer + this->bytesSaved, buffer, length*sizeof(short));
        this->bytesSaved += length*sizeof(short);
        this->samplesSaved[ch] += length;
        return true;
    }
}
unsigned long long VerboseBufferedBinaryWriter::finish() {

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
void VerboseBufferedBinaryWriter::startNewStream(ApplicationConfiguration& config)
{
    BufferedBinaryFileWriter::startNewStream(config); // call super
    for(int i = 0; i< MAX_NOF_CHANNELS; i++)
    {
        if((1<<i) & this->channelMask)
        {
            spdlog::debug("Writing minifed config(size={}) to stream for ch {}", sizeof(MinifiedChannelConfiguration), i);
            MinifiedChannelConfiguration m = minifyChannelConfiguration(config.channelConfig[i]);
            this->dataStream[i].write((char*)&m,sizeof(MinifiedChannelConfiguration));
        }
    }
}
