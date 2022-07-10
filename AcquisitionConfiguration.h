#ifndef ACQUISITIONCONFIGURATION_H
#define ACQUISITIONCONFIGURATION_H
#include "DigitizerConstants.h"
#include "RegisterConstants.h"
#include "qjsonobject.h"
#include "spdlog/spdlog.h"
#include "util.h"
#include <QJsonObject>
#include <array>
#include <string>
#include "version.h"



class ModificationObserver
{
protected:
    std::function<void()> callback;

public:
    void setListenerCallback(const std::function<void()>& callback = nullptr)
    {
        this->callback = callback;
    }
    virtual void modified()
    {
        if (this->callback)
            this->callback();
    };
};

class TriggerChannelConfiguration : public JSONSerializable, public ModificationObserver
{
  private:
    int reset_ = 0;
    int horizontalShift_ = 0;
    int level_ = 0;
    TRIGGER_EDGES edge_ = TRIGGER_EDGES::RISING;
    TRIGGER_MODES mode_ = TRIGGER_MODES::SOFTWARE;

  public:
    int reset() const
    {
        return reset_;
    };
    int horizontalShift() const
    {
        return horizontalShift_;
    };
    int level() const
    {
        return level_;
    };
    TRIGGER_EDGES edge() const
    {
        return edge_;
    };
    TRIGGER_MODES mode() const
    {
        return mode_;
    };
    void setReset(int val)
    {
        reset_ = val;
        this->modified();
    }
    void setHorizontalShift(int val)
    {
        horizontalShift_ = val;
        this->modified();
    }
    void setLevel(int val)
    {
        level_ = val;
        this->modified();
    }
    void setEdge(TRIGGER_EDGES val)
    {
        edge_ = val;
        this->modified();
    }
    void setMode(TRIGGER_MODES val)
    {
        mode_ = val;
        this->modified();
    }
    static TriggerChannelConfiguration fromJSON(const QJsonObject& json);
    QJsonObject toJSON() override;
};

class AnalogFrontEndChannelConfiguration : public JSONSerializable, public ModificationObserver
{
  private:
    int desiredInputRange_ = DEFAULT_INPUT_RANGE;
    float obtainedInputRange_ = 5000;
    int dcBias_ = 0;

  public:
    int desiredInputRange() const
    {
        return desiredInputRange_;
    }
    float obtainedInputRange() const
    {
        return obtainedInputRange_;
    }
    int dcBias() const
    {
        return dcBias_;
    }
    void setDesiredInputRange(int val)
    {
        desiredInputRange_ = val;
        this->modified();
    }
    void setObtainedInputRange(float val)
    {
        obtainedInputRange_ = val;
        this->modified();
    }
    void setDcBias(int val)
    {
        dcBias_ = val;
        this->modified();
    }
    static AnalogFrontEndChannelConfiguration fromJSON(const QJsonObject& json);
    QJsonObject toJSON() override;
};

class CalibrationChannelConfiguration : public JSONSerializable, public ModificationObserver
{
  private:
    int digitalGain_ = DEFAULT_DIGITAL_USER_GAIN;
    int digitalOffset_ = 0;
    int analogOffset_ = 0;

  public:
    int digitalGain() const
    {
        return digitalGain_;
    }
    int digitalOffset() const
    {
        return digitalOffset_;
    }
    int analogOffset() const
    {
        return analogOffset_;
    }
    void setDigitalGain(int val)
    {
        digitalGain_ = val;
        this->modified();
    }
    void setDigitalOffset(int val)
    {
        digitalOffset_ = val;
        this->modified();
    }
    void setAnalogOffset(int val)
    {
        analogOffset_ = val;
        this->modified();
    }
    static CalibrationChannelConfiguration fromJSON(const QJsonObject& json);
    QJsonObject toJSON() override;
};
class RecordChannelConfiguration : public JSONSerializable, public ModificationObserver
{
  private:
    unsigned long recordCount_ = INFINITE_RECORDS;
    unsigned long recordLength_ = 256;

  public:
    unsigned long recordCount() const
    {
        return recordCount_;
    }
    unsigned long recordLength() const
    {
        return recordLength_;
    }
    void setRecordCount(unsigned long val)
    {
        recordCount_ = val;
        this->modified();
    }
    void setRecordLength(unsigned long val)
    {
        recordLength_ = val;
        this->modified();
    }
    bool isInfinite() const;
    void setInfinite();
    static RecordChannelConfiguration fromJSON(const QJsonObject& json);
    QJsonObject toJSON() override;
};

class FileSaveConfiguration : public JSONSerializable, public ModificationObserver
{
  private:
    bool enabled_;
    std::string tag_;
    bool appendDate_ = false;
    bool storeHeaders_ = false;
    bool bufferInRAM_ = false;
    unsigned long long fileSizeLimit_ = 2e9;

  public:
    bool enabled() const {
        return enabled_;
    }
    std::string tag() const
    {
        return tag_;
    }
    bool appendDate() const
    {
        return appendDate_;
    }
    bool storeHeaders() const
    {
        return storeHeaders_;
    }
    bool bufferInRAM() const
    {
        return bufferInRAM_;
    }
    unsigned long long fileSizeLimit() const
    {
        return fileSizeLimit_;
    }
    void setEnabled(bool val)
    {
        enabled_ = val;
    }
    void setTag(std::string val)
    {
        tag_ = val;
    };
    void setAppendDate(bool val)
    {
        appendDate_ = val;
        this->modified();
    }
    void setStoreHeaders(bool val)
    {
        storeHeaders_ = val;
        this->modified();
    }
    void setBufferInRAM(bool val)
    {
        bufferInRAM_ = val;
        this->modified();
    }
    void setFileSizeLimit(unsigned long long val)
    {
        fileSizeLimit_ = val;
        this->modified();
    }
    static FileSaveConfiguration fromJSON(const QJsonObject& json);
    QJsonObject toJSON() override;
};

class DataTransferConfiguration : public JSONSerializable, public ModificationObserver
{
  private:
    unsigned long bufferSize_ = 2048;
    unsigned long bufferCount_ = 32;
    unsigned long queueSize_ = 128;
    unsigned long dmaTimeout_ = 500;

  public:
    unsigned long bufferSize() const
    {
        return bufferSize_;
    }
    unsigned long bufferCount() const
    {
        return bufferCount_;
    }
    unsigned long queueSize() const
    {
        return queueSize_;
    }
    unsigned long dmaTimeout() const
    {
        return dmaTimeout_;
    }
    void setBufferSize(unsigned long val)
    {
        bufferSize_ = val;
        this->modified();
    }
    void setBufferCount(unsigned long val)
    {
        bufferCount_ = val;
        this->modified();
    }
    void setQueueSize(unsigned long val)
    {
        queueSize_ = val;
        this->modified();
    }
    void setDmaTimeout(unsigned long val)
    {
        dmaTimeout_ = val;
        this->modified();
    }
    static DataTransferConfiguration fromJSON(const QJsonObject& json);
    QJsonObject toJSON() override;
};

class SpectroscopeConfiguration : public JSONSerializable, public ModificationObserver
{
  private:
    unsigned char userLogicBypassMask_ = 0b11;
    bool enabled_ = false;
    bool transferOverDMA_ = false;
    unsigned int binCountReductionShift_ = 0;
    unsigned int windowLength_ = 100;

  public:
    unsigned char userLogicBypassMask() const
    {
        return userLogicBypassMask_;
    }
    bool enabled() const
    {
        return enabled_;
    }
    bool transferOverDMA() const
    {
        return transferOverDMA_;
    }
    unsigned int binCountReductionShift() const
    {
        return binCountReductionShift_;
    }
    unsigned int binCount() const 
    {
        return Spectroscopy::MAX_SPECTRUM_BIN_COUNT >> this->binCountReductionShift();
    }
    unsigned int windowLength() const
    {
        return windowLength_;
    }
    void setUserLogicBypassMask(unsigned char val)
    {
        userLogicBypassMask_ = val;
        this->modified();
    }
    void setEnabled(bool val)
    {
        enabled_ = val;
        this->modified();
    }
    void setTransferOverDMA(bool val)
    {
        transferOverDMA_ = val;
        this->modified();
    }
    void setBinCountReductionShift(unsigned int val)
    {
        binCountReductionShift_ = val;
        this->modified();
    }
    void setWindowLength(unsigned int val)
    {
        windowLength_ = val;
        this->modified();
    }
    void setBypass(UserLogic ul, bool bypassed);
    bool isBypassed(UserLogic ul) const;
    static SpectroscopeConfiguration fromJSON(const QJsonObject& json);
    QJsonObject toJSON() override;
};

class DataCollectionConfiguration : public JSONSerializable, public ModificationObserver
{
  private:
    CLOCK_SOURCES clockSource_ = CLOCK_SOURCES::INTSRC_INTREF_10MHZ;
    ACQUISITION_MODES acquisitionMode_ = ACQUISITION_MODES::TRIGGERED;
    TRIGGER_APPROACHES triggerApproach_ = TRIGGER_APPROACHES::SINGLE;
    unsigned int sampleSkip_ = 1;
    unsigned long duration_ = 200;
    unsigned long stabilizationDelay_ = 2500;

    unsigned char channelMask_ = 0b0001;
    unsigned char triggerMask_ = 0b0001;

  public:
    const static unsigned long NO_DURATION = 0;

    CLOCK_SOURCES clockSource() const
    {
        return clockSource_;
    };
    ACQUISITION_MODES acquisitionMode() const
    {
        return acquisitionMode_;
    };
    bool isContinuous() const
    {
        return this->acquisitionMode_ == ACQUISITION_MODES::CONTINOUS;
    }
    TRIGGER_APPROACHES triggerApproach() const
    {
        return triggerApproach_;
    };
    unsigned int sampleSkip() const
    {
        return sampleSkip_;
    };
    unsigned long duration() const
    {
        return duration_;
    };
    unsigned long stabilizationDelay() const
    {
        return stabilizationDelay_;
    };
    unsigned char channelMask() const
    {
        return channelMask_;
    };
    unsigned char triggerMask() const
    {
        return triggerMask_;
    };

    void setClockSource(CLOCK_SOURCES val)
    {
        clockSource_ = val;
        this->modified();
    }
    void setAcquisitionMode(ACQUISITION_MODES val)
    {
        acquisitionMode_ = val;
        this->modified();
    }
    void setTriggerApproach(TRIGGER_APPROACHES val)
    {
        triggerApproach_ = val;
        this->modified();
    }
    void setSampleSkip(unsigned int val)
    {
        sampleSkip_ = val;
        this->modified();
    }
    void setDuration(unsigned long val)
    {
        duration_ = val;
        this->modified();
    }
    void disableDuration()
    {
        duration_ = NO_DURATION;
        this->modified();
    }
    void setStabilizationDelay(unsigned long val)
    {
        stabilizationDelay_ = val;
        this->modified();
    }
    void setChannelMask(unsigned char val)
    {
        channelMask_ = val;
        this->modified();
    }
    void setTriggerMask(unsigned char val)
    {
        triggerMask_ = val;
        this->modified();
    }

    static DataCollectionConfiguration fromJSON(const QJsonObject& json);
    QJsonObject toJSON() override;
};

class AcquisitionConfiguration : public JSONSerializable
{
  public:
    DataCollectionConfiguration collection;
    DataTransferConfiguration transfer;
    FileSaveConfiguration storage;
    SpectroscopeConfiguration spectroscope;

    std::array<TriggerChannelConfiguration, MAX_NOF_CHANNELS> triggers = {
        {TriggerChannelConfiguration(), TriggerChannelConfiguration(),
         TriggerChannelConfiguration(), TriggerChannelConfiguration()}};
    std::array<AnalogFrontEndChannelConfiguration, MAX_NOF_CHANNELS> AFEs = {
        {AnalogFrontEndChannelConfiguration(), AnalogFrontEndChannelConfiguration(),
         AnalogFrontEndChannelConfiguration(), AnalogFrontEndChannelConfiguration()}};
    std::array<CalibrationChannelConfiguration, MAX_NOF_CHANNELS> calibrations = {
        {CalibrationChannelConfiguration(), CalibrationChannelConfiguration(),
         CalibrationChannelConfiguration(), CalibrationChannelConfiguration()}};
    std::array<RecordChannelConfiguration, MAX_NOF_CHANNELS> records = {
        {RecordChannelConfiguration(), RecordChannelConfiguration(), RecordChannelConfiguration(),
         RecordChannelConfiguration()}};

  public:
    unsigned int getPrimaryTriggerChannel() const;
    void log();
    static AcquisitionConfiguration fromJSON(const QJsonObject& json);
    QJsonObject toJSON() override;
    int getTotalDCShift(int ch, int& unclipped);
    static unsigned char verifyChannelMaskForSingularApproach(unsigned char channelMask);
    float getTargetInputRangeFloat(int channel);
    unsigned int maxRecordLength();
};

#endif // DIGITIZERCONFIGURATION_H
