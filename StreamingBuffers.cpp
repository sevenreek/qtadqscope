#include "StreamingBuffers.h"
#include "spdlog/spdlog.h"

StreamingBuffers::StreamingBuffers(unsigned long bufferSize, unsigned char channelMask, unsigned int recordLength)
{
    this->bufferSize = bufferSize;
    this->channelMask = channelMask;
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
      if((channelMask & (1<<ch)) == (1<<ch))
      {
        this->data[ch] = (short*)std::malloc(bufferSize);
        //spdlog::debug("Allocated {} for channel {} in pointer {}", bufferSize, ch+1, fmt::ptr(this->data[ch]));
        if(this->data[ch] == nullptr)
        {
          spdlog::critical("Out of memory for data buffer for channel {}", ch+1);
        }
        unsigned int headerCount = bufferSize/sizeof(short)/recordLength + 1; // a single buffer can contain bufferSize/sizeof(short) samples so a total of that/recordLength + 1 headers
        //this->headers[ch] = (ADQRecordHeader*)std::malloc(headerCount*sizeof(ADQRecordHeader));
        this->headers[ch] = (ADQRecordHeader*)std::malloc(bufferSize);
        if(this->headers[ch] == nullptr)
        {
          spdlog::critical("Out of memory for headers buffer for channel {}", ch+1);
        }
      }
    }
}
StreamingBuffers::~StreamingBuffers()
{
  for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
  {
    if(this->data[ch])
    {
      //spdlog::debug("Freeing data for channel {} from pointer {}", ch+1, fmt::ptr(this->data[ch]));
      free(this->data[ch]);
      this->data[ch] = nullptr;
    }
    if(this->headers[ch])
    {
      //spdlog::debug("Freeing headers for channel {}", ch+1);
      free(this->headers[ch]);
      this->headers[ch] = nullptr;
    }
  }
}

WriteBuffers::WriteBuffers(unsigned int bufferCount, unsigned long bufferSize, unsigned char channelMask, unsigned int recordLength) :
    readQueue(bufferCount), writeQueue(bufferCount)
{
    if(recordLength == 0) recordLength = 2; // for continuous streaming;
    this->bufferCount = bufferCount;
    this->buffers.clear();
    this->buffers.reserve(this->bufferCount);
    for(unsigned int b = 0; b < this->bufferCount; b++) {
        StreamingBuffers * bfp = new StreamingBuffers(bufferSize, channelMask, recordLength);
        if(bfp == nullptr) {
          spdlog::critical("Insufficient memory for buffers allocate.");
        }
        //spdlog::debug("Pushing buffer {} address {}", b, fmt::ptr(bfp));
        this->buffers.push_back(bfp);
    }
    //spdlog::debug("writePos={}; readPos={}", this->writePosition, this->readPosition);

}
WriteBuffers::~WriteBuffers()
{
  for(unsigned int b = 0; b < this->bufferCount; b++)
  {
    //spdlog::debug("Deleting buffer {} address {}", b, fmt::ptr(this->buffers[b]));
    if(this->buffers[b] == nullptr) continue;
    delete this->buffers[b];
    //this->buffers[b] = nullptr;
  }
  //this->buffers.clear();
}
void WriteBuffers::reconfigure(unsigned int bufferCount, unsigned long bufferSize, unsigned char channelMask, unsigned int recordLength)
{
    this->readQueue.clear(bufferCount);
    this->writeQueue.clear(bufferCount);
    if(recordLength == 0) recordLength = 2; // for continuous streaming;
    for(auto p : this->buffers)
    {
        delete p;
    }
    /*for(unsigned int b = 0; b < this->bufferCount; b++)
    {
      //spdlog::debug("Deleting buffer {} address {}", b, fmt::ptr(this->buffers[b]));
      if(this->buffers[b] != nullptr)
      {
        delete this->buffers[b];
        //this->buffers[b] = nullptr;
      }
    }*/
    this->buffers.clear();
    this->buffers.reserve(this->bufferCount);
    this->bufferCount = bufferCount;
    for(unsigned int b = 0; b < this->bufferCount; b++)
    {
      StreamingBuffers * bfp = new StreamingBuffers(bufferSize, channelMask, recordLength);
      if(bfp == nullptr) {
        spdlog::critical("Insufficient memory for buffers allocate.");
      }
      //spdlog::debug("Pushing buffer {} address {}", b, fmt::ptr(bfp));
      this->buffers.push_back(bfp);
      this->writeQueue.push(bfp);
    }

}

StreamingBuffers* WriteBuffers::popReadBuffer()
{
    return this->readQueue.pop();
}

StreamingBuffers*  WriteBuffers::popWriteBuffer()
{
    return this->writeQueue.pop();
}
bool WriteBuffers::pushWrittenBuffer(StreamingBuffers *buffer)
{
    return this->readQueue.push(buffer);
}

bool WriteBuffers::pushReadDoneBuffer(StreamingBuffers *buffer)
{
    return this->writeQueue.push(buffer);
}

long WriteBuffers::getWriteBufferCount()
{
    return this->writeQueue.size();
}

long WriteBuffers::getReadBufferCount()
{
    return this->readQueue.size();
}
