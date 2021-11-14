#include "SignalParameterComputer.h"
#include "spdlog/spdlog.h"
const double MAX_SAMPLE = 1<<15;
SignalParameterComputer::SignalParameterComputer(unsigned long long sizeLimit)
{
    this->sizeLimit = sizeLimit;
    this->bytesSaved = 0;
    this->samplesSaved = 0;
}
SignalParameterComputer::~SignalParameterComputer()
{

}
bool SignalParameterComputer::startNewAcquisition(Acquisition &config)
{
    this->finished = false;
    this->samplesSaved = 0;
    this->bytesSaved = 0;
    this->avg = 0;
    this->rms = 0;
    return true;
}

IRecordProcessor::STATUS SignalParameterComputer::processRecord(ADQRecord* record, size_t bufferSize)
{
    size_t length = bufferSize/sizeof(short);
    for(size_t i = 0; i < length; i++)
    {
        this->avg += static_cast<double>(reinterpret_cast<short*>(record->data)[i]);
        this->rms += avg*avg;
    }
    this->samplesSaved += bufferSize;
    return IRecordProcessor::STATUS::OK;
}

unsigned long long SignalParameterComputer::finish(){
    this->finished = true;
    this->bytesSaved = this->samplesSaved*sizeof(short);
    return this->bytesSaved;
}
std::unique_ptr<SignalParameters> SignalParameterComputer::getResults()
{
    if(!finished) return nullptr;
    std::unique_ptr<SignalParameters> result = std::unique_ptr<SignalParameters>(new SignalParameters());
    result->average = 0;
    result->rms = 0;
    result->average = this->avg / this->samplesSaved;
    result->rms = this->rms / this->samplesSaved;
    result->rms = std::sqrt(result->rms);
    return result;
}
const char* SignalParameterComputer::getName()
{
    return "SignalParameterComputer";
}
unsigned long long SignalParameterComputer::getProcessedBytes()
{
    return this->bytesSaved;
}
