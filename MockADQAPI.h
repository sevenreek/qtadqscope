#ifdef MOCK_ADQAPI
#ifndef MOCKAQAPI_H
#define MOCKAQAPI_H
#include "spdlog/spdlog.h"
#include <memory>
#include <iostream>
#include <fstream>
#include "StreamingHeader.h"
#define BUFFER_SIZE (4096UL)

class ADQInterface
{
private:
    bool streamActive = false;
    unsigned char channelMask;
    unsigned long bufferSize;
    unsigned long bufferCount;
    unsigned long recordLength = 0;
    short *sourceData[4];
public:
    ADQInterface();
    virtual ~ADQInterface();
    int StopStreaming();
    int StartStreaming();
    int SetClockSource(int cs);
    int SetTriggerMode(int tm);
    int SetSampleSkip(int ss);
    int SetInputRange(int channel, float in, float* out);
    int BypassUserLogic(int channel, int bypass);
    int SetAdjustableBias(int channel, int bias);
    int SetTransferBuffers(unsigned long count, unsigned long size);
    int ContinuousStreamingSetup(unsigned char channelMask);
    int SetPreTrigSamples(int pt);
    int SetLvlTrigLevel(int lev);
    int SetLvlTrigChannel(int mask);
    int SetLvlTrigEdge(int edge);
    int TriggeredStreamingSetup(unsigned long recordCount, unsigned long recordLength, unsigned long pretrigger, unsigned long delay, int channelMask);
    int SWTrig();
    int SetGainAndOffset(unsigned char cahnnel, int Gain, int Offset);
    int SetTrigLevelResetValue(int val);
    int WriteUserRegister(unsigned int ul_target, unsigned int regnum, unsigned int mask , unsigned int data, unsigned int *retval);


    int GetTransferBufferStatus(unsigned int * buffersFilled);
    int GetStreamOverflow();
    int GetDataStreaming(void **d, void **h, unsigned char channelMask, unsigned int* samplesAdded, unsigned int * headersAdded, unsigned int * headerStatus);

    void loadBuffersFromFile(int channel, const char* file);
};
void * CreateADQControlUnit();
void ADQControlUnit_EnableErrorTrace(void* adq_cu, unsigned int trace_level, const char *dir);
void ADQControlUnit_FindDevices(void* adq_cu);
int ADQControlUnit_NofADQ(void* adq_cu);
ADQInterface * ADQControlUnit_GetADQ(void* adq_cu, unsigned int devicenum);


#endif // MOCKAQAPI_H
#endif
