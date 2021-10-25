#ifdef MOCK_ADQAPI
#include "MockADQAPI.h"
const char* FILE_DATA_SOURCE[] = {"ch1.mock", "ch2.mock", "ch3.mock", "ch4.mock"};
const float BUFFER_FILL_PROBABILITY = 0.0001;
#include <algorithm>
#include <random>
const unsigned long ADQInterface::DEFAULT_BUFFER_SIZE = 4096UL*4UL;
//#define LOAD_FROM_FILE
ADQInterface::ADQInterface()
{
    spdlog::debug("Created new mock ADQInterface");
    for(int ch = 0; ch < 4 ; ch++)
    {
        this->sourceData[ch] = reinterpret_cast<short*>(std::malloc(sizeof(short)*DEFAULT_BUFFER_SIZE));
#ifdef LOAD_FROM_FILE
        std::ifstream file;
        file.open(FILE_DATA_SOURCE[ch], std::ios::binary);
        if(!file.good()) continue;
        std::memset(this->sourceData[ch], 0, sizeof(short)*DEFAULT_BUFFER_SIZE);
        file.read(reinterpret_cast<char*>(this->sourceData[ch]), sizeof(short)*DEFAULT_BUFFER_SIZE);
        file.close();
#else
        for(size_t sa = 0; sa < DEFAULT_BUFFER_SIZE/sizeof(short); sa++)
        {
            this->sourceData[ch][sa] = (sa+1) * (ch+1);
        }
#endif
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
void DeleteADQControlUnit(void* adq_cu_ptr)
{
    return;
}
void ADQControlUnit_DeleteADQ(void* adq_cu_ptr, int ADQ_num)
{
    return;
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
    for(int ch=0; ch<4; ch++)
    {
        this->remainingSamplesInRecord[ch] = 0;
        this->recordNumber = 0;

    }
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
    *buffersFilled = rand()/(float)(RAND_MAX)>(1-BUFFER_FILL_PROBABILITY)?2:0;
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
    *retval = data;
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
            ADQRecordHeader * hp = (ADQRecordHeader *)h[ch];
            hp[0].Channel = ch;
            hp[0].RecordLength = std::min(DEFAULT_BUFFER_SIZE, this->bufferSize);
            hp[0].RecordNumber = this->recordNumber++;
            headersAdded[ch] = 1;
            samplesAdded[ch] = this->bufferSize/sizeof(short);
        }
        else
        {
            //spdlog::debug("=== Buffer ===");
            samplesAdded[ch] = 0;
            headersAdded[ch] = 0;
            headerStatus[ch] = 1;
            unsigned long samplesPerBuffer = this->bufferSize / sizeof(short);
            unsigned long remainingSamplesToAdd = this->remainingSamplesInRecord[ch]<=samplesPerBuffer?this->remainingSamplesInRecord[ch]:samplesPerBuffer;
            short * dp = reinterpret_cast<short*>(d[ch]);
            if(remainingSamplesToAdd)
            {

                for(size_t sa = 0; sa < remainingSamplesToAdd; sa++)
                {
                    this->sourceData[ch][ this->recordLength - this->remainingSamplesInRecord[ch] + sa] = this->recordNumber%32 * 1000 + this->recordLength - this->remainingSamplesInRecord[ch] + sa;
                }
                memcpy(&dp[0], &this->sourceData[ch][this->recordLength - this->remainingSamplesInRecord[ch]], remainingSamplesToAdd*sizeof(short));
                headersAdded[ch] += 1;
                samplesAdded[ch] += remainingSamplesToAdd;
                this->remainingSamplesInRecord[ch] -= remainingSamplesToAdd;
                if(this->remainingSamplesInRecord[ch]) // no more room
                {
                    headerStatus[ch] = 0;
                    continue;
                }
                else
                {
                    ADQRecordHeader * hp = (ADQRecordHeader *)(h[ch]);
                    memset(&hp[0], 0, sizeof(ADQRecordHeader));
                    hp[0].Channel = ch;
                    hp[0].RecordLength = this->recordLength;
                    hp[0].RecordNumber = this->recordNumber++;
                    //spdlog::debug("Stored [{} {} {}] at i={}", hp[0].Channel, hp[0].RecordLength,  hp[0].RecordNumber, 0);
                }
            }
            unsigned long samplesToAddInBuffer = samplesPerBuffer - remainingSamplesToAdd;
            unsigned long fullRecordsToAddInBuffer = samplesToAddInBuffer / this->recordLength;
            for (long i = 0; i < fullRecordsToAddInBuffer; i++)
            {
                for(size_t sa = 0; sa < this->recordLength; sa++)
                {
                    this->sourceData[ch][sa] = this->recordNumber%32 * 1000 + sa;
                }
                memcpy(&(dp[samplesAdded[ch]]), this->sourceData[ch], this->recordLength*sizeof(short));
                ADQRecordHeader * hp = (ADQRecordHeader *)(h[ch]);
                memset(&hp[headersAdded[ch]], 0, sizeof(ADQRecordHeader));
                hp[headersAdded[ch]].Channel = ch;
                hp[headersAdded[ch]].RecordLength = this->recordLength;
                hp[headersAdded[ch]].RecordNumber = this->recordNumber++;
                //spdlog::debug("Stored [{} {} {}] at i={}", hp[headersAdded[ch]].Channel, hp[headersAdded[ch]].RecordLength,  hp[headersAdded[ch]].RecordNumber, headersAdded[ch]);
                samplesAdded[ch] += this->recordLength;
                headersAdded[ch] += 1;
            }
            unsigned long leftoverBufferSpace = samplesToAddInBuffer - fullRecordsToAddInBuffer*this->recordLength;
            if(leftoverBufferSpace)
            {
                //spdlog::debug("Leftover {}", leftoverBufferSpace);
                if(this->remainingSamplesInRecord[ch])
                {
                    for(size_t sa = 0; sa < leftoverBufferSpace; sa++)
                    {
                        this->sourceData[ch][sa] = this->recordNumber%32 * 1000 + this->recordLength-this->remainingSamplesInRecord[ch] + sa;
                    }
                    memcpy(&(dp[samplesAdded[ch]]), &this->sourceData[ch][this->recordLength-this->remainingSamplesInRecord[ch]], leftoverBufferSpace*sizeof(short));
                }
                else
                {
                    for(size_t sa = 0; sa < leftoverBufferSpace; sa++)
                    {
                        this->sourceData[ch][sa] = headersAdded[ch]%32 * 1000 + sa;
                    }
                    memcpy(&(dp[samplesAdded[ch]]), &this->sourceData[ch][0], leftoverBufferSpace*sizeof(short));
                    remainingSamplesInRecord[ch] = this->recordLength - leftoverBufferSpace;
                }
                samplesAdded[ch] += leftoverBufferSpace;
                headerStatus[ch] = 0;
                headersAdded[ch] += 1;
            }
        }

    }
    return 1;
}



ADQInterface::~ADQInterface()
{
    for(int i = 0; i < 4; i++) free(sourceData[i]);
}
#endif
