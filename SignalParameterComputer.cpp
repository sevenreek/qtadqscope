#include "SignalParameterComputer.h"


SignalParameterComputer::SignalParameterComputer(unsigned long long sizeLimit)
{
    this->sizeLimit = sizeLimit;
    this->bytesSaved = 0;
    this->samplesSaved = 0;
    this->dataBuffer = (short*)std::malloc(sizeof(short)*this->sizeLimit);
}
SignalParameterComputer::~SignalParameterComputer()
{
    std::free(this->dataBuffer);
}
void SignalParameterComputer::startNewStream(ApplicationConfiguration& config)
{
    this->finished = false;
    this->samplesSaved = 0;
    this->bytesSaved = 0;
    if(this->sizeLimit != config.fileSizeLimit)
    {
        this->sizeLimit = config.fileSizeLimit;
        std::free(this->dataBuffer);
        this->dataBuffer = (short*)std::malloc(sizeof(short)*this->sizeLimit);
    }
}
bool SignalParameterComputer::writeRecord(StreamingHeader_t* header, short* buffer, unsigned int length)
{
    // record length is stored in the cfg file
    // an alternative is to prefix every buffer with the header
    // a header has a constant size of 40 bytes
    // so it would be possible to read how many samples to read
    // from the header
    if(this->bytesSaved > this->sizeLimit)
    {
        return false;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        this->samplesSaved += length;
        std::memcpy(&this->dataBuffer[this->samplesSaved], buffer, length*sizeof(short));
        return true;
    }
}
bool SignalParameterComputer::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length)
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
bool SignalParameterComputer::writeContinuousBuffer(short* buffer, unsigned int length)
{
    if(this->bytesSaved > this->sizeLimit)
    {
        return false;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        this->samplesSaved += length;
        std::memcpy(&this->dataBuffer[this->samplesSaved], buffer, length*sizeof(short));
        return true;
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
    for(unsigned long i = 0; i < samplesSaved; i++)
    {
        result->average += dataBuffer[i];
        result->rms += dataBuffer[i]*dataBuffer[i];
    }
    result->average /= samplesSaved;
    result->rms /= samplesSaved;
    result->rms = std::sqrt(result->rms);

    return result;
}
const char* SignalParameterComputer::getName()
{
    return "SignalParameterComputer";
}
