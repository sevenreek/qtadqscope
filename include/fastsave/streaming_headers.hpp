#pragma once
#define MAX_NOF_CHANNELS 4
#define MIN_SIZE_RECORD (28)
#include "semaphore.hpp"
#include <vector>
#include <memory>
typedef struct
{
  unsigned char          OverRange     : 1;  // Record status byte                   X
  unsigned char          FIFOFill      : 3;  // Record status byte
  unsigned char          LostData      : 4;  // Record status byte                   X
  unsigned char          UserID;             // UserID byte                          X
  unsigned char          Channel;            // Channel byte                         X
  unsigned char          Reserved0     : 7;  // Data format byte padding             -
  unsigned char          DataFormat    : 1;  // Data format byte (1 bit)             X
  unsigned int           SerialNumber;       // Serial number (32 bits)              X
  unsigned int           RecordNumber;       // Record number (32 bits)              X
  unsigned int           SamplePeriod;       // Sample period (32 bits)              X
  long long unsigned int Timestamp;          // Timestamp (64 bits)                  X
  long long int          RecordStart;        // Record start timestamp (64 bits)     X
  unsigned int           RecordLength;       // Record length (32 bits)              X
  unsigned short         GeneralPurpose0;    // Moving average (16 bits)             X
  unsigned short         GeneralPurpose1;    // Gate counter (16 bits)               X
} StreamingHeader_t;

class StreamingBuffers {
  public:
  StreamingBuffers(unsigned long bufferSize, unsigned char channelMask);
  ~StreamingBuffers();
  unsigned char     channelMask;
  unsigned int      nof_headers[MAX_NOF_CHANNELS];
  unsigned int      nof_samples[MAX_NOF_CHANNELS];
  unsigned int      header_status[MAX_NOF_CHANNELS];
  short             *data[MAX_NOF_CHANNELS];
  StreamingHeader_t *headers[MAX_NOF_CHANNELS];
};

class WriteBuffers {
private:
  unsigned int bufferCount;
  unsigned int readPosition = 0;
  unsigned int writePosition = 0;
public:
  WriteBuffers(unsigned int bufferCount, unsigned long bufferSize, unsigned char channelMask);
  ~WriteBuffers();
  std::vector<StreamingBuffers*> buffers = {};
  Semaphore sWrite;
  Semaphore sRead;
  Semaphore sFile;
  bool stopWriteThreads = false;
  StreamingBuffers* awaitWrite();
  StreamingBuffers* awaitRead();
  void notifyWritten();
  void notifyRead();
};
