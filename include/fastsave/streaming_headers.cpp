#include "streaming_headers.hpp"
#include "spdlog/spdlog.h"

StreamingBuffers::StreamingBuffers(unsigned long bufferSize, unsigned char channelMask)
{
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
      this->headers[ch] = (StreamingHeader_t*)std::malloc(bufferSize);
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
    if((channelMask & (1<<ch)) == (1<<ch))
    {
      //spdlog::debug("Freeing data for channel {} from pointer {}", ch+1, fmt::ptr(this->data[ch]));
      free(this->data[ch]);
      //spdlog::debug("Freeing headers for channel {}", ch+1);
      free(this->headers[ch]);
    }
  }
}

WriteBuffers::WriteBuffers(unsigned int bufferCount, unsigned long bufferSize, unsigned char channelMask) :
  sWrite(bufferCount), sRead(0), sFile(1)
{
  this->bufferCount = bufferCount;
  for(int b = 0; b < this->bufferCount; b++)
  {
    StreamingBuffers * bfp = new StreamingBuffers(bufferSize, channelMask);
    if(bfp == NULL) {
      spdlog::critical("Insufficient memory for buffers allocate.");
    }
    //spdlog::debug("Pushing buffer {} address {}", b, fmt::ptr(bfp));
    this->buffers.push_back(bfp);
  }
  //spdlog::debug("writePos={}; readPos={}", this->writePosition, this->readPosition);

}
WriteBuffers::~WriteBuffers()
{
  for(int b = 0; b < this->bufferCount; b++)
  {
    //spdlog::debug("Deleting buffer {} address {}", b, fmt::ptr(this->buffers[b]));
    delete this->buffers[b];
    this->buffers[b] = nullptr;
  }
  //this->buffers.clear();
}
StreamingBuffers* WriteBuffers::awaitWrite()
{
  this->sWrite.wait();
  //spdlog::debug("Lock on write buffer {} obtained.", this->writePosition);
  unsigned int returnWritePos = this->writePosition;
  this->writePosition = (this->writePosition+1)%this->bufferCount;
  return this->buffers[returnWritePos];
}
StreamingBuffers* WriteBuffers::awaitRead()
{
  this->sRead.wait();
  //spdlog::debug("Lock on read buffer {} obtained.", this->readPosition);
  unsigned int returnReadPos = this->readPosition;
  this->readPosition = (this->readPosition+1)%this->bufferCount;
  return this->buffers[returnReadPos];
}
void WriteBuffers::notifyWritten()
{
  this->sRead.notify();
}
void WriteBuffers::notifyRead()
{
  this->sWrite.notify();
}