#ifndef ACQUISITIONCONFIGURATION_H
#define ACQUISITIONCONFIGURATION_H
#include "DigitizerConstants.h"
#include <string>
#include <array>
#include <QJsonObject>
#include "spdlog/spdlog.h"

class ChannelConfiguration {
protected:
    int channelIndex = -1;
public:
    const static int DEFAULT_NO_CHANNEL; // constant to signify that the channel config applies to a default/all channels
    ChannelConfiguration(int channelIndex);
    virtual QJsonObject toJSON() = 0;
    virtual static fromJSON(QJsonObject json) = 0;
};

class TriggerChannelConfiguration: public ChannelConfiguration {
public:
    enum TriggerTruthSources {
        CODE = 0,
        MV = 1
    };
    int triggerReset = 0;
    int horizontalShift = 0;
    void setTriggerLevel(int code);
    void setTriggerMillis(float mv);
    TriggerTruthSources getTriggerTruthSource();
    int getTriggerLevel();
    float getTriggerLevelMillis();
private:
    TriggerTruthSources truthSource = TriggerTruthSources::CODE;
    int triggerLevel = 0;
    float triggerLevelMillivolts = 0;
    TRIGGER_EDGES triggerEdge = TRIGGER_EDGES::RISING;
    TRIGGER_MODES triggerMode = TRIGGER_MODES::SOFTWARE;
};

class DataTransferConfiguration {
public:
    unsigned long transferBufferSize = 2048;
    unsigned long transferBufferCount = 32;
    unsigned long transferBufferQueueSize = 128;
};

class AnalogFrontEndChannelConfiguration: public ChannelConfiguration {
public:
    INPUT_RANGES desiredInputRange = INPUT_RANGES::MV_5000;
    float obtainedInputRange = 5000;
    int dcBias = 0;
};

class CalibrationChannelConfiguration: public ChannelConfiguration {
public:
    int digitalGain = DEFAULT_GAIN;
    int digitalOffset = 0;
    int analogOffset = 0;
};

class FileSaveConfiguration {
public:
    std::string tag;
    bool appendDate = false
    bool storeHeaders = false;
    unsigned long long fileSizeLimit = 2e9;
};

class RecordChannelConfiguration: public ChannelConfiguration  {
public:
    unsigned long recordCount = INFINITE_RECORDS;
    unsigned long recordLength = 256;
};

class SpectroscopeConfiguration {
public:
    bool enabled = false;
};

class AcquisitionConfiguration {
public:
    const static unsigned long INFINITE_RECORDS = -1;
    const static unsigned long NO_DURATION = 0;
    const static int DEFAULT_GAIN = 1024;

    CLOCK_SOURCES clockSource = CLOCK_SOURCES::INTSRC_INTREF_10MHZ;

    ACQUISITION_MODES triggeringMode = ACQUISITION_MODES::TRIGGERED;
    unsigned int sampleSkip = 1;
    unsigned long duration = 200;

    unsigned char userLogicBypassMask = 0b11;
    unsigned char channelMask = 0b0001;
    unsigned char triggerMask = 0b0001;
    TRIGGER_APPROACHES triggerApproach = TRIGGER_APPROACHES::SINGLE;

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
    static Acquisition fromJson(const QJsonObject &json);
    QJsonObject toJson();
    int getTotalDCShift(int ch, int& unclipped);
    static unsigned char verifyChannelMaskForSingularApproach(unsigned char channelMask);
    float getTargetInputRangeFloat(int channel);
};


#endif // DIGITIZERCONFIGURATION_H
