#include "QADQConfig.h"
unsigned char QADQConfiguration::getChannelMask() const
{
    return channelMask;
}

void QADQConfiguration::setChannelMask(unsigned char value)
{
    channelMask = value;
}

BufferConfiguration QADQConfiguration::buffers() const
{
    return _buffers;
}

RecordConfiguration QADQConfiguration::records() const
{
    return _records;
}

InputConfiguration QADQConfiguration::input() const
{
    return _input;
}

TriggerConfiguration QADQConfiguration::trigger() const
{
    return _trigger;
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
