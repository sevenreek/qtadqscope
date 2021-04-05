#ifndef STREAMINGBUFFERS_H
#define STREAMINGBUFFERS_H
#include "Semaphore.h"
#include <vector>
#include <memory>
#include "StreamingHeader.h"
#include "ADQDeviceConfiguration.h"
class StreamingBuffers {
  public:
  StreamingBuffers(unsigned long bufferSize, unsigned char channelMask);
  ~StreamingBuffers();

  void reallocate(unsigned long bufferSize, unsigned char channelMask);
  unsigned long bufferSize;
  unsigned char     channelMask;
  unsigned int      nof_headers[MAX_NOF_CHANNELS];
  unsigned int      nof_samples[MAX_NOF_CHANNELS];
  unsigned int      header_status[MAX_NOF_CHANNELS];
  short             *data[MAX_NOF_CHANNELS] = {nullptr};
  StreamingHeader_t *headers[MAX_NOF_CHANNELS] = {nullptr};
};

class WriteBuffers {
private:
  unsigned int bufferCount;
  unsigned int readPosition = 0;
  unsigned int writePosition = 0;
public:
  WriteBuffers(unsigned int bufferCount, unsigned long bufferSize, unsigned char channelMask);
  ~WriteBuffers();
  void reconfigure(unsigned int bufferCount, unsigned long bufferSize, unsigned char channelMask);
  std::vector<StreamingBuffers*> buffers = {nullptr};
  Semaphore sWrite;
  Semaphore sRead;
  StreamingBuffers* awaitWrite(int timeout);
  StreamingBuffers* awaitRead(int timeout);
  int getWriteCount();
  int getReadCount();
  void notifyWritten();
  void notifyRead();
  void resetSemaphores();
};
#endif
