#include "DigitizerConfiguration.h"
#include "util.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
float Acquisition::getObtainedInputRange(int ch) const
{
    return obtainedInputRange.at(ch);
}

void Acquisition::setObtainedInputRange(int ch, float val)
{
    obtainedInputRange.at(ch) = val;
}

float Acquisition::getDesiredInputRange(int channel) const
{
    return INPUT_RANGE_VALUES[int(this->inputRange.at(channel))];
}

INPUT_RANGES Acquisition::getInputRange(int ch) const
{
    return this->inputRange.at(ch);
}

void Acquisition::setInputRange(int ch, const INPUT_RANGES &value)
{
    this->inputRange.at(ch) = value;
}

int Acquisition::getDcBias(int ch) const
{
    return this->dcBias.at(ch);
}

void Acquisition::setDcBias(int ch, int value)
{
    this->dcBias.at(ch) = value;
}

int Acquisition::getDigitalGain(int ch) const
{
    return this->digitalGain.at(ch);
}

void Acquisition::setDigitalGain(int ch, int value)
{
    this->digitalGain.at(ch) = value;
}

int Acquisition::getDigitalOffset(int ch) const
{
    return this->digitalOffset.at(ch);
}

void Acquisition::setDigitalOffset(int ch, int value)
{
    this->digitalOffset.at(ch) = value;
}

int Acquisition::getAnalogOffset(int ch) const
{
    return this->analogOffset.at(ch);
}

void Acquisition::setAnalogOffset(int ch, int value)
{
    this->analogOffset.at(ch) = value;
}

int Acquisition::getTotalDCShift(int ch, int& unclipped)
{
    unclipped = this->getAnalogOffset(ch) + this->getDcBias(ch);
    return clip(unclipped, CODE_MIN, CODE_MAX);
}

unsigned long long Acquisition::getFileSizeLimit() const
{
    return fileSizeLimit;
}

void Acquisition::setFileSizeLimit(unsigned long long value)
{
    fileSizeLimit = value;
}

int Acquisition::getTriggerLevel() const
{
    return triggerLevel;
}

void Acquisition::setTriggerLevel(int value)
{
    triggerLevel = value;
}

Acquisition Acquisition::fromJson(const QJsonObject &json)
{
    Acquisition returnValue;
    returnValue.tag                     = json["tag"].toString("").toStdString();
    returnValue.isContinuous            = json["continuous"].toBool(false);
    returnValue.duration                = json["duration"].toInt(0);
    returnValue.transferBufferSize      = json["buffer_size"].toInt(2048);
    returnValue.transferBufferCount     = json["buffer_count"].toInt(32);
    returnValue.transferBufferQueueSize = json["buffer_queue"].toInt(128);
    returnValue.fileSizeLimit           = json["file_size"].toDouble(2e9);
    returnValue.userLogicBypassMask     = json["ul_bypass"].toInt(0b11);
    returnValue.clockSource             = static_cast<CLOCK_SOURCES>(json["clock_source"].toInt(static_cast<int>(CLOCK_SOURCES::INTSRC_INTREF_10MHZ)));
    returnValue.triggerMode             = static_cast<TRIGGER_MODES>(json["trigger_mode"].toInt(static_cast<int>(TRIGGER_MODES::SOFTWARE)));
    returnValue.triggerEdge             = static_cast<TRIGGER_EDGES>(json["trigger_edge"].toInt(static_cast<int>(TRIGGER_EDGES::RISING)));
    returnValue.triggerMask             = json["trigger_mask"].toInt(0b0001);
    returnValue.triggerLevel            = json["trigger_level"].toInt(0);
    returnValue.triggerDelay            = json["trigger_delay"].toInt(0);
    returnValue.pretrigger              = json["pretrigger"].toInt(0);
    returnValue.recordCount             = json["record_count"].toInt(-1);
    returnValue.recordLength            = json["record_length"].toInt(256);
    returnValue.channelMask             = json["channel_mask"].toInt(0b0001);
    returnValue.sampleSkip              = json["sample_skip"].toInt(1);
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        returnValue.inputRange.at(ch) = static_cast<INPUT_RANGES>(
            json["input_range"].isArray()?
                json["input_range"].toArray().at(ch).toInt(static_cast<int>(INPUT_RANGES::MV_5000))
                :INPUT_RANGES::MV_5000
        );
        returnValue.dcBias.at(ch) =
            json["dc_bias"].isArray()?
                json["dc_bias"].toArray().at(ch).toInt(0)
                :0;
        returnValue.digitalGain.at(ch) =
            json["digital_gain"].isArray()?
                json["digital_gain"].toArray().at(ch).toInt(DEFAULT_GAIN)
                :DEFAULT_GAIN;
        returnValue.digitalOffset.at(ch) =
            json["digital_offset"].isArray()?
                json["digital_offset"].toArray().at(ch).toInt(0)
                :0;
        returnValue.analogOffset.at(ch) =
            json["analog_offset"].isArray()?
                json["analog_offset"].toArray().at(ch).toInt(0)
                :0;
        returnValue.obtainedInputRange.at(ch) =
            json["obtained_range"].isArray()?
                json["obtained_range"].toArray().at(ch).toDouble(5000)
                :5000;
    }
    return returnValue;
}

QJsonObject Acquisition::toJson()
{
    QJsonObject returnValue;
    returnValue.insert("tag",               QString::fromStdString(this->tag));
    returnValue.insert("continuous",        this->isContinuous);
    returnValue.insert("duration",          QJsonValue(qint64(this->duration)));
    returnValue.insert("buffer_size",       QJsonValue(qint64(this->transferBufferSize)));
    returnValue.insert("buffer_count",      QJsonValue(qint64(this->transferBufferCount)));
    returnValue.insert("buffer_queue",      QJsonValue(qint64(this->transferBufferQueueSize)));
    returnValue.insert("file_size",         QJsonValue(double(this->fileSizeLimit)));
    returnValue.insert("ul_bypass",         QJsonValue(int(this->userLogicBypassMask)));
    returnValue.insert("clock_source",      QJsonValue(int(this->clockSource)));
    returnValue.insert("trigger_mode",      QJsonValue(int(this->triggerMode)));
    returnValue.insert("trigger_edge",      QJsonValue(int(this->triggerEdge)));
    returnValue.insert("trigger_mask",      QJsonValue(int(this->triggerMask)));
    returnValue.insert("trigger_level",     QJsonValue(int( this->triggerLevel)));
    returnValue.insert("trigger_delay",     QJsonValue(int( this->triggerDelay)));
    returnValue.insert("pretrigger",        QJsonValue(int(this->pretrigger)));
    returnValue.insert("record_count",      QJsonValue(qint64(this->recordCount)));
    returnValue.insert("record_length",     QJsonValue(qint64( this->recordLength)));
    returnValue.insert("channel_mask",      QJsonValue(int(this->channelMask)));
    returnValue.insert("sample_skip",       QJsonValue(int(this->sampleSkip)));
    returnValue.insert("input_range",       QJsonArray({this->inputRange.at(0), this->inputRange.at(1), this->inputRange.at(2), this->inputRange.at(3)}));
    returnValue.insert("dc_bias",           QJsonArray({this->dcBias.at(0), this->dcBias.at(1), this->dcBias.at(2), this->dcBias.at(3)}));
    returnValue.insert("digital_gain",      QJsonArray({this->digitalGain.at(0), this->digitalGain.at(1), this->digitalGain.at(2), this->digitalGain.at(3)}));
    returnValue.insert("digital_offset",    QJsonArray({this->digitalOffset.at(0), this->digitalOffset.at(1), this->digitalOffset.at(2), this->digitalOffset.at(3)}));
    returnValue.insert("analog_offset",     QJsonArray({this->analogOffset.at(0), this->analogOffset.at(1), this->analogOffset.at(2), this->analogOffset.at(3)}));
    returnValue.insert("obtained_range",    QJsonArray({this->obtainedInputRange.at(0), this->obtainedInputRange.at(1), this->obtainedInputRange.at(2), this->obtainedInputRange.at(3)}));
    return returnValue;
}

int Acquisition::getTriggerReset() const
{
    return triggerReset;
}

void Acquisition::setTriggerReset(int value)
{
    triggerReset = value;
}

std::string Acquisition::getTag() const
{
    return tag;
}

void Acquisition::setTag(const std::string &value)
{
    tag = value;
}

bool Acquisition::getIsContinuous() const
{
    return isContinuous;
}

void Acquisition::setIsContinuous(bool value)
{
    isContinuous = value;
}

unsigned long Acquisition::getDuration() const
{
    return duration;
}

void Acquisition::setDuration(unsigned long value)
{
    duration = value;
}

unsigned long Acquisition::getTransferBufferSize() const
{
    return transferBufferSize;
}

void Acquisition::setTransferBufferSize(unsigned long value)
{
    transferBufferSize = value;
}

unsigned long Acquisition::getTransferBufferCount() const
{
    return transferBufferCount;
}

void Acquisition::setTransferBufferCount(unsigned long value)
{
    transferBufferCount = value;
}

unsigned long Acquisition::getTransferBufferQueueSize() const
{
    return transferBufferQueueSize;
}

void Acquisition::setTransferBufferQueueSize(unsigned long value)
{
    transferBufferQueueSize = value;
}

unsigned char Acquisition::getUserLogicBypassMask() const
{
    return userLogicBypassMask;
}

void Acquisition::setUserLogicBypassMask(unsigned char value)
{
    userLogicBypassMask = value;
}

CLOCK_SOURCES Acquisition::getClockSource() const
{
    return clockSource;
}

void Acquisition::setClockSource(const CLOCK_SOURCES &value)
{
    clockSource = value;
}

TRIGGER_MODES Acquisition::getTriggerMode() const
{
    return triggerMode;
}

void Acquisition::setTriggerMode(const TRIGGER_MODES &value)
{
    triggerMode = value;
}

TRIGGER_EDGES Acquisition::getTriggerEdge() const
{
    return triggerEdge;
}

void Acquisition::setTriggerEdge(const TRIGGER_EDGES &value)
{
    triggerEdge = value;
}

unsigned char Acquisition::getTriggerMask() const
{
    return triggerMask;
}

void Acquisition::setTriggerMask(unsigned char value)
{
    triggerMask = value;
}

unsigned short Acquisition::getPretrigger() const
{
    return pretrigger;
}

void Acquisition::setPretrigger(unsigned short value)
{
    pretrigger = value;
}

unsigned short Acquisition::getTriggerDelay() const
{
    return triggerDelay;
}

void Acquisition::setTriggerDelay(unsigned short value)
{
    triggerDelay = value;
}

unsigned long Acquisition::getRecordCount() const
{
    return recordCount;
}

void Acquisition::setRecordCount(unsigned long value)
{
    recordCount = value;
}

unsigned long Acquisition::getRecordLength() const
{
    return recordLength;
}

void Acquisition::setRecordLength(unsigned long value)
{
    recordLength = value;
}

unsigned char Acquisition::getChannelMask() const
{
    return channelMask;
}

void Acquisition::setChannelMask(unsigned char value)
{
    channelMask = value;
}

unsigned int Acquisition::getSampleSkip() const
{
    return sampleSkip;
}

void Acquisition::setSampleSkip(unsigned int value)
{
    sampleSkip = value;
}

