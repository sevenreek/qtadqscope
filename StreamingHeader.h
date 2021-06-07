#ifndef STREAMINGHEADER_H
#define STREAMINGHEADER_H


#ifndef MOCK_ADQAPI
#include "ADQAPI.h"
typedef struct ADQRecordHeader StreamingHeader_t;
#else
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
#endif

#endif // STREAMINGHEADER_H
