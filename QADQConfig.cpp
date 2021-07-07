#include "QADQConfig.h"
unsigned char QADQConfiguration::getChannelMask() const
{
    return channelMask;
}

void QADQConfiguration::setChannelMask(unsigned char value)
{
    channelMask = value;
}

BufferConfiguration &QADQConfiguration::buffers()
{
    return _buffers;
}

RecordConfiguration &QADQConfiguration::records()
{
    return _records;
}

InputConfiguration &QADQConfiguration::input()
{
    return _input;
}

TriggerConfiguration &QADQConfiguration::trigger()
{
    return _trigger;
}

TimingConfiguration &QADQConfiguration::timing()
{
    return _timing;
}

CalibrationTable &QADQConfiguration::calibrations()
{
    return _calibrationTable;
}


unsigned long long BufferConfiguration::getFilesize() const
{
    return filesize;
}

void BufferConfiguration::setFilesize(unsigned long long value)
{
    filesize = value;
}

unsigned int BufferConfiguration::getQueueLength() const
{
    return queueLength;
}

void BufferConfiguration::setQueueLength(unsigned int value)
{
    queueLength = value;
}

unsigned int BufferConfiguration::getCount() const
{
    return count;
}

void BufferConfiguration::setCount(unsigned int value)
{
    count = value;
}

unsigned long BufferConfiguration::getSize() const
{
    return size;
}

void BufferConfiguration::setSize(unsigned long value)
{
    size = value;
}

unsigned char InputConfiguration::getUserLogicBypass() const
{
    return userLogicBypass;
}

void InputConfiguration::setUserLogicBypass(unsigned char value)
{
    userLogicBypass = value;
}

float InputConfiguration::getInputRangeMilivolts()
{
    return INPUT_RANGE_VALUES[this->range];
}

float InputConfiguration::getDcShift() const
{
    return dcShift;
}

void InputConfiguration::setDcShift(float value)
{
    dcShift = value;
}

INPUT_RANGES InputConfiguration::getRange() const
{
    return range;
}

void InputConfiguration::setRange(const INPUT_RANGES &value)
{
    range = value;
}


unsigned int TimingConfiguration::getDuration() const
{
    return duration;
}

void TimingConfiguration::setDuration(unsigned int value)
{
    duration = value;
}

unsigned short TimingConfiguration::getSampleSkip() const
{
    return sampleSkip;
}

void TimingConfiguration::setSampleSkip(unsigned short value)
{
    sampleSkip = value;
}

CLOCK_SOURCES TimingConfiguration::getClockSource() const
{
    return clockSource;
}

void TimingConfiguration::setClockSource(const CLOCK_SOURCES &value)
{
    clockSource = value;
}

unsigned char TriggerConfiguration::getMask() const
{
    return mask;
}

void TriggerConfiguration::setMask(unsigned char value)
{
    mask = value;
}

bool TriggerConfiguration::getEnabled() const
{
    return enabled;
}

void TriggerConfiguration::setEnabled(bool value)
{
    enabled = value;
    if(!enabled)
        this->mode = SOFTWARE;
}

TRIGGER_MODES TriggerConfiguration::getMode() const
{
    return mode;
}

void TriggerConfiguration::setMode(const TRIGGER_MODES &value)
{
    mode = value;
}

TRIGGER_EDGES TriggerConfiguration::getEdge() const
{
    return edge;
}

void TriggerConfiguration::setEdge(const TRIGGER_EDGES &value)
{
    edge = value;
}

int TriggerConfiguration::getLevel() const
{
    return level;
}

void TriggerConfiguration::setLevel(int value)
{
    level = value;
}

int TriggerConfiguration::getReset() const
{
    return reset;
}

void TriggerConfiguration::setReset(int value)
{
    reset = value;
}

unsigned int RecordConfiguration::getLength() const
{
    return length;
}

void RecordConfiguration::setLength(unsigned int value)
{
    length = value;
}

unsigned int RecordConfiguration::getCount() const
{
    return count;
}

void RecordConfiguration::setCount(unsigned int value)
{
    count = value;
}

unsigned int RecordConfiguration::getPretrigger() const
{
    return pretrigger;
}

void RecordConfiguration::setPretrigger(unsigned int value)
{
    pretrigger = value;
}

unsigned int RecordConfiguration::getDelay() const
{
    return delay;
}

void RecordConfiguration::setDelay(unsigned int value)
{
    delay = value;
}
