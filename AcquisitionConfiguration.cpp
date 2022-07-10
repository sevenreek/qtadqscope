#include "AcquisitionConfiguration.h"
#include "DigitizerConstants.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "util.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

QJsonObject TriggerChannelConfiguration::toJSON()
{
    return QJsonObject({
        {"level", this->level()},
        {"mode", static_cast<int>(this->mode())},
        {"edge", static_cast<int>(this->edge())},
        {"horizontal_shift", this->horizontalShift()},
        {"reset", this->reset()},
    });
}

TriggerChannelConfiguration TriggerChannelConfiguration::fromJSON(const QJsonObject &json)
{
    TriggerChannelConfiguration rv;
    rv.edge_ = static_cast<TRIGGER_EDGES>(json["edge"].toInt(0));
    rv.mode_ = static_cast<TRIGGER_MODES>(json["mode"].toInt(1));
    rv.level_ = static_cast<int>(json["level"].toInt(0));
    rv.horizontalShift_ = static_cast<int>(json["horizontal_shift"].toInt(0));
    rv.reset_ = static_cast<int>(json["reset"].toInt(0));
    return rv;
}

QJsonObject AnalogFrontEndChannelConfiguration::toJSON()
{
    return QJsonObject({
        {"desired_range", this->desiredInputRange()},
        {"obtained_range", this->obtainedInputRange()},
        {"dc_bias", this->dcBias()},
    });
}

AnalogFrontEndChannelConfiguration AnalogFrontEndChannelConfiguration::fromJSON(const QJsonObject &json)
{
    AnalogFrontEndChannelConfiguration rv;
    rv.desiredInputRange_ = static_cast<int>(json["desired_range"].toInt(DEFAULT_INPUT_RANGE));
    rv.obtainedInputRange_ = static_cast<float>(json["obtained_range"].toDouble(DEFAULT_INPUT_RANGE));
    rv.dcBias_ = static_cast<int>(json["dc_bias"].toInt(0));
    return rv;
}

QJsonObject CalibrationChannelConfiguration::toJSON()
{
    return QJsonObject({
        {"digital_gain", this->digitalGain()},
        {"digital_offset", this->digitalOffset()},
        {"analog_offset", this->analogOffset()},
    });
}

CalibrationChannelConfiguration CalibrationChannelConfiguration::fromJSON(const QJsonObject &json)
{
    CalibrationChannelConfiguration rv;
    rv.digitalOffset_ = static_cast<int>(json["digital_offset"].toInt(0));
    rv.digitalGain_ = static_cast<int>(json["digital_gain"].toInt(DEFAULT_DIGITAL_USER_GAIN));
    rv.analogOffset_ = static_cast<int>(json["analog_offset"].toInt(0));
    return rv;
}


QJsonObject RecordChannelConfiguration::toJSON()
{
    return QJsonObject({
        {"count", static_cast<int>(this->recordCount())},
        {"length", static_cast<int>(this->recordLength())},
    });
}

RecordChannelConfiguration RecordChannelConfiguration::fromJSON(const QJsonObject &json)
{
    RecordChannelConfiguration rv;
    rv.recordCount_ = static_cast<unsigned long>(json["count"].toInt(INFINITE_RECORDS));
    rv.recordLength_ = static_cast<unsigned long>(json["length"].toInt(1024));
    return rv;
}

bool RecordChannelConfiguration::isInfinite() const
{
    return this->recordCount() == INFINITE_RECORDS;
}

void RecordChannelConfiguration::setInfinite()
{
    this->recordCount_ = INFINITE_RECORDS;
    this->modified();
}

QJsonObject FileSaveConfiguration::toJSON()
{
    return QJsonObject({
        {"tag", QString::fromStdString(this->tag())},
        {"append_date", this->appendDate()},
        {"store_headers", this->storeHeaders()},
        {"use_ram_buffer", this->bufferInRAM()},
        {"size_limit", static_cast<double>(this->fileSizeLimit())},
    });
}

FileSaveConfiguration FileSaveConfiguration::fromJSON(const QJsonObject &json)
{
    FileSaveConfiguration rv;
    rv.tag_ = json["tag"].toString("").toStdString();
    rv.appendDate_ = json["append_date"].toBool(false);
    rv.storeHeaders_ = json["store_headers"].toBool(false);
    rv.bufferInRAM_ = json["use_ram_buffer"].toBool(false);
    rv.fileSizeLimit_ = json["size_limit"].toDouble(2e9);
    return rv;
}

QJsonObject DataTransferConfiguration::toJSON()
{
    return QJsonObject({
        {"buffer_size",  static_cast<int>(this->bufferSize())},
        {"buffer_count", static_cast<int>(this->bufferCount())},
        {"queue_size",   static_cast<int>(this->queueSize())},
        {"dma_timeout",  static_cast<int>(this->dmaTimeout())},
    });
}

DataTransferConfiguration DataTransferConfiguration::fromJSON(const QJsonObject &json)
{
    DataTransferConfiguration rv;
    rv.bufferSize_ = json["buffer_size"].toInt(2048);
    rv.bufferCount_ = json["buffer_count"].toInt(32);
    rv.queueSize_ = json["queue_size"].toInt(128);
    rv.dmaTimeout_ = json["dma_timeout"].toInt(500);
    return rv;
}

void SpectroscopeConfiguration::setBypass(UserLogic ul, bool bypassed)
{
    unsigned int ulMask = 0b1 << static_cast<int>(ul);
    if (bypassed)
    {
        this->userLogicBypassMask_ |= ulMask;
    }
    else
    {
        this->userLogicBypassMask_ &= ulMask;
    }
    this->modified();
}
bool SpectroscopeConfiguration::isBypassed(UserLogic ul) const
{
    unsigned int ulMask = 0b1 << static_cast<int>(ul);
    return ulMask && this->userLogicBypassMask_;
}

QJsonObject SpectroscopeConfiguration::toJSON()
{
    return QJsonObject({
        {"ul_bypass",  static_cast<int>(this->userLogicBypassMask())},
        {"enabled",   this->enabled()},
        {"dma_transfer",   this->transferOverDMA()},
        {"bin_reduction", static_cast<int>(this->binCountReductionShift())},
        {"window_length", static_cast<int>(this->windowLength())},
    });
}

SpectroscopeConfiguration SpectroscopeConfiguration::fromJSON(const QJsonObject &json)
{
    SpectroscopeConfiguration rv;
    rv.userLogicBypassMask_ = json["ul_bypass"].toInt(0b11);
    rv.enabled_ = json["enabled"].toBool(false);
    rv.transferOverDMA_ = json["dma_transfer"].toBool(false);
    rv.binCountReductionShift_ = json["bin_reduction"].toInt(0);
    rv.windowLength_ = json["window_length"].toInt(100);
    return rv;
}



QJsonObject DataCollectionConfiguration::toJSON()
{
    return QJsonObject({
        {"clock_source",  static_cast<int>(this->clockSource())},
        {"acquisition_mode",  static_cast<int>(this->acquisitionMode())},
        {"trigger_approach",  static_cast<int>(this->triggerApproach())},
        {"sample_skip",  static_cast<int>(this->sampleSkip())},
        {"duration",  static_cast<int>(this->duration())},
        {"channel_mask",  static_cast<int>(this->channelMask())},
        {"trigger_mask",  static_cast<int>(this->triggerMask())},
        {"start_delay",  static_cast<int>(this->stabilizationDelay())},
    });
}

DataCollectionConfiguration DataCollectionConfiguration::fromJSON(const QJsonObject &json)
{
    DataCollectionConfiguration rv;
    rv.clockSource_ = static_cast<CLOCK_SOURCES>(json["clock_source"].toInt(static_cast<int>(CLOCK_SOURCES::INTSRC_INTREF_10MHZ)));
    rv.acquisitionMode_ = static_cast<ACQUISITION_MODES>(json["acquisition_mode"].toInt(static_cast<int>(ACQUISITION_MODES::TRIGGERED)));
    rv.triggerApproach_ = static_cast<TRIGGER_APPROACHES>(json["trigger_approach"].toInt(static_cast<int>(TRIGGER_APPROACHES::SINGLE)));
    rv.sampleSkip_ = json["sample_skip"].toInt(1);
    rv.duration_ = json["duration"].toInt(1000);
    rv.channelMask_ = json["channel_mask"].toInt(0b1);
    rv.triggerMask_ = json["trigger_mask"].toInt(0b1);
    rv.stabilizationDelay_ = json["start_delay"].toInt(2500);
    return rv;
}

int AcquisitionConfiguration::getTotalDCShift(int ch, int& unclipped)
{
    unclipped = this->AFEs[ch].dcBias() + this->calibrations[ch].analogOffset();
    return clip(unclipped, CODE_MIN, CODE_MAX);
}

unsigned char AcquisitionConfiguration::verifyChannelMaskForSingularApproach(unsigned char channelMask)
{
    if(!isOnlyOneBitSet(channelMask))
    {
        unsigned char newMask = channelMask;
        for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++) {
            if(channelMask & (1<<ch)) {
                newMask = (1<<ch);
                break;
            }
        }
        return newMask;
    }
    return channelMask;
}

unsigned int AcquisitionConfiguration::maxRecordLength() {
    auto maxRecord = this->records.at(0).recordCount();
    if(this->collection.isContinuous())
    {
        // Continuous acquisition can produce "records" 
        // equal to the amout of samples that fit in the buffer
        // In reality it is slighly less as headers are also present there
        return this->transfer.bufferSize()/sizeof(short);
    }
    for (auto &rec : this->records) {
      if (rec.recordCount() > maxRecord)
        maxRecord = rec.recordCount();
    }
    return maxRecord;
}

AcquisitionConfiguration AcquisitionConfiguration::fromJSON(const QJsonObject &json)
{
    AcquisitionConfiguration rv;

    QJsonObject dataCollectionJSON = json["collection"].toObject();
    rv.collection = DataCollectionConfiguration::fromJSON(dataCollectionJSON);
    QJsonObject dataTransferJSON = json["transfer"].toObject();
    rv.transfer = DataTransferConfiguration::fromJSON(dataTransferJSON);
    QJsonObject storageJSON = json["storage"].toObject();
    rv.storage = FileSaveConfiguration::fromJSON(storageJSON);
    QJsonObject spectroscopeJSON = json["spectroscope"].toObject();
    rv.spectroscope = SpectroscopeConfiguration::fromJSON(spectroscopeJSON);
    QJsonArray triggersJSON = json["triggers"].toArray();
    QJsonArray AFEJSON = json["afes"].toArray();
    QJsonArray calibrationsJSON = json["calibrations"].toArray();
    QJsonArray recordsJSON = json["records"].toArray();
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        rv.triggers.at(ch) = TriggerChannelConfiguration::fromJSON(triggersJSON.at(ch).toObject());
        rv.AFEs.at(ch) = AnalogFrontEndChannelConfiguration::fromJSON(AFEJSON.at(ch).toObject());
        rv.calibrations.at(ch) = CalibrationChannelConfiguration::fromJSON(calibrationsJSON.at(ch).toObject());
        rv.records.at(ch) = RecordChannelConfiguration::fromJSON(recordsJSON.at(ch).toObject());
    }
    return rv;
}

QJsonObject AcquisitionConfiguration::toJSON()
{
    return QJsonObject({
        {"version", APP_VERSION},
        {"collection", this->collection.toJSON()},
        {"transfer", this->transfer.toJSON()},
        {"storage", this->storage.toJSON()},
        {"spectroscope", this->spectroscope.toJSON()},
        {"triggers", QJsonArray({this->triggers[0].toJSON(), this->triggers[1].toJSON(), this->triggers[2].toJSON(), this->triggers[3].toJSON()}) },
        {"afes", QJsonArray({this->AFEs[0].toJSON(), this->AFEs[1].toJSON(), this->AFEs[2].toJSON(), this->AFEs[3].toJSON()}) },
        {"calibrations", QJsonArray({this->calibrations[0].toJSON(), this->calibrations[1].toJSON(), this->calibrations[2].toJSON(), this->calibrations[3].toJSON()}) },
        {"records", QJsonArray({this->records[0].toJSON(), this->records[1].toJSON(), this->records[2].toJSON(), this->records[3].toJSON()}) },
    });
}

void AcquisitionConfiguration::log()
{
    QJsonObject j = this->toJSON();
    QJsonDocument doc(j);
    spdlog::info("=== {} ===\n{}\n=========", this->storage.tag(), doc.toJson().toStdString());
}

unsigned int AcquisitionConfiguration::getPrimaryTriggerChannel() const
{
    int returnChannel = 0;
    unsigned char mask = this->collection.triggerMask();
    for(int ch=0; ch < MAX_NOF_CHANNELS; ch++)
    {
        if(mask & (1<<ch))
        {
            returnChannel = ch;
            break;
        }
    }
    return returnChannel;
}

