#ifndef QADQCONFIG_H
#define QADQCONFIG_H
#include "CalibrationTable.h"


class BufferConfiguration
{
    unsigned long long filesize = 1ull*1024ull*1024ull*1024ull;
    unsigned int queueLength = 64;
    unsigned int count = 32;
    unsigned long size = 2ul*1024ul;

public:
    unsigned long long getFilesize() const;
    void setFilesize(unsigned long long value);
    unsigned int getQueueLength() const;
    void setQueueLength(unsigned int value);
    unsigned int getCount() const;
    void setCount(unsigned int value);
    unsigned long getSize() const;
    void setSize(unsigned long value);
};
class TriggerConfiguration
{
    bool enabled = false;
    TRIGGER_MODES mode = TRIGGER_MODES::INTERNAL;
    TRIGGER_EDGES edge = TRIGGER_EDGES::FALLING;
    int level = 0;
    int reset = 0;
    unsigned char mask = 0b0001;
public:
    bool getEnabled() const;
    void setEnabled(bool value);
    TRIGGER_MODES getMode() const;
    void setMode(const TRIGGER_MODES &value);
    TRIGGER_EDGES getEdge() const;
    void setEdge(const TRIGGER_EDGES &value);
    int getLevel() const;
    void setLevel(int value);
    int getReset() const;
    void setReset(int value);
    unsigned char getMask() const;
    void setMask(unsigned char value);
};
class RecordConfiguration
{
    unsigned int length = 128;
    unsigned int count = -1; // -1 for infinity
    unsigned int pretrigger = 0;
    unsigned int delay = 0;

public:
    unsigned int getLength() const;
    void setLength(unsigned int value);
    unsigned int getCount() const;
    void setCount(unsigned int value);
    unsigned int getPretrigger() const;
    void setPretrigger(unsigned int value);
    unsigned int getDelay() const;
    void setDelay(unsigned int value);
};
class InputConfiguration
{
    unsigned char userLogicBypass = 0b11;
    float dcShift = 0;
    INPUT_RANGES range = INPUT_RANGES::MV_5000;

public:
    float getInputRangeMilivolts();
    float getDcShift() const;
    void setDcShift(float value);
    INPUT_RANGES getRange() const;
    void setRange(const INPUT_RANGES &value);
    unsigned char getUserLogicBypass() const;
    void setUserLogicBypass(unsigned char value);
};

class TimingConfiguration
{
    unsigned short sampleSkip = 1;
    CLOCK_SOURCES clockSource = CLOCK_SOURCES::INTSRC_INTREF_10MHZ;
    unsigned int duration = 0;

public:
    unsigned short getSampleSkip() const;
    void setSampleSkip(unsigned short value);
    CLOCK_SOURCES getClockSource() const;
    void setClockSource(const CLOCK_SOURCES &value);
    unsigned int getDuration() const;
    void setDuration(unsigned int value);
};

class QADQConfiguration
{
public:
    unsigned char getChannelMask() const;
    void setChannelMask(unsigned char value);

    BufferConfiguration &buffers() ;

    RecordConfiguration &records() ;

    InputConfiguration &input() ;

    TriggerConfiguration &trigger() ;

    TimingConfiguration &timing() ;

    CalibrationTable &calibrations();

private:
    unsigned char channelMask = 0b0000;
    unsigned int uiUpdatePeriod = 100;
    BufferConfiguration _buffers;
    RecordConfiguration _records;
    InputConfiguration _input;
    TriggerConfiguration _trigger;
    TimingConfiguration _timing;
    CalibrationTable _calibrationTable;
};

#endif // QADQCONFIG_H
