#include "SignalParameterComputer.h"
#include "spdlog/spdlog.h"

SignalParameterComputer::SignalParameterComputer(unsigned long long sizeLimit)
{
    this->sizeLimit = sizeLimit;
    this->bytesSaved = 0;
    this->samplesSaved = 0;
    this->dataBuffer = nullptr;
}
SignalParameterComputer::~SignalParameterComputer()
{
    if(this->dataBuffer != nullptr)
        std::free(this->dataBuffer);
}
bool SignalParameterComputer::startNewAcquisition(Acquisition &config)
{
    this->finished = false;
    this->samplesSaved = 0;
    this->bytesSaved = 0;
    if(this->sizeLimit != config.getFileSizeLimit())
    {
        this->sizeLimit = (size_t)config.getFileSizeLimit();
        if(this->dataBuffer != nullptr)
        {
            std::free(this->dataBuffer);
            this->dataBuffer = nullptr;
        }
    }
    if(this->dataBuffer == nullptr)
    {
        short * newptr = (short*)std::malloc(this->sizeLimit);
        this->dataBuffer = newptr;
    }
    if(this->dataBuffer == nullptr) {
        spdlog::critical("Failed to allocate space for SignalParameterComputer. Cannot start calibration!");
        return false;
    }
    return true;
}
IRecordProcessor::STATUS SignalParameterComputer::writeRecord(ADQRecordHeader* header, short* buffer, unsigned int length)
{
    // record length is stored in the cfg file
    // an alternative is to prefix every buffer with the header
    // a header has a constant size of 40 bytes
    // so it would be possible to read how many samples to read
    // from the header
    if(this->bytesSaved > this->sizeLimit)
    {
        return STATUS::LIMIT_REACHED;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        this->samplesSaved += length;
        std::memcpy(&this->dataBuffer[this->samplesSaved], buffer, length*sizeof(short));
        return STATUS::OK;
    }
}
IRecordProcessor::STATUS SignalParameterComputer::processRecord(ADQRecordHeader* header, short* buffer, unsigned long length, int channel)
{
    if(header == NULL) // if that is the case it is a continuous (no trigger and headers) stream
    {
        return this->writeContinuousBuffer(buffer, length);
    }
    else
    {
        return this->writeRecord(header, buffer, length);
    }
}
IRecordProcessor::STATUS SignalParameterComputer::writeContinuousBuffer(short* buffer, unsigned int length)
{
    if(this->sizeLimit/sizeof(short) < this->samplesSaved+length)
    {
        return STATUS::LIMIT_REACHED;
    }
    else
    {
        std::memcpy(&this->dataBuffer[this->samplesSaved], buffer, length*sizeof(short));
        this->bytesSaved += length*sizeof(short);
        this->samplesSaved += length;
        return STATUS::OK;
    }

}
unsigned long long SignalParameterComputer::finish(){
    this->finished = true;
    return this->bytesSaved;
}
std::unique_ptr<SignalParameters> SignalParameterComputer::getResults()
{
    if(!finished) return nullptr;
    std::unique_ptr<SignalParameters> result = std::unique_ptr<SignalParameters>(new SignalParameters());
    result->average = 0;
    result->rms = 0;
    if(samplesSaved == 0)
    {
        result->average = 0;
        result->rms = 0;
        spdlog::warn("SignalParameterComputer did not collect any samples. Returning average of 0.");
        return result;
    }
    for(unsigned long i = 0; i < samplesSaved; i++)
    {
        result->average += dataBuffer[i];
        result->rms += dataBuffer[i]*dataBuffer[i];
    }
    result->average /= samplesSaved;
    result->rms /= samplesSaved;
    result->rms = std::sqrt(result->rms);
    if(this->dataBuffer != nullptr)
    {
        std::free(this->dataBuffer);
        this->dataBuffer = nullptr;
    }
    return result;
}
const char* SignalParameterComputer::getName()
{
    return "SignalParameterComputer";
}
