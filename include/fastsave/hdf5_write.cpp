#include "hdf5_write.hpp"
#include "spdlog/spdlog.h"

bool NullRecordProcessor::processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount)
{
    spdlog::info("Nullprocessor: {} {} {}", header->RecordNumber, header->Timestamp, header->RecordLength);
    return true;
}
unsigned long long NullRecordProcessor::finish(){
    return 0;
}
HDF5Writer::HDF5Writer(char* filename)
{
}

bool HDF5Writer::writeRecord(StreamingHeader_t* header, short* buffer, unsigned int length)
{
    return false;
}
bool HDF5Writer::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length)
{
    return false;
}
unsigned long long HDF5Writer::finish(){
    return 0;
}
BinaryWriter::BinaryWriter(const char* cFilename, const char* tFilename, unsigned long long sizeLimit) : 
    continuousFile(cFilename, std::ios::out | std::ios::binary),
    triggeredFile(tFilename, std::ios::out | std::ios::binary)
{
    this->sizeLimit = sizeLimit;
    this->bytesSaved = 0;
}
BinaryWriter::~BinaryWriter()
{

}
bool BinaryWriter::writeRecord(StreamingHeader_t* header, short* buffer, unsigned int length)
{ 
    spdlog::critical("BinaryWriter does not implement writeRecord");
    return false;
}
bool BinaryWriter::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length)
{
    if(header == NULL) // continuous stream
    {
        return this->writeContinuousBuffer(buffer, length); 
    }
    else
    {
        return this->writeRecord(header, buffer, length);
    }
}
bool BinaryWriter::writeContinuousBuffer(short* buffer, unsigned int length)
{
    if(this->bytesSaved > this->sizeLimit)
    {
        return false;
    }
    else
    {
        this->bytesSaved += length*sizeof(short);
        this->continuousFile.write((char*)buffer, sizeof(short)*length);
        return true;
    }
    
}
unsigned long long BinaryWriter::finish(){
    this->continuousFile.close();
    this->triggeredFile.close();
    return this->bytesSaved;
}



RamBufferBinaryWriter::RamBufferBinaryWriter(const char* cFilename, const char* tFilename, unsigned long long sizeLimit) : 
    continuousFile(cFilename, std::ios::out | std::ios::binary),
    triggeredFile(tFilename, std::ios::out | std::ios::binary)
{
    this->sizeLimit = sizeLimit;
    this->sampleLimit = sizeLimit/sizeof(short);
    this->storage = (short*)malloc(this->sampleLimit*sizeof(short));
    this->samplesSaved = 0;
}
RamBufferBinaryWriter::~RamBufferBinaryWriter()
{
    this->continuousFile.close();
    this->triggeredFile.close();
    free(this->storage);
}

bool RamBufferBinaryWriter::processRecord(StreamingHeader_t* header, short* buffer, unsigned long length)
{
    if(this->samplesSaved+length > this->sampleLimit)
    {
        spdlog::error("Size limit reached");
        return false;
    }
    std::memcpy(&(this->storage[this->samplesSaved]), buffer, length*sizeof(short));
    this->samplesSaved += length;
    return true;
}
unsigned long long RamBufferBinaryWriter::finish(){
    this->continuousFile.write((char*)this->storage, this->samplesSaved*sizeof(short));
    this->continuousFile.close();
    return this->samplesSaved*sizeof(short);
}
