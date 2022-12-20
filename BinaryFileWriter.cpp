#include "BinaryFileWriter.h"
#include "spdlog/spdlog.h"
#include <ctime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include "util.h"

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
bool BinaryFileWriter::startNewAcquisition(Acquisition* acq)
{
    this->channelMask = acq->getChannelMask();
    this->bytesSaved = 0;
    this->sizeLimit = acq->getFileSizeLimit();
    this->isContinuousStream = acq->getIsContinuous();
    this->expectedRecordLength = acq->getRecordLength();
    std::time_t t = std::time(nullptr); // get current time
    auto tm = *std::localtime(&t);
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        if((1<<ch) & this->channelMask)
        {
            int adqch = ch+1;
            std::string s_data = fmt::format("{}_{:02d}{:02d}_{:02d}{:02d}{:02d}_ch{}.dat", acq->getTag(), tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, adqch);
            s_data = removeIllegalFilenameChars(s_data, ILLEGAL_CHAR_REPLACE);
            const char* cstr = s_data.c_str();
            this->dataStream[ch].open(cstr, std::ios_base::binary | std::ios_base::out);
        }
    }
    // save config
    std::string s_cfg = fmt::format(
        "{}_{:02d}{:02d}_{:02d}{:02d}{:02d}_cfg.json", acq->getTag(), tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec
    );
    s_cfg = removeIllegalFilenameChars(s_cfg, ILLEGAL_CHAR_REPLACE);
    QJsonObject json = acq->toJson();
    QJsonDocument doc;
    doc.setObject(json);
    QFile cfgFile(QString::fromStdString(s_cfg));
    cfgFile.open(QFile::OpenModeFlag::WriteOnly);
    cfgFile.write(doc.toJson());
    cfgFile.close();
    return true;
}

IRecordProcessor::STATUS BinaryFileWriter::processRecord(ADQRecord* record, size_t bufferSize)
{
    if(this->bytesSaved > this->sizeLimit)
    {
        return STATUS::LIMIT_REACHED;
    }
    else
    {
        this->bytesSaved += bufferSize;
        this->dataStream[record->header->Channel].write((char*)record->data, bufferSize /*sizeof(short)*record->header->RecordLength*/);
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




VerboseBinaryWriter::VerboseBinaryWriter(unsigned long long sizeLimit) : BinaryFileWriter(sizeLimit)
{

}

IRecordProcessor::STATUS VerboseBinaryWriter::processRecord(ADQRecord *record, size_t bufferSize)
{
    int channel = record->header->Channel;
    if(this->bytesSaved >= this->sizeLimit)
    {
        return STATUS::LIMIT_REACHED;
    }
    else if(this->isContinuousStream)
    {
        this->bytesSaved += bufferSize;
        this->dataStream[record->header->Channel].write(reinterpret_cast<char*>(record->data), bufferSize);
        return STATUS::OK;
    }
    else
    {
        MinifiedRecordHeader mh = minifyRecordHeader(*record->header);
        /*
        if(mh.recordLength != this->expectedRecordLength)
        {
            spdlog::debug("Bad record detected in binaryfilewriter after minification, skipping; expected {} got {}",this->expectedRecordLength, mh.recordLength);
            return STATUS::OK;
        }
        */
        this->dataStream[channel].write(reinterpret_cast<char*>(&mh), sizeof(MinifiedRecordHeader));
        this->bytesSaved += sizeof(MinifiedRecordHeader);

        this->dataStream[channel].write(reinterpret_cast<char*>(record->data), bufferSize /*record->header->RecordLength*sizeof(short)*/);
        this->bytesSaved += bufferSize;// record->header->RecordLength*sizeof(short);
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


bool VerboseBinaryWriter::startNewAcquisition(Acquisition* acq)
{
    this->BinaryFileWriter::startNewAcquisition(acq); // call super
    for(int i = 0; i< MAX_NOF_CHANNELS; i++)
    {
        if((1<<i) & this->channelMask)
        {
            spdlog::debug("Writing minifed config(size={}) to stream for ch {}", sizeof(struct MinifiedAcquisitionConfiguration), i);
            spdlog::debug("Header size is {}", sizeof(MinifiedRecordHeader));
            MinifiedAcquisitionConfiguration m = minifyAcquisitionConfiguration(*acq, i);
            this->dataStream[i].write(reinterpret_cast<char*>(&m), sizeof(struct MinifiedAcquisitionConfiguration));
        }
    }
    return true;
}



