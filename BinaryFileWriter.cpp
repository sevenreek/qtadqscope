#include "BinaryFileWriter.h"
#include "spdlog/spdlog.h"
#include <ctime>
#include "MinifiedRecordHeader.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include "util.h"

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
void BinaryFileWriter::startNewAcquisition(Acquisition& config)
{
    this->channelMask = config.getChannelMask();
    this->bytesSaved = 0;
    this->sizeLimit = config.getFileSizeLimit();
    this->isContinuousStream = config.getIsContinuous();
    std::time_t t = std::time(nullptr); // get current time
    auto tm = *std::localtime(&t);
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        if((1<<ch) & this->channelMask)
        {
            int adqch = ch+1;
            std::string s_data = fmt::format("{}_{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", config.getTag(), tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, adqch);
            s_data = removeIllegalFilenameChars(s_data, ILLEGAL_CHAR_REPLACE);
            const char* cstr = s_data.c_str();
            this->dataStream[ch].open(cstr, std::ios_base::binary | std::ios_base::out);
        }
    }
    // save config
    std::string s_cfg = fmt::format(
        "{}_{:02d}{:02d}_{:02d}{:02d}{:02d}_cfg.json", config.getTag(), tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec
    );
    s_cfg = removeIllegalFilenameChars(s_cfg, ILLEGAL_CHAR_REPLACE);
    QJsonObject json = config.toJson();
    QJsonDocument doc;
    doc.setObject(json);
    QFile cfgFile(QString::fromStdString(s_cfg));
    cfgFile.open(QFile::OpenModeFlag::WriteOnly);
    cfgFile.write(doc.toJson());
    cfgFile.close();
}

IRecordProcessor::STATUS BinaryFileWriter::processRecord(ADQRecordHeader* header, short* buffer, unsigned long length, int channel)
{
    if(this->bytesSaved > this->sizeLimit)
    {
        return STATUS::LIMIT_REACHED;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        this->dataStream[channel].write((char*)buffer, sizeof(short)*length);
        return STATUS::OK;
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




BufferedBinaryFileWriter::BufferedBinaryFileWriter(unsigned long long sizeLimit) : BinaryFileWriter(sizeLimit)
{
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
void BufferedBinaryFileWriter::startNewAcquisition(Acquisition& config)
{
    this->BinaryFileWriter::startNewAcquisition(config);
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
}

IRecordProcessor::STATUS BufferedBinaryFileWriter::processRecord(ADQRecordHeader* header, short* buffer, unsigned long length, int channel)
{
    // record length is stored in the cfg file
    // an alternative is to prefix every buffer with the header
    // a header has a constant size of 40 bytes
    // so it would be possible to read how many samples to read
    // from the header, this is done in the Verbose Writer
    if(this->bytesSaved >= this->sizeLimit)
    {
        return STATUS::LIMIT_REACHED;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        int ch = channel;
        //spdlog::debug("Copying data from ch{} to shift {}", ch, this->samplesSaved[ch]);
        std::memcpy(&(this->dataBuffer[ch][this->samplesSaved[ch]]), buffer, length*sizeof(short));
        this->samplesSaved[ch] += length;
        return STATUS::OK;
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
IRecordProcessor::STATUS VerboseBufferedBinaryWriter::processRecord(ADQRecordHeader* header, short* buffer, unsigned long length, int channel)
{
    if(this->bytesSaved >= this->sizeLimit)
    {
        return STATUS::LIMIT_REACHED;
    }
    else if(this->isContinuousStream)
    {
        this->bytesSaved += length*sizeof(short);
        int ch = channel;
        //spdlog::debug("Copying data from ch{} to shift {}", ch, this->samplesSaved[ch]);
        std::memcpy(&(this->dataBuffer[ch][this->samplesSaved[ch]]), buffer, length*sizeof(short));
        this->samplesSaved[ch] += length;
        return STATUS::OK;
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
        return STATUS::OK;
    }
}

const char *VerboseBufferedBinaryWriter::getName()
{
    return "VerboseBufferedBinaryWriter";
}
void VerboseBufferedBinaryWriter::startNewAcquisition(Acquisition& config)
{
    this->BufferedBinaryFileWriter::startNewAcquisition(config); // call super
    for(int i = 0; i< MAX_NOF_CHANNELS; i++)
    {
        if((1<<i) & this->channelMask)
        {
            spdlog::debug("Writing minifed config(size={}) to stream for ch {}", sizeof(MinifiedAcquisitionConfiguration), i);
            MinifiedAcquisitionConfiguration m = minifyAcquisitionConfiguration(config, i);
            this->dataStream[i].write((char*)&m,sizeof(MinifiedAcquisitionConfiguration));
        }
    }
}

VerboseBinaryWriter::VerboseBinaryWriter(unsigned long long sizeLimit) : BinaryFileWriter(sizeLimit)
{

}

IRecordProcessor::STATUS VerboseBinaryWriter::processRecord(ADQRecordHeader *header, short *buffer, unsigned long length, int channel)
{
    if(this->bytesSaved >= this->sizeLimit)
    {
        return STATUS::LIMIT_REACHED;
    }
    else if(this->isContinuousStream)
    {
        this->bytesSaved += length*sizeof(short);
        this->dataStream[channel].write(reinterpret_cast<char*>(buffer), length*sizeof(short));
        return STATUS::OK;
    }
    else
    {
        //spdlog::debug("Copying data from ch{} to shift {}", ch, this->samplesSaved[ch]);
        MinifiedRecordHeader mh = minifyRecordHeader(*header);
        this->dataStream[channel].write(reinterpret_cast<char*>(&mh), sizeof(MinifiedRecordHeader));
        this->bytesSaved += sizeof(MinifiedRecordHeader);

        this->dataStream[channel].write(reinterpret_cast<char*>(buffer), length*sizeof(short));
        this->bytesSaved += length*sizeof(short);
        return STATUS::OK;
    }
}

VerboseBinaryWriter::~VerboseBinaryWriter()
{

}

const char *VerboseBinaryWriter::getName()
{
    return "VerboseBinaryWriter";
}


void VerboseBinaryWriter::startNewAcquisition(Acquisition& config)
{
    this->BinaryFileWriter::startNewAcquisition(config); // call super
    for(int i = 0; i< MAX_NOF_CHANNELS; i++)
    {
        if((1<<i) & this->channelMask)
        {
            spdlog::debug("Writing minifed config(size={}) to stream for ch {}", sizeof(MinifiedAcquisitionConfiguration), i);
            MinifiedAcquisitionConfiguration m = minifyAcquisitionConfiguration(config, i);
            this->dataStream[i].write(reinterpret_cast<char*>(&m), sizeof(MinifiedAcquisitionConfiguration));
        }
    }
}

