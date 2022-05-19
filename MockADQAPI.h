
#ifndef MOCKAQAPI_H
#define MOCKAQAPI_H
#include "spdlog/spdlog.h"
#include <memory>
#include <iostream>
#include <fstream>
#include "MockADQAPIGen3Constants.h"
///
/// \brief The ADQInterface class
/// Although the real ADQAPI.h exposes an ADQInterface class-interface
/// inheriting from it requires the implementation of a huge number of
/// pure abstract functions. Every single API function is declared as such.
/// To avoid that this class redefines only those functions that are covered
/// during tests. If you happen to need another function, you will have to
/// declare it in this file and define it in MockADQAPI.cpp.
///
/// ADQ Control Unit functions are also redeclared below.
///
/// The issue with this solution is that ADQAPI.h and MockADQAPI.h cannot be both
/// included at the same time. This means that all enums and structs from ADQAPI.h
/// must also be redefined in MockADQAPIGen3Constants.h.


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
    ADQRecord record;
    ADQRecordHeader header;
public:
    static const unsigned long DEFAULT_BUFFER_SIZE;
    ADQInterface();
    virtual ~ADQInterface();
    int StopStreaming();
    int StartStreaming();
    int SetClockSource(int cs);
    int SetTriggerMode(int tm);
    unsigned int SetSampleSkip(unsigned int ss);
    unsigned int SetInputRange(unsigned int channel, float in, float* out);
    unsigned int BypassUserLogic(unsigned int channel, unsigned int bypass);
    unsigned int SetAdjustableBias(unsigned int channel, int bias);
    int SetTransferBuffers(unsigned int count, unsigned int size);
    unsigned int ContinuousStreamingSetup(unsigned char channelMask);
    int SetPreTrigSamples(unsigned int pt);
    int SetLvlTrigLevel(int lev);
    int SetLvlTrigChannel(int mask);
    int SetLvlTrigEdge(int edge);
    unsigned int TriggeredStreamingSetup(unsigned int recordCount, unsigned int recordLength, unsigned int pretrigger, unsigned int delay, unsigned char channelMask);
    int SWTrig();
    unsigned int SetGainAndOffset(unsigned char cahnnel, int Gain, int Offset);
    int SetTrigLevelResetValue(int val);
    int WriteUserRegister(int ul_target, unsigned int regnum, unsigned int mask , unsigned int data, unsigned int *retval);
    int ReadBlockUserRegister(int ulTarget, uint32_t startAddr, uint32_t *data, uint32_t numBytes, uint32_t options);
    int ReadUserRegister(unsigned int ul, unsigned int regnum, unsigned int *returval);

    unsigned int GetTransferBufferStatus(unsigned int * buffersFilled);
    int GetStreamOverflow();
    int GetDataStreaming(void **d, void **h, unsigned char channelMask, unsigned int* samplesAdded, unsigned int * headersAdded, unsigned int * headerStatus);

    void loadBuffersFromFile(int channel, const char* file);
    unsigned int FlushDMA();

    // Gen3 functions
    int StartDataAcquisition(void);
    int StopDataAcquisition(void);
    int64_t WaitForRecordBuffer(int *channel, void **buffer, int timeout, struct ADQDataReadoutStatus *status);
    int ReturnRecordBuffer(int channel, void *buffer);
    int GetParameters(enum ADQParameterId id, void *const parameters);
    int SetParameters(void *const parameters);
    int InitializeParameters(enum ADQParameterId id, void *const parameters);
    int SetChannelSampleSkip(unsigned int channel, unsigned int skipfactor);
    unsigned int SetupLevelTrigger(int * level, int * edge, int * resetLevel, unsigned int channelMask, unsigned int individualMode);
};
void * CreateADQControlUnit();
extern ADQInterface * adqInterface;
unsigned int ADQControlUnit_EnableErrorTrace(void* adq_cu, unsigned int trace_level, const char *dir);
int ADQControlUnit_FindDevices(void* adq_cu);
int ADQControlUnit_NofADQ(void* adq_cu);
void DeleteADQControlUnit(void* adq_cu_ptr);
void ADQControlUnit_DeleteADQ(void* adq_cu_ptr, int ADQ_num);
ADQInterface * ADQControlUnit_GetADQ(void* adq_cu, unsigned int devicenum);


#endif // MOCKAQAPI_H

