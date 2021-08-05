#ifndef DIGITIZERCONFIGURATION_H
#define DIGITIZERCONFIGURATION_H
#include "DigitizerConstants.h"
#include <string>
#include <array>
#include <QJsonObject>

class Acquisition {
public:
    const static unsigned long INFINITE_RECORDS = -1;
    const static unsigned long NO_DURATION = 0;
    const static int DEFAULT_GAIN = 1024;
private:
    std::string tag;
    bool isContinuous = false;
    unsigned long duration = NO_DURATION;
    unsigned long transferBufferSize = 2048;
    unsigned long transferBufferCount = 32;
    unsigned long transferBufferQueueSize = 128;
    unsigned long long fileSizeLimit = 2e9;

    unsigned char userLogicBypassMask = 0b11;
    CLOCK_SOURCES clockSource = CLOCK_SOURCES::INTSRC_INTREF_10MHZ;
    TRIGGER_MODES triggerMode = TRIGGER_MODES::SOFTWARE;
    TRIGGER_EDGES triggerEdge = TRIGGER_EDGES::RISING;
    unsigned char triggerMask = 0b0001;
    int triggerLevel = 0;

    unsigned short pretrigger = 0;
    unsigned short triggerDelay = 0;

    unsigned long recordCount = INFINITE_RECORDS;
    unsigned long recordLength = 256;


    unsigned char channelMask = 0b0001;
    unsigned int sampleSkip = 1;

    std::array<INPUT_RANGES, MAX_NOF_CHANNELS> inputRange = {INPUT_RANGES::MV_5000};
    std::array<int, MAX_NOF_CHANNELS> dcBias = {0};
    std::array<int, MAX_NOF_CHANNELS> digitalGain = {DEFAULT_GAIN};
    std::array<int, MAX_NOF_CHANNELS> digitalOffset = {0};
    std::array<int, MAX_NOF_CHANNELS> analogOffset = {0};
    std::array<float, MAX_NOF_CHANNELS> obtainedInputRange = {5000};
public:
    std::string getTag() const;
    void setTag(const std::string &value);
    bool getIsContinuous() const;
    void setIsContinuous(bool value);
    unsigned long getDuration() const;
    void setDuration(unsigned long value);
    unsigned long getTransferBufferSize() const;
    void setTransferBufferSize(unsigned long value);
    unsigned long getTransferBufferCount() const;
    void setTransferBufferCount(unsigned long value);
    unsigned long getTransferBufferQueueSize() const;
    void setTransferBufferQueueSize(unsigned long value);
    unsigned char getUserLogicBypassMask() const;
    void setUserLogicBypassMask(unsigned char value);
    CLOCK_SOURCES getClockSource() const;
    void setClockSource(const CLOCK_SOURCES &value);
    TRIGGER_MODES getTriggerMode() const;
    void setTriggerMode(const TRIGGER_MODES &value);
    TRIGGER_EDGES getTriggerEdge() const;
    void setTriggerEdge(const TRIGGER_EDGES &value);
    unsigned char getTriggerMask() const;
    void setTriggerMask(unsigned char value);
    unsigned short getPretrigger() const;
    void setPretrigger(unsigned short value);
    unsigned short getTriggerDelay() const;
    void setTriggerDelay(unsigned short value);
    unsigned long getRecordCount() const;
    void setRecordCount(unsigned long value);
    unsigned long getRecordLength() const;
    void setRecordLength(unsigned long value);
    unsigned char getChannelMask() const;
    void setChannelMask(unsigned char value);
    unsigned int getSampleSkip() const;
    void setSampleSkip(unsigned int value);
    float getObtainedInputRange(int channel) const;
    void setObtainedInputRange(int channel, float value);
    float getDesiredInputRange(int channel) const;
    INPUT_RANGES getInputRange(int ch) const;
    void setInputRange(int ch, const INPUT_RANGES &value);
    int getDcBias(int ch) const;
    void setDcBias(int ch, int value);
    int getDigitalGain(int ch) const;
    void setDigitalGain(int ch, int value);
    int getDigitalOffset(int ch) const;
    void setDigitalOffset(int ch, int value);
    int getAnalogOffset(int ch) const;
    void setAnalogOffset(int ch, int value);
    int getTotalDCShift(int ch, int& unclipped);
    unsigned long long getFileSizeLimit() const;
    void setFileSizeLimit(unsigned long long value);
    int getTriggerLevel() const;
    void setTriggerLevel(int value);
    static Acquisition fromJSON(const QJsonObject &json);
    QJsonObject toJSON();
};


#endif // DIGITIZERCONFIGURATION_H