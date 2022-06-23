#include "Digitizer.h"
#include "util.h"
#include "RegisterConstants.h"

void Digitizer::SWTrig()
{
    this->adq->SWTrig();
}


void Digitizer::finishRecordProcessors()
{
    for(auto rp : this->recordProcessors)
    {
        auto processedBytes = rp->finish();
        double dataRate = (processedBytes/1.0e6) / (this->defaultAcquisition.getDuration()/1.0e3);
        if(processedBytes)
            spdlog::info("{} processed {:.4f} GBs. Data rate {:.4f} MB/s.", rp->getName(), processedBytes/1.0e9, dataRate);
    }
}

void Digitizer::handleAcquisitionFullyStopped()
{
    if(this->currentState == DIGITIZER_STATE::READY) return;
    spdlog::info("Completing acquisition.");
    this->finishRecordProcessors();
    this->bufferProcessorHandler->reset();
    if(this->adq.GetStreamOverflow())
    {
        spdlog::warn("Overflow occured during acquisition! Some data might be missing.");
    }
    this->changeDigitizerState(DIGITIZER_STATE::READY);

}

Digitizer::Digitizer(ADQInterfaceWrapper &digitizerWrapper) :
    defaultRecordProcessors(),
    recordProcessors(defaultRecordProcessors),
    adq(digitizerWrapper),
    defaultAcquisition(),
    bufferProcessorHandler(new BufferProcessor(this->recordProcessors, this->adq)),
    ADQThread()
{
    //qRegisterMetaType<BufferProcessor::STATE>();
    this->bufferProcessorHandler->moveToThread(&this->ADQThread);
    connect(
        this,                           &Digitizer::acquisitionStarted,
        bufferProcessorHandler.get(),   &BufferProcessor::startBufferProcessLoop,
        Qt::ConnectionType::QueuedConnection
    );
    connect(bufferProcessorHandler.get(), &BufferProcessor::stateChanged, this, &Digitizer::processorLoopStateChanged);
    connect(bufferProcessorHandler.get(), &BufferProcessor::ramFillChanged, this, &Digitizer::ramFillChanged);
    this->ADQThread.start();
    this->acquisitionTimer.setSingleShot(true);
    connect(
        &this->acquisitionTimer,    &QTimer::timeout,
        this,                       &Digitizer::stopAcquisition,
        Qt::ConnectionType::QueuedConnection
    );
    this->acquisitionStartDelayTimer.setInterval(Digitizer::ACQUISITION_DELAY);
    this->acquisitionStartDelayTimer.setSingleShot(true);
    this->acquisitionStartDelayTimer.connect(
        &this->acquisitionStartDelayTimer,  &QTimer::timeout,
        this,                               &Digitizer::onAcquisitionDelayEnd
    );
}

Digitizer::~Digitizer()
{
    this->joinThreads();
}

std::chrono::milliseconds Digitizer::getMillisFromLastStarve()
{
    return this->bufferProcessorHandler->getMillisFromLastStarve();
}

float Digitizer::getDeviceRamFillLevel()
{
    return this->bufferProcessorHandler->getRamFillLevel();
}

bool Digitizer::stopAcquisition()
{
    int result;
    this->acquisitionStartDelayTimer.stop();
    this->acquisitionTimer.stop();
    this->bufferProcessorHandler->stop();
    result = this->adq.StopDataAcquisition();
    if(result == ADQ_EAGAIN)
    {
        spdlog::warn("Tried to stop acquisition. Acqusition is not running.");
    }
    else if(result == ADQ_EINTERRUPTED)
    {
        spdlog::warn("Ended acquisition prematurely.");
    }
    else if(result == ADQ_EIO)
    {
        spdlog::error("I/O error while stopping acquisition.");
    }
    if(this->isStreamFullyStopped())
    {
        this->handleAcquisitionFullyStopped();
    }
    else
    {
        this->changeDigitizerState(DIGITIZER_STATE::STOPPING);
    }
    unsigned int retval;
    this->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~ACTIVE_SPECTRUM_BIT, 0, &retval); // disable pha
    return true;
}

bool Digitizer::runAcquisition()
{
    return this->runOverridenAcquisition(
        &this->defaultAcquisition,
        this->defaultRecordProcessors,
        this->defaultCalibrationTable
    );
}
void Digitizer::processorLoopStateChanged(BufferProcessor::STATE newState)
{
    if(newState == BufferProcessor::STATE::SERROR)
    {
        this->stopAcquisition();
    }
    else if(newState == BufferProcessor::STATE::INACTIVE)
    {
        this->handleAcquisitionFullyStopped();
    }
}
void Digitizer::onAcquisitionDelayEnd()
{
    int result;
    result = this->adq.StartDataAcquisition();
    if(result == ADQ_ENOTREADY)
    {
     spdlog::error("Acqusition already active. Cannot start.");
     goto START_FAILED;
    }
    else if(result == ADQ_EINTERRUPTED)
    {
     spdlog::error("Could not start acqusisition thread.");
     goto START_FAILED;
    }
    else if(result != ADQ_EOK)
    {
     spdlog::critical("Could not start acquisition. Reason unknown.");
     goto START_FAILED;
    }
    if(this->acquisitionToStart->getIsContinuous()) this->adq.SWTrig();
    emit this->acquisitionStarted();
    if(this->acquisitionToStart->getDuration())
    {
     this->acquisitionTimer.setInterval(this->acquisitionToStart->getDuration());
     this->acquisitionTimer.start();
    }
    unsigned int retval;

    this->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~ACTIVE_SPECTRUM_BIT, this->acquisitionToStart->getSpectroscopeEnabled()?ACTIVE_SPECTRUM_BIT:0, &retval);
    spdlog::info("Acquisition started!");
    this->changeDigitizerState(DIGITIZER_STATE::ACTIVE);
    return;
START_FAILED:
    this->stopAcquisition();
    spdlog::error("Stream failed to start!");
    this->acquisitionTimer.stop();

}