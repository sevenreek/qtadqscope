#ifdef MOCK_ADQAPI
#include "MockADQAPI.h"
#include <stdexcept>
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
unsigned int ADQControlUnit_EnableErrorTrace(void* adq_cu, unsigned int trace_level, const char *dir)
{
    return 0;
}
int ADQControlUnit_FindDevices(void* adq_cu)
{
    return 0;
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
    delete adqInterface;
    return;
}
ADQInterface * adqInterface;
ADQInterface * ADQControlUnit_GetADQ(void* adq_cu, unsigned int devicenum)
{
    adqInterface = new ADQInterface();
    return adqInterface;
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
unsigned int ADQInterface::SetSampleSkip(unsigned int tm)
{
    return 1;
}
unsigned int ADQInterface::SetInputRange(unsigned int channel, float in, float* out)
{
    *out = in;
    return 1;
}
unsigned int ADQInterface::BypassUserLogic(unsigned int channel, unsigned int bypass)
{
    return 1;
}
unsigned int ADQInterface::SetAdjustableBias(unsigned int channel, int bias)
{
    return 1;
}
int ADQInterface::SetTransferBuffers(unsigned int count, unsigned int size)
{
    this->bufferCount = count;
    this->bufferSize = size;
    return 1;
}
unsigned int ADQInterface::ContinuousStreamingSetup(unsigned char channelMask)
{
    this->recordLength = 0;
    this->channelMask = channelMask;
    return 1;
}
int ADQInterface::SetPreTrigSamples(unsigned int pt)
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
unsigned int ADQInterface::TriggeredStreamingSetup(unsigned int recordCount, unsigned int recordLength, unsigned int pretrigger, unsigned int delay, unsigned char channelMask)
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

unsigned int ADQInterface::GetTransferBufferStatus(unsigned int * buffersFilled)
{
    *buffersFilled = rand()/(float)(RAND_MAX)>(1-BUFFER_FILL_PROBABILITY)?2:0;
    return 1;
}
int ADQInterface::GetStreamOverflow()
{
    return 1;
}
unsigned int ADQInterface::SetGainAndOffset(unsigned char cahnnel, int Gain, int Offset)
{
    return 1;
}
int ADQInterface::SetTrigLevelResetValue(int val)
{
    return 1;
}
unsigned int ADQInterface::FlushDMA()
{
    return 1;
}

int ADQInterface::StartDataAcquisition()
{
    return 0;
}

int ADQInterface::StopDataAcquisition()
{
    return 0;
}

int64_t ADQInterface::WaitForRecordBuffer(int *channel, void **buffer, int timeout, ADQDataReadoutStatus *status)
{
    status->flags = 0;
    int recordChannel = *channel==ADQ_ANY_CHANNEL?0:*channel;
    int recordLength = this->recordLength==ADQ_INFINITE_RECORD_LENGTH?1024:this->recordLength;
    this->record.header = &this->header;
    this->record.header->Channel = recordChannel;
    this->record.header->RecordLength = recordLength;
    this->record.header->RecordStatus = 0;
    this->record.data = this->sourceData[recordChannel];
    *buffer = &this->record;
    return recordLength*sizeof(short);
}

int ADQInterface::ReturnRecordBuffer(int channel, void *buffer)
{
    return 1;
}

int ADQInterface::GetParameters(ADQParameterId id, void * const parameters)
{
    throw std::exception("Gen3 streaming not yet implemented in MockADQAPI");
}

int ADQInterface::SetParameters(void * const parameters)
{
    ADQDataAcquisitionParameters * acqp = (ADQDataAcquisitionParameters*)parameters;
    ADQParameterId id = acqp->id;
    switch(id)
    {
        case ADQParameterId::ADQ_PARAMETER_ID_DATA_ACQUISITION:
        {
            this->recordLength = acqp->channel[0].record_length;
        }
        break;
        case ADQParameterId::ADQ_PARAMETER_ID_DATA_TRANSFER:
            return sizeof(ADQDataTransferParameters);
        break;
        case ADQParameterId::ADQ_PARAMETER_ID_DATA_READOUT:
            return sizeof(ADQDataReadoutParameters);
        break;
        default:
            return 0;
        break;
    }
    return 1;
}

int ADQInterface::InitializeParameters(ADQParameterId id, void * const parameters)
{
    switch(id)
    {
        case ADQParameterId::ADQ_PARAMETER_ID_DATA_ACQUISITION: {
            ADQDataAcquisitionParameters * adap = reinterpret_cast<ADQDataAcquisitionParameters*>(parameters);
            adap->id = ADQParameterId::ADQ_PARAMETER_ID_DATA_ACQUISITION;
            for(int ch = 0; ch < ADQ_MAX_NOF_CHANNELS; ch++)
            {
                adap->channel[ch].horizontal_offset = 0;
                adap->channel[ch].record_length = 0;
                adap->channel[ch].nof_records = 0;
                adap->channel[ch].trigger_source = ADQ_EVENT_SOURCE_SOFTWARE;
                adap->channel[ch].trigger_edge = ADQ_EDGE_RISING;
                adap->channel[ch].trigger_blocking_source = static_cast<enum ADQFunction>(0);
            }
            return sizeof(ADQDataAcquisitionParameters);
        } break;
        case ADQParameterId::ADQ_PARAMETER_ID_DATA_TRANSFER:
            return sizeof(ADQDataTransferParameters);
        break;
        case ADQParameterId::ADQ_PARAMETER_ID_DATA_READOUT:
            return sizeof(ADQDataReadoutParameters);
        break;
        default:
            return 0;
        break;
    }
}

int ADQInterface::SetChannelSampleSkip(unsigned int channel, unsigned int skipfactor)
{
    return 1;
}

unsigned int ADQInterface::SetupLevelTrigger(int *level, int *edge, int *resetLevel, unsigned int channelMask, unsigned int individualMode)
{
    return 1;
}
int ADQInterface::WriteUserRegister(int ul_target, unsigned int regnum, unsigned int mask , unsigned int data, unsigned int *retval)
{
    *retval = data;
    return 1;
}

int ADQInterface::ReadBlockUserRegister(int ulTarget, uint32_t startAddr, uint32_t *data, uint32_t numBytes, uint32_t options)
{
    for(size_t i=0; i < numBytes/4; i++)
    {
        data[i] = i;
    }
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
            for (unsigned long i = 0; i < fullRecordsToAddInBuffer; i++)
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

void ADQInterface::loadBuffersFromFile(int channel, const char *file)
{
    throw std::exception("lodaBuffersFromFile is not implemented");
}



ADQInterface::~ADQInterface()
{
    for(int i = 0; i < 4; i++) free(sourceData[i]);
}
#endif
