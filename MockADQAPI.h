#ifdef MOCK_ADQAPI
#ifndef MOCKAQAPI_H
#define MOCKAQAPI_H
#include "spdlog/spdlog.h"
#include <memory>
#include <iostream>
#include <fstream>
#include "StreamingHeader.h"

class ADQInterface
{
private:
    bool streamActive = false;
    unsigned char channelMask;
    unsigned long bufferSize;
    unsigned long bufferCount;
    unsigned long recordLength = 0;
    short *sourceData[4] = {nullptr,nullptr,nullptr,nullptr};
    unsigned long remainingSamplesInRecord[4] = {0,0,0,0};
    unsigned long long recordNumber = 0;
public:
    static const unsigned long DEFAULT_BUFFER_SIZE;
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
    int FlushDMA();
};
void * CreateADQControlUnit();
extern ADQInterface * adqInterface;
void ADQControlUnit_EnableErrorTrace(void* adq_cu, unsigned int trace_level, const char *dir);
void ADQControlUnit_FindDevices(void* adq_cu);
int ADQControlUnit_NofADQ(void* adq_cu);
void DeleteADQControlUnit(void* adq_cu_ptr);
void ADQControlUnit_DeleteADQ(void* adq_cu_ptr, int ADQ_num);
ADQInterface * ADQControlUnit_GetADQ(void* adq_cu, unsigned int devicenum);


#endif // MOCKAQAPI_H
#endif
