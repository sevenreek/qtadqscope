#ifdef MOCK_ADQAPI
#include "MockADQAPI.h"
const char* FILE_DATA_SOURCE[] = {"ch1.mock", "ch2.mock", "ch3.mock", "ch4.mock"};
#include <algorithm>
#include <random>

ADQInterface::ADQInterface()
{
    spdlog::debug("Created new mock ADQInterface");
    for(int ch = 0; ch < 4; ch++)
    {
        this->sourceData[ch] = (short*)malloc(sizeof(short)*DEFAULT_BUFFER_SIZE);
        std::ifstream file;
        file.open(FILE_DATA_SOURCE[ch], std::ios::binary);
        if(!file.good()) continue;
        file.read((char*)this->sourceData[ch], sizeof(short)*DEFAULT_BUFFER_SIZE);
    }
}
const unsigned long ADQInterface::DEFAULT_BUFFER_SIZE = 4096UL;

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
    return 1;
}
int ADQInterface::StartStreaming()
{
    this->streamActive = true;
    return 1;
}
int ADQInterface::SetClockSource(int cs)
{
    return 1;
}
int ADQInterface::SetTriggerMode(int tm)
{
    return 1;
}
int ADQInterface::SetSampleSkip(int tm)
{
    return 1;
}
int ADQInterface::SetInputRange(int channel, float in, float* out)
{
    *out = in;
    return 1;
}
int ADQInterface::BypassUserLogic(int channel, int bypass)
{
    return 1;
}
int ADQInterface::SetAdjustableBias(int channel, int bias)
{
    return 1;
}
int ADQInterface::SetTransferBuffers(unsigned long count, unsigned long size)
{
    this->bufferCount = count;
    this->bufferSize = size;
    return 1;
}
int ADQInterface::ContinuousStreamingSetup(unsigned char channelMask)
{
    this->recordLength = 0;
    this->channelMask = channelMask;
    return 1;
}
int ADQInterface::SetPreTrigSamples(int pt)
{
    return 1;
}
int ADQInterface::SetLvlTrigLevel(int lev)
{
    return 1;
}
int ADQInterface::SetLvlTrigChannel(int mask)
{
    return 1;
}
int ADQInterface::SetLvlTrigEdge(int edge)
{
    return 1;
}
int ADQInterface::TriggeredStreamingSetup(unsigned long recordCount, unsigned long recordLength, unsigned long pretrigger, unsigned long delay, int channelMask)
{
    this->recordLength = recordLength;
    this->channelMask = channelMask;
    this->recordNumber = 0;
    return 1;
}
int ADQInterface::SWTrig()
{
    return 1;
}

int ADQInterface::GetTransferBufferStatus(unsigned int * buffersFilled)
{
    *buffersFilled = rand()/(float)(RAND_MAX)>0.9?1:0;
    return 1;
}
int ADQInterface::GetStreamOverflow()
{
    return 1;
}
int ADQInterface::SetGainAndOffset(unsigned char cahnnel, int Gain, int Offset)
{
    return 1;
}
int ADQInterface::SetTrigLevelResetValue(int val)
{
    return 1;
}
int ADQInterface::FlushDMA()
{
    return 1;
}
int ADQInterface::WriteUserRegister(unsigned int ul_target, unsigned int regnum, unsigned int mask , unsigned int data, unsigned int *retval)
{
    return 1;
}

int ADQInterface::GetDataStreaming(void **d, void **h, unsigned char channelMask, unsigned int* samplesAdded, unsigned int * headersAdded, unsigned int * headerStatus)
{
    for(int ch = 0; ch < 4; ch++)
    {
        if(!(1<<ch & this->channelMask)) continue;
        if(this->recordLength == 0) // cont stream
        {
            memcpy(d[ch], this->sourceData[ch], std::min(DEFAULT_BUFFER_SIZE, this->bufferSize));
            StreamingHeader_t * hp = (StreamingHeader_t *)h[ch];
            hp[0].Channel = ch;
            hp[0].RecordLength = std::min(DEFAULT_BUFFER_SIZE, this->bufferSize);
            hp[0].RecordNumber = this->recordNumber++;
            headersAdded[ch] = 1;
            samplesAdded[ch] = this->bufferSize/sizeof(short);
        }
        else
        {
           long rcount = this->bufferSize / ( this->recordLength * sizeof(short) );
            for (long i = 0; i < rcount; i++)
            {
                short * dp = (short*)(d[ch]);
                memcpy(&(dp[i*this->recordLength]), this->sourceData[ch], this->recordLength*sizeof(short));
                StreamingHeader_t * hp = (StreamingHeader_t *)(h[ch]);
                hp[i].Channel = ch;
                hp[i].RecordLength = this->recordLength;
                hp[i].RecordNumber = this->recordNumber++;
            }
            samplesAdded[ch] = rcount*this->recordLength;
            headersAdded[ch] = rcount; /*
            StreamingHeader_t * hp = (StreamingHeader_t *)(h[ch]);
            hp[0].Channel = ch;
            hp[0].RecordLength = this->recordLength;
            hp[0].RecordNumber = this->recordNumber++;
            samplesAdded[ch] = this->recordLength;
            headersAdded[ch] = 1;*/
        }
        headerStatus[ch] = 1;

    }
    return 1;
}



ADQInterface::~ADQInterface()
{
    for(int i = 0; i < 4; i++) free(sourceData[i]);
}
#endif
