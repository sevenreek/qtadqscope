#ifndef STREAMINGBUFFERS_H
#define STREAMINGBUFFERS_H
#include "Semaphore.h"
#include <vector>
#include <memory>
#include "StreamingHeader.h"
#include "DigitizerConstants.h"
#include "ThreadSafeQueue.h"
class StreamingBuffers {
  public:
  StreamingBuffers(unsigned long bufferSize, unsigned char channelMask, unsigned int recordLength);
  ~StreamingBuffers();

  unsigned long bufferSize;
  unsigned char     channelMask;
  unsigned int      nof_headers[MAX_NOF_CHANNELS] = {1,1,1,1};
  unsigned int      nof_samples[MAX_NOF_CHANNELS] = {0,0,0,0};
  unsigned int      header_status[MAX_NOF_CHANNELS] = {0,0,0,0};
  short             *data[MAX_NOF_CHANNELS] = {nullptr,nullptr,nullptr,nullptr};
  ADQRecordHeader *headers[MAX_NOF_CHANNELS] = {nullptr,nullptr,nullptr,nullptr};
};

class WriteBuffers {
private:
  unsigned int bufferCount;
  unsigned int readPosition = 0;
  unsigned int writePosition = 0;
public:
  WriteBuffers(unsigned int bufferCount, unsigned long bufferSize, unsigned char channelMask, unsigned int recordLength);
  ~WriteBuffers();
  void reconfigure(unsigned int bufferCount, unsigned long bufferSize, unsigned char channelMask, unsigned int recordLength);
  std::vector<StreamingBuffers*> buffers;
  ThreadsafeQueue<StreamingBuffers*> readQueue;
  ThreadsafeQueue<StreamingBuffers*> writeQueue;
  StreamingBuffers* popReadBuffer();
  StreamingBuffers* popWriteBuffer();
  bool pushWrittenBuffer(StreamingBuffers * buffer);
  bool pushReadDoneBuffer(StreamingBuffers * buffer);
  long getWriteBufferCount();
  long getReadBufferCount();
};
#endif
