#include "BufferProcessorThread.h"

const float RAM_FILL_LEVELS[]  = {0, 1.0/8, 2.0/8, 3.0/8, 4.0/8, 5.0/8, 6.0/8, 7.0/8};

BufferProcessor::BufferProcessor(std::list<IRecordProcessor *> &recordProcessors, ADQInterfaceWrapper &adq, QObject *parent):
    QObject(parent), recordProcessors(recordProcessors), adq(adq)
{
    qRegisterMetaType<BufferProcessor::STATE>("BufferProcessor::STATE");
}

BufferProcessor::~BufferProcessor()
{

}

void BufferProcessor::stop()
{
    if(this->loopState == BufferProcessor::STATE::ACTIVE)
    {
        this->changeState(BufferProcessor::STATE::STOPPING);
    }
}
void BufferProcessor::changeState(BufferProcessor::STATE newState)
{
    this->loopState = newState;
    emit this->stateChanged(newState);
}
void BufferProcessor::startBufferProcessLoop()
{
    if(this->loopState != BufferProcessor::STATE::INACTIVE)
    {
        spdlog::critical("Buffer Processor loop already active.");
        return;
    }
    else if(this->loopState == BufferProcessor::STATE::ERROR)
    {
        spdlog::critical("Buffer Processor entered error state and must be reset before starting.");
        return;
    }
    this->changeState(BufferProcessor::STATE::ACTIVE);
    while(this->loopState == BufferProcessor::STATE::ACTIVE)
    {
        long long bufferPayloadSize;
        int channel = ADQ_ANY_CHANNEL; // must pass the channel to get its buffers, use ADQ_ANY_CHANNEL to capture all channels;
                                       // is passed as a pointer, so the actual channel is returned
        ADQDataReadoutStatus status;
        ADQRecord * record;
        bufferPayloadSize = this->adq.WaitForRecordBuffer(&channel, reinterpret_cast<void**>(&record), 0, &status);
        if(bufferPayloadSize < 0)
        {
            if(!this->handleWaitForRecordErrors(bufferPayloadSize) && this->loopState != BufferProcessor::STATE::STOPPING)
            {
                this->changeState(BufferProcessor::STATE::ERROR);
                continue;
            }
        }
        else if (bufferPayloadSize == 0)
        {
            if(status.flags & ADQ_DATA_READOUT_STATUS_FLAGS_STARVING)
            {
                spdlog::warn("Thread is starved. Some data will be discarded.");
            }
            if(status.flags & ADQ_DATA_READOUT_STATUS_FLAGS_INCOMPLETE)
            {
                spdlog::warn("Record is incomplete and empty??");
            }
            continue;
        }
        else
        {
            //spdlog::debug("Got {} buffer", bufferPayloadSize);
            if(!this->completeRecord(record, bufferPayloadSize))
            {
                enterErrorCondition();
            }
            this->adq.ReturnRecordBuffer(channel, record);
        }
    }
    if(this->loopState == BufferProcessor::STATE::STOPPING)
    {
        spdlog::debug("Buffer Processor stopped gracefully.");
        this->changeState( BufferProcessor::STATE::INACTIVE );
    }
    else
    {
        spdlog::warn("Buffer processor exited in state {}", this->loopState);
    }
}

BufferProcessor::STATE BufferProcessor::getLoopState() const
{
    return this->loopState;
}

bool BufferProcessor::completeRecord(ADQRecord *record, size_t bufferSize)
{
    int ramFill = (record->header->RecordStatus & 0b01110000) >> 4;
    if(ramFill != lastRAMFillLevel)
    {
        lastRAMFillLevel = ramFill;
        emit this->ramFillChanged(this->getRamFillLevel());
    }
    if(this->recordLength != 0 && record->header->RecordLength != this->recordLength)
    {
        spdlog::warn("Obtained record with bad length. this={} header={} buffer={}", this->recordLength, record->header->RecordLength, bufferSize/sizeof(short));
        return true;
        //record->header->RecordLength = this->recordLength;
    }
    for(auto rp : this->recordProcessors)
    {
        if(rp->processRecord(record, bufferSize)) return false;
    }
    return true;
}
void BufferProcessor::enterErrorCondition()
{
    this->loopState = BufferProcessor::STATE::ERROR;
}
void BufferProcessor::reset()
{
    this->loopState = BufferProcessor::STATE::INACTIVE;
}
float BufferProcessor::getRamFillLevel()
{
    return RAM_FILL_LEVELS[this->lastRAMFillLevel];
}



bool BufferProcessor::handleWaitForRecordErrors(long long returnValue)
{
    switch(returnValue)
    {
        case ADQ_EINVAL:
            spdlog::error("Invalid arguments pased to WaitForRecordBuffer");
            return false;
        break;
        case ADQ_EAGAIN:
            //spdlog::info("WaitForRecordBuffer timed out.");
        break;
        case ADQ_ENOTREADY:
            spdlog::error("WaitForRecordBuffer called despite no acquisition running.");
            return false;
        break;
        case ADQ_EINTERRUPTED:
            spdlog::error("WaitForRecordBuffer interrupted.");
        break;
        case ADQ_EEXTERNAL:
            spdlog::error("External error when calling WaitForRecordBuffer.");
            return false;
        break;
    }
    return true;
}

void BufferProcessor::configureNewAcquisition(Acquisition &acq)
{
    if(acq.getIsContinuous()) this->recordLength = 0;
    else this->recordLength = acq.getRecordLength();
    this->isContinuous = acq.getIsContinuous();
}
