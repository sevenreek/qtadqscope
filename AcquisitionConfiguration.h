#ifndef ACQUISITIONCONFIGURATION_H
#define ACQUISITIONCONFIGURATION_H
#include "DigitizerConstants.h"
#include "Configuration.h"
#include <string>
#include <array>
#include <QJsonObject>
#include "spdlog/spdlog.h"


class TriggerChannelConfiguration: public ChannelConfiguration {
public:
    int triggerReset = 0;
    int horizontalShift = 0;
    void setTriggerLevel(int code);
    int getTriggerLevel();
private:
    int triggerLevel = 0;
    TRIGGER_EDGES triggerEdge = TRIGGER_EDGES::RISING;
    TRIGGER_MODES triggerMode = TRIGGER_MODES::SOFTWARE;
    static TriggerChannelConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;
};

class DataTransferConfiguration: public IConfiguration {
public:
    unsigned long transferBufferSize = 2048;
    unsigned long transferBufferCount = 32;
    unsigned long transferBufferQueueSize = 128;
    unsigned long dmaCheckMinimumTimeout = 500;
    static DataTransferConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;
};

class AnalogFrontEndChannelConfiguration: public ChannelConfiguration {
public:
    INPUT_RANGES desiredInputRange = INPUT_RANGES::MV_5000;
    float obtainedInputRange = 5000;
    int dcBias = 0;
    static AnalogFrontEndChannelConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;
};

class CalibrationChannelConfiguration: public ChannelConfiguration {
public:
    int digitalGain = DEFAULT_DIGITAL_USER_GAIN;
    int digitalOffset = 0;
    int analogOffset = 0;
    static CalibrationChannelConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;
};

class FileSaveConfiguration : public IConfiguration {
public:
    std::string tag;
    bool appendDate = false;
    bool storeHeaders = false;
    bool bufferInRAM = false;
    unsigned long long fileSizeLimit = 2e9;
    static FileSaveConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;
};

class RecordChannelConfiguration: public ChannelConfiguration  {
public:
    unsigned long recordCount = INFINITE_RECORDS;
    unsigned long recordLength = 256;
    static RecordChannelConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;
};

class SpectroscopeConfiguration : public IConfiguration {
public:
    unsigned char userLogicBypassMask = 0b11;
    bool enabled = false;
    static SpectroscopeConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;
};

class DataCollectionConfiguration: public IConfiguration {
public:
    const static unsigned long NO_DURATION = 0;
    CLOCK_SOURCES clockSource = CLOCK_SOURCES::INTSRC_INTREF_10MHZ;
    ACQUISITION_MODES triggeringMode = ACQUISITION_MODES::TRIGGERED;
    unsigned int sampleSkip = 1;
    unsigned long duration = 200;

    unsigned char channelMask = 0b0001;
    unsigned char triggerMask = 0b0001;
    TRIGGER_APPROACHES triggerApproach = TRIGGER_APPROACHES::SINGLE;
    static DataCollectionConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;

};

class AcquisitionConfiguration : public IConfiguration {
public:
    DataCollectionConfiguration dataCollection;
    DataTransferConfiguration dataTransfer;
    FileSaveConfiguration storage;
    SpectroscopeConfiguration spectroscope;
    
    std::array<TriggerChannelConfiguration, MAX_NOF_CHANNELS> triggers = {{TriggerChannelConfiguration(0), TriggerChannelConfiguration(1), TriggerChannelConfiguration(2), TriggerChannelConfiguration(3)}};
    std::array<AnalogFrontEndChannelConfiguration, MAX_NOF_CHANNELS> AFEs = {{AnalogFrontEndChannelConfiguration(0), AnalogFrontEndChannelConfiguration(1), AnalogFrontEndChannelConfiguration(2), AnalogFrontEndChannelConfiguration(3)}};
    std::array<CalibrationChannelConfiguration, MAX_NOF_CHANNELS> calibrations = {{CalibrationChannelConfiguration(0), CalibrationChannelConfiguration(1), CalibrationChannelConfiguration(2), CalibrationChannelConfiguration(3)}};
    std::array<RecordChannelConfiguration, MAX_NOF_CHANNELS> records = {{RecordChannelConfiguration(0), RecordChannelConfiguration(1), RecordChannelConfiguration(2), RecordChannelConfiguration(3)}};

public:
    unsigned int getPrimaryTriggerChannel() const;
    void log();
    static AcquisitionConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;
    int getTotalDCShift(int ch, int& unclipped);
    static unsigned char verifyChannelMaskForSingularApproach(unsigned char channelMask);
    float getTargetInputRangeFloat(int channel);
};


#endif // DIGITIZERCONFIGURATION_H
