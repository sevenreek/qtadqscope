#ifdef MOCK_ADQAPI
#include "MockADQAPI.h"
const char* FILE_DATA_SOURCE[] = {"ch1.mock", "ch2.mock", "ch3.mock", "ch4.mock"};
#include <algorithm>

ADQInterface::ADQInterface()
{
    spdlog::debug("Created new mock ADQInterface");
    for(int ch = 0; ch < 4; ch++)
    {
        this->sourceData[ch] = (short*)malloc(sizeof(short)*BUFFER_SIZE);
        std::ifstream file;
        file.open(FILE_DATA_SOURCE[ch], std::ios::binary);
        if(!file.good()) continue;
        file.read((char*)this->sourceData[ch], sizeof(short)*BUFFER_SIZE);
    }
}


void * CreateADQControlUnit()
{
    return nullptr;
}
void ADQControlUnit_EnableErrorTrace(void* adq_cu, unsigned int trace_level, const char *dir)
{
    return;
}
void ADQControlUnit_FindDevices(void* adq_cu)
{
    return;
}
int ADQControlUnit_NofADQ(void* adq_cu)
{
    return 1;
}
ADQInterface * ADQControlUnit_GetADQ(void* adq_cu, unsigned int devicenum)
{
    return new ADQInterface();
}

int ADQInterface::StopStreaming()
{
    this->streamActive = false;
    return 0;
}
int ADQInterface::StartStreaming()
{
    this->streamActive = true;
    return 0;
}
int ADQInterface::SetClockSource(int cs)
{
    return 0;
}
int ADQInterface::SetTriggerMode(int tm)
{
    return 0;
}
int ADQInterface::SetSampleSkip(int tm)
{
    return 0;
}
int ADQInterface::SetInputRange(int channel, float in, float* out)
{
    *out = in;
    return 0;
}
int ADQInterface::BypassUserLogic(int channel, int bypass)
{
    return 0;
}
int ADQInterface::SetAdjustableBias(int channel, int bias)
{
    return 0;
}
int ADQInterface::SetTransferBuffers(unsigned long count, unsigned long size)
{
    return 0;
}
int ADQInterface::ContinuousStreamingSetup(unsigned char channelMask)
{
    this->recordLength = 0;
    this->channelMask = channelMask;
    return 0;
}
int ADQInterface::SetPreTrigSamples(int pt)
{
    return 0;
}
int ADQInterface::SetLvlTrigLevel(int lev)
{
    return 0;
}
int ADQInterface::SetLvlTrigChannel(int mask)
{
    return 0;
}
int ADQInterface::SetLvlTrigEdge(int edge)
{
    return 0;
}
int ADQInterface::TriggeredStreamingSetup(unsigned long recordCount, unsigned long recordLength, unsigned long pretrigger, unsigned long delay, int channelMask)
{
    this->recordLength = recordLength;
    this->channelMask = channelMask;
    return 0;
}
int ADQInterface::SWTrig()
{
    return 0;
}

int ADQInterface::GetTransferBufferStatus(unsigned int * buffersFilled)
{
    *buffersFilled = 1;
    return 0;
}
int ADQInterface::GetStreamOverflow()
{
    return 0;
}
int ADQInterface::SetGainAndOffset(unsigned char cahnnel, int Gain, int Offset)
{
    return 0;
}
int ADQInterface::SetTrigLevelResetValue(int val)
{
    return 0;
}
int ADQInterface::WriteUserRegister(unsigned int ul_target, unsigned int regnum, unsigned int mask , unsigned int data, unsigned int *retval)
{
    return 0;
}

int ADQInterface::GetDataStreaming(void **d, void **h, unsigned char channelMask, unsigned int* samplesAdded, unsigned int * headersAdded, unsigned int * headerStatus)
{
    for(int ch = 0; ch < 4; ch++)
    {
        if(!(1<<ch & this->channelMask)) continue;
        if(this->recordLength == 0) // cont stream
        {
            memcpy(d[ch], this->sourceData[ch], std::min(BUFFER_SIZE, this->bufferSize));
            StreamingHeader_t * hp = (StreamingHeader_t *)h[ch];
            hp[0].Channel = ch+1;
            hp[0].RecordLength = std::min(BUFFER_SIZE, this->bufferSize);
            headersAdded[ch] = 1;
            samplesAdded[ch] = this->bufferSize/sizeof(short);
        }
        else
        {
            long rcount = this->bufferSize / this->recordLength;
            for (long i = 0; i < rcount; i++)
            {
                short * dp = (short*)d[ch];
                memcpy(&dp[i*this->recordLength], this->sourceData[ch], this->recordLength);
                StreamingHeader_t * hp = (StreamingHeader_t *)h[ch];
                hp[i].Channel = ch+1;
                hp[i].RecordLength = this->recordLength;
            }
            samplesAdded[ch] = rcount*this->recordLength;
            headersAdded[ch] = rcount;
        }
        headerStatus[ch] = 1;

    }
    return 0;
}



ADQInterface::~ADQInterface()
{
    for(int i = 0; i < 4; i++) free(sourceData[i]);
}
#endif
