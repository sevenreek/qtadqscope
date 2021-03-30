#include "BinaryFileWriter.h"
#include "spdlog/spdlog.h"
#include <ctime>
BinaryFileWriter::BinaryFileWriter()
{
    this->sizeLimit = sizeLimit;
    this->bytesSaved = 0;
}
BinaryFileWriter::~BinaryFileWriter()
{

}
void BinaryFileWriter::startNewStream(ApplicationConfiguration& config)
{
    time_t rawtime;
    struct tm * timeinfo;
    char bufferdata[80];
    char buffercfg[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(bufferdata,sizeof(bufferdata),"%Y-%m-%d_%H%M%S_data.dat",timeinfo);
    strftime(buffercfg,sizeof(buffercfg),"%Y-%m-%d_%H%M%S_config.json",timeinfo);
    this->dataStream.open(bufferdata, std::ios_base::binary | std::ios_base::out);
    config.toFile(buffercfg);


}
bool BinaryFileWriter::writeRecord(StreamingHeader_t* header, short* buffer, unsigned int length)
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
        this->dataStream.write((char*)buffer, sizeof(short)*length);
        return true;
    }
}
bool BinaryFileWriter::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length)
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
bool BinaryFileWriter::writeContinuousBuffer(short* buffer, unsigned int length)
{
    if(this->bytesSaved > this->sizeLimit)
    {
        return false;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        this->dataStream.write((char*)buffer, sizeof(short)*length);
        return true;
    }

}
unsigned long long BinaryFileWriter::finish(){
    this->dataStream.close();
    return this->bytesSaved;
}
