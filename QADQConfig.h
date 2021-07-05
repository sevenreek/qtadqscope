#ifndef QADQCONFIG_H
#define QADQCONFIG_H

enum TRIGGER_MODES
{
    SOFTWARE = 1,
    EXTERNAL = 2,
    LEVEL = 3,
    INTERNAL = 4,
    EXTERNAL_2 = 7,
    EXTERNAL_3 = 8
};

enum TRIGGER_EDGES
{
    FALLING = 0,
    RISING = 1,
    BOTH = 2
};

enum INPUT_RANGES
{
    MV_100 = 0,
    MV_250 = 1,
    MV_500 = 2,
    MV_1000 = 3,
    MV_2000 = 4,
    MV_5000 = 5,
    MV_10000 = 6
};
const int INPUT_RANGE_COUNT = (7);
const float INPUT_RANGE_VALUES[INPUT_RANGE_COUNT] = {100, 250, 500, 1000, 2000, 5000, 10000};
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
    TRIGGER_MODES mode;
    TRIGGER_EDGES edge;
    int level = 0;
    int reset = 0;
};
class RecordConfiguration
{
    unsigned int length;
    unsigned int count;
    unsigned int pretrigger = 0;
    unsigned int delay = 0;
};
class InputConfiguration
{
    int analogOffset = 0;
    int dcShift = 0;
    INPUT_RANGES range;
    int digitalGain = 1024;
    int digitaalOffset = 0;
};

class QADQConfiguration
{
public:
    unsigned char getChannelMask() const;
    void setChannelMask(unsigned char value);

    BufferConfiguration buffers() const;

    RecordConfiguration records() const;

    InputConfiguration input() const;

    TriggerConfiguration trigger() const;

private:
    unsigned char channelMask = 0b0000;
    unsigned int uiUpdatePeriod = 100;
    BufferConfiguration _buffers;
    RecordConfiguration _records;
    InputConfiguration _input;
    TriggerConfiguration _trigger;
};

#endif // QADQCONFIG_H
