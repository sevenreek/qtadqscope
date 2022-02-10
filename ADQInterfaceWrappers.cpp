#include "ADQInterfaceWrappers.h"
#include <QThread>

MutexADQWrapper::MutexADQWrapper(void* adqCU, unsigned int devnum) :
    ADQInterfaceWrapper(adqCU, devnum)
{

}

bool MutexADQWrapper::SetAdjustableBias(int channel, int code)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetAdjustableBias(channel, code);
}

bool MutexADQWrapper::SetInputRange(int channel, float target, float *result)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetInputRange(channel, target, result);
}

bool MutexADQWrapper::SetLvlTrigEdge(int edge)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetLvlTrigEdge(edge);
}

bool MutexADQWrapper::SetLvlTrigLevel(int level)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetLvlTrigLevel(level);
}

bool MutexADQWrapper::SetLvlTrigChannel(int channel)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetLvlTrigChannel(channel);
}

bool MutexADQWrapper::SetPreTrigSamples(int pretrigger)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetPreTrigSamples(pretrigger);
}

bool MutexADQWrapper::SetTrigLevelResetValue(int reset)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetTrigLevelResetValue(reset);
}

bool MutexADQWrapper::SetClockSource(unsigned int clockSource)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetClockSource(clockSource);
}

bool MutexADQWrapper::SetTriggerMode(unsigned int triggerMode)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetTriggerMode(triggerMode);
}

bool MutexADQWrapper::SetSampleSkip(unsigned int sampleSkip)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetSampleSkip(sampleSkip);
}

bool MutexADQWrapper::BypassUserLogic(unsigned int ulTarget, unsigned int bypass)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::BypassUserLogic(ulTarget, bypass);
}

bool MutexADQWrapper::SetGainAndOffset(unsigned int channel, int gain, int offset)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetGainAndOffset(channel, gain, offset);
}

bool MutexADQWrapper::SetTransferBuffers(unsigned long count, unsigned long size)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetTransferBuffers(count, size);
}

bool MutexADQWrapper::SWTrig()
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SWTrig();
}

bool MutexADQWrapper::WriteUserRegister(unsigned int target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int *retval)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::WriteUserRegister(target, regnum, mask, data, retval);
}

bool MutexADQWrapper::ReadBlockUserRegister(int ulTarget, unsigned int startAddr, unsigned int *data, unsigned int numBytes, unsigned int options)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::ReadBlockUserRegister(ulTarget, startAddr, data, numBytes, options);
}

bool MutexADQWrapper::ReadUserRegister(unsigned int ul, unsigned int regnum, unsigned int *returval)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::ReadUserRegister(ul, regnum, returval);
}
bool MutexADQWrapper::FlushDMA()
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::FlushDMA();
}

bool MutexADQWrapper::StartStreaming()
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::StartStreaming();
}

bool MutexADQWrapper::StopStreaming()
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::StopStreaming();
}

bool MutexADQWrapper::TriggeredStreamingSetup(unsigned int recordCount, unsigned int recordLength, unsigned int pretrigger, unsigned int triggerDelay, unsigned int channelMask)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::TriggeredStreamingSetup(recordCount, recordLength, pretrigger, triggerDelay, channelMask);
}

bool MutexADQWrapper::ContinuousStreamingSetup(unsigned int channelMask)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::ContinuousStreamingSetup(channelMask);
}

bool MutexADQWrapper::GetStreamOverflow()
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::GetStreamOverflow();
}

bool MutexADQWrapper::GetTransferBufferStatus(unsigned int *buffersFilled)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::GetTransferBufferStatus(buffersFilled);
}

bool MutexADQWrapper::GetDataStreaming(void **targetBuffers, void **targetHeaders, unsigned char channelMask, unsigned int *samplesAdded, unsigned int *headersAdded, unsigned int *headerStatus)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::GetDataStreaming(targetBuffers, targetHeaders, channelMask, samplesAdded, headersAdded, headerStatus);
}

int MutexADQWrapper::StartDataAcquisition()
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::StartDataAcquisition();
    
}

int MutexADQWrapper::StopDataAcquisition()
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::StopDataAcquisition();
}

int64_t MutexADQWrapper::WaitForRecordBuffer(int *channel, void **buffer, int timeout, ADQDataReadoutStatus *status)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::WaitForRecordBuffer(channel, buffer, timeout, status);
    
}

int MutexADQWrapper::ReturnRecordBuffer(int channel, void *buffer)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::ReturnRecordBuffer(channel, buffer);
}

int MutexADQWrapper::GetParameters(ADQParameterId id, void * const parameters)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::GetParameters(id, parameters);
    
}

int MutexADQWrapper::SetParameters(void * const parameters)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetParameters(parameters);
}

int MutexADQWrapper::InitializeParameters(ADQParameterId id, void * const parameters)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::InitializeParameters(id, parameters);

}

int MutexADQWrapper::SetChannelSampleSkip(unsigned int channel, unsigned int skipfactor)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetChannelSampleSkip(channel, skipfactor);
}

unsigned int MutexADQWrapper::SetupLevelTrigger(int *level, int *edge, int *resetLevel, unsigned int channelMask, unsigned int individualMode)
{
    std::lock_guard<std::mutex> lck(this->mutex);
    return ADQInterfaceWrapper::SetupLevelTrigger(level, edge, resetLevel, channelMask, individualMode);

}

ADQInterfaceWrapper::ADQInterfaceWrapper(void *adqCU, unsigned int deviceNumber):
    deviceNumber(deviceNumber),
    adq(ADQControlUnit_GetADQ(adqCU, deviceNumber)),
    adqCU(adqCU)
{

}

bool ADQInterfaceWrapper::SetAdjustableBias(int channel, int code)
{
    return this->adq->SetAdjustableBias(channel, code);
}

bool ADQInterfaceWrapper::SetInputRange(int channel, float target, float *result)
{
    return this->adq->SetInputRange(channel, target, result);
}

bool ADQInterfaceWrapper::SetLvlTrigEdge(int edge)
{
    return this->adq->SetLvlTrigEdge(edge);
}

bool ADQInterfaceWrapper::SetLvlTrigLevel(int level)
{
    return this->adq->SetLvlTrigLevel(level);
}

bool ADQInterfaceWrapper::SetLvlTrigChannel(int channel)
{
    return this->adq->SetLvlTrigChannel(channel);
}

bool ADQInterfaceWrapper::SetPreTrigSamples(int pretrigger)
{
    return this->adq->SetPreTrigSamples(pretrigger);
}

bool ADQInterfaceWrapper::SetTrigLevelResetValue(int reset)
{
    return this->adq->SetTrigLevelResetValue(reset);
}

bool ADQInterfaceWrapper::SetClockSource(unsigned int clockSource)
{
    return this->adq->SetClockSource(clockSource);
}

bool ADQInterfaceWrapper::SetTriggerMode(unsigned int triggerMode)
{
    return this->adq->SetTriggerMode(triggerMode);
}

bool ADQInterfaceWrapper::SetSampleSkip(unsigned int sampleSkip)
{
    return this->adq->SetSampleSkip(sampleSkip);
}

bool ADQInterfaceWrapper::BypassUserLogic(unsigned int ulTarget, unsigned int bypass)
{
    return this->adq->BypassUserLogic(ulTarget, bypass);
}

bool ADQInterfaceWrapper::SetGainAndOffset(unsigned int channel, int gain, int offset)
{
    return this->adq->SetGainAndOffset(channel, gain, offset);
}

bool ADQInterfaceWrapper::SetTransferBuffers(unsigned long count, unsigned long size)
{
    return this->adq->SetTransferBuffers(count, size);
}

bool ADQInterfaceWrapper::SWTrig()
{
    return this->adq->SWTrig();
}

bool ADQInterfaceWrapper::WriteUserRegister(unsigned int target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int *retval)
{
    return this->adq->WriteUserRegister(target, regnum, mask, data, retval);
}

bool ADQInterfaceWrapper::ReadBlockUserRegister(int ulTarget, unsigned int startAddr, unsigned int *data, unsigned int numBytes, unsigned int options)
{
    return this->adq->ReadBlockUserRegister(ulTarget, startAddr, data, numBytes, options);
}
bool ADQInterfaceWrapper::ReadUserRegister(unsigned int ul, unsigned int regnum, unsigned int *returval)
{
    return this->adq->ReadUserRegister(ul, regnum, returval);
}
bool ADQInterfaceWrapper::FlushDMA()
{
    return this->adq->FlushDMA();
}

bool ADQInterfaceWrapper::StartStreaming()
{
    return this->adq->StartStreaming();
}

bool ADQInterfaceWrapper::StopStreaming()
{
    return this->adq->StopStreaming();
}

bool ADQInterfaceWrapper::TriggeredStreamingSetup(unsigned int recordCount, unsigned int recordLength, unsigned int pretrigger, unsigned int triggerDelay, unsigned int channelMask)
{
    return this->adq->TriggeredStreamingSetup(recordCount, recordLength, pretrigger, triggerDelay, channelMask);
}

bool ADQInterfaceWrapper::ContinuousStreamingSetup(unsigned int channelMask)
{
    return this->adq->ContinuousStreamingSetup(channelMask);
}

bool ADQInterfaceWrapper::GetStreamOverflow()
{
    return this->adq->GetStreamOverflow();
}

bool ADQInterfaceWrapper::GetTransferBufferStatus(unsigned int *buffersFilled)
{
    return this->adq->GetTransferBufferStatus(buffersFilled);
}

bool ADQInterfaceWrapper::GetDataStreaming(void **targetBuffers, void **targetHeaders, unsigned char channelMask, unsigned int *samplesAdded, unsigned int *headersAdded, unsigned int *headerStatus)
{
    return this->adq->GetDataStreaming(targetBuffers, targetHeaders, channelMask, samplesAdded, headersAdded, headerStatus);
}

int ADQInterfaceWrapper::StartDataAcquisition()
{
    return this->adq->StartDataAcquisition();
}

int ADQInterfaceWrapper::StopDataAcquisition()
{
    return this->adq->StopDataAcquisition();
}

int64_t ADQInterfaceWrapper::WaitForRecordBuffer(int *channel, void **buffer, int timeout, ADQDataReadoutStatus *status)
{
    return this->adq->WaitForRecordBuffer(channel, buffer, timeout, status);

}

int ADQInterfaceWrapper::ReturnRecordBuffer(int channel, void *buffer)
{
    return this->adq->ReturnRecordBuffer(channel, buffer);

}

int ADQInterfaceWrapper::GetParameters(ADQParameterId id, void * const parameters)
{
    return this->adq->GetParameters(id, parameters);

}

int ADQInterfaceWrapper::SetParameters(void * const parameters)
{
    return this->adq->SetParameters(parameters);

}

int ADQInterfaceWrapper::InitializeParameters(ADQParameterId id, void * const parameters)
{
    return this->adq->InitializeParameters(id, parameters);
}

ADQInterfaceWrapper::~ADQInterfaceWrapper()
{
    ADQControlUnit_DeleteADQ(this->adqCU, this->deviceNumber);
}

int ADQInterfaceWrapper::SetChannelSampleSkip(unsigned int channel, unsigned int skipfactor)
{
    return this->adq->SetChannelSampleSkip(channel, skipfactor);
}

unsigned int ADQInterfaceWrapper::SetupLevelTrigger(int *level, int *edge, int *resetLevel, unsigned int channelMask, unsigned int individualMode)
{
    return this->adq->SetupLevelTrigger(level, edge, resetLevel, channelMask, individualMode);
}
