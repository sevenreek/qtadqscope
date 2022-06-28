#include "QAcquisitionHandler.h"
#include "AcquisitionConfiguration.h"
#include "DigitizerConstants.h"
#include "RecordProcessor.h"
#include "qnamespace.h"
#include "qobject.h"
#include <memory>
#include <stdlib.h>

QAcquisitionHandlerGen3::QAcquisitionHandlerGen3(QObject* parent) : QObject(parent)
{
    this->processingThread.start();
    this->acquisitionTimer.setSingleShot(true);
    connect(&this->acquisitionTimer, &QTimer::timeout, this,
            &QAcquisitionHandlerGen3::stopAcquisition, Qt::ConnectionType::QueuedConnection);
    this->acquisitionStabilizationTimer.setSingleShot(true);
    this->acquisitionStabilizationTimer.connect(
        &this->acquisitionStabilizationTimer, &QTimer::timeout, this,
        &QAcquisitionHandlerGen3::requestProcessorProxyStart, Qt::ConnectionType::QueuedConnection);
}
QAcquisitionHandlerGen3::~QAcquisitionHandlerGen3()
{
    this->requestStop();
    this->processingThread.quit();
    this->processingThread.requestInterruption();
    this->processingThread.wait();
}
bool QAcquisitionHandlerGen3::requestStart(AcquisitionConfiguration* config,
                                           std::vector<IRecordProcessor*>* recordProcessors)
{
    this->config = config;
    this->recordProcessors = recordProcessors;
    // configure ADQ
    bool configurationResult = this->configure(config);
    if (!configurationResult)
        return false;

    // disconnect signals from last used proxy
    if(this->processorProxy)
    {
        this->processorProxy->disconnect();
        this->disconnect(processorProxy.get());
    }

    // recreate the proxy and connect signals
    this->bufferProcessor = std::unique_ptr<BufferProcessorGen3>(
        new BufferProcessorGen3(*this->adq, *this->config, *this->recordProcessors,
                                [=](AcquisitionStates o, AcquisitionStates n) {
                                    emit this->processorProxy->stateChanged(o, n);
                                }));
    this->processorProxy =
        std::unique_ptr<QBufferProcessorProxy>(new QBufferProcessorProxy(*this->bufferProcessor));
    this->processorProxy->moveToThread(&this->processingThread);
    this->connect(this, &QAcquisitionHandlerGen3::requestProcessorProxyStart,
                  this->processorProxy.get(), &QBufferProcessorProxy::startLoop,
                  Qt::QueuedConnection);
    this->connect(this, &QAcquisitionHandlerGen3::requestProcessorProxyStop,
                  this->processorProxy.get(), &QBufferProcessorProxy::stop, Qt::QueuedConnection);
    this->connect(this->processorProxy.get(), &QBufferProcessorProxy::stateChanged, this,
                  &QAcquisitionHandlerGen3::onProcessorStateChanged, Qt::QueuedConnection);
    this->delayAcquisitionStart();
    return true;
}
void QAcquisitionHandlerGen3::stopAcquisition()
{
    this->requestStop();
}
bool QAcquisitionHandlerGen3::requestStop()
{
    emit this->requestProcessorProxyStop();
    return true;
}

float QAcquisitionHandlerGen3::ramFill()
{
    return this->bufferProcessor ? this->bufferProcessor->ramUsage() : 0;
}

float QAcquisitionHandlerGen3::dmaUsage()
{
    return this->bufferProcessor ? this->bufferProcessor->dmaUsage() : 0;
}

void QAcquisitionHandlerGen3::delayAcquisitionStart()
{
    // time is set in this->configure()
    this->acquisitionTimer.start();
}

void QAcquisitionHandlerGen3::onProcessorStateChanged(AcquisitionStates olds, AcquisitionStates news)
{
    switch(news)
    {
        case AcquisitionStates::INACTIVE: {
            this->processorStopped();
        } break;
        default: break;
    }
    emit this->stateChanged(olds, news);
}

void QAcquisitionHandlerGen3::processorStopped()
{
    this->finishRecordProcessors();
}

void QAcquisitionHandlerGen3::finishRecordProcessors()
{
    for (auto rp : *this->recordProcessors)
    {
        auto processedBytes = rp->finish();
        double dataRate =
            (processedBytes / 1.0e6) / (this->config->collection.duration() / 1.0e3);
        if (processedBytes)
            spdlog::info("{} processed {:.4f} GBs. Data rate {:.4f} MB/s.", rp->getName(),
                         processedBytes / 1.0e9, dataRate);
    }
}
bool QAcquisitionHandlerGen3::configure(AcquisitionConfiguration* acq)
{
    int result;
    this->adq->StopDataAcquisition();

    // if(!this->adq.SetClockSource(acq.getClockSource()))
    // {spdlog::error("SetClockSource failed."); return false;};

    for (int i = 0; i < 2; i++)
    {
        if (acq->spectroscope.userLogicBypassMask() & (1 << i))
        {
            if (!this->adq->BypassUserLogic(i + 1, 1))
            {
                spdlog::error("BypassUserLogic failed");
                return false;
            };
        }
        else
        {
            if (!this->adq->BypassUserLogic(i + 1, 0))
            {
                spdlog::error("BypassUserLogic failed.");
                return false;
            };
        }
    }
    float res;
    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        if (acq->collection.channelMask() & (1 << ch))
        {
            int adqch = ch + 1;
            if (!this->adq->SetInputRange(adqch, acq->AFEs.at(ch).desiredInputRange(), &res))
            {
                spdlog::error("SetInputRange failed.");
                return false;
            };
            acq->AFEs.at(ch).setObtainedInputRange(res);
            int unclippedDCShift;
            int clippedDCShift = acq->getTotalDCShift(ch, unclippedDCShift);
            if (clippedDCShift != unclippedDCShift)
                spdlog::warn("DC shift was clipped.");
            if (!this->adq->SetAdjustableBias(adqch, clippedDCShift))
            {
                spdlog::error("SetAdjustableBias failed.");
                return false;
            };
            if (!this->adq->SetGainAndOffset(adqch, acq->calibrations.at(ch).digitalGain(),
                                             acq->calibrations.at(ch).digitalOffset()))
            {
                spdlog::error("SetGainAndOffset failed.");
                return false;
            };
        }
        // TODO: REPLACE WITH A PER CHANNEL CONFIG
        if (!this->adq->SetChannelSampleSkip(ch + 1, acq->collection.sampleSkip()))
        {
            spdlog::error("SetChannelSampleSkip failed.");
            return false;
        };
    }
    // still GEN 3:
    struct ADQDataAcquisitionParameters acqParams;
    if (this->adq->InitializeParameters(ADQParameterId::ADQ_PARAMETER_ID_DATA_ACQUISITION,
                                        &acqParams) != sizeof(acqParams)) // this is an error check
    {
        spdlog::error("InitializeAcquisitionParameters failed.");
        return false;
    };

    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        acqParams.channel[ch].trigger_source =
            static_cast<ADQEventSource>(acq->triggers.at(ch).mode());
        acqParams.channel[ch].horizontal_offset = acq->triggers.at(ch).horizontalShift();
        acqParams.channel[ch].trigger_edge = static_cast<ADQEdge>(acq->triggers.at(ch).edge());

        if (acq->collection.isContinuous())
        {
            if (acq->collection.channelMask() & (1 << ch))
            {
                acqParams.channel[ch].nof_records = ADQ_INFINITE_NOF_RECORDS;
            }
            else
            {
                acqParams.channel[ch].nof_records = 0; // disables channel
            }
            acqParams.channel[ch].record_length = ADQ_INFINITE_RECORD_LENGTH;
        }
        else
        {
            if (acq->collection.channelMask() & (1 << ch))
            {
                acqParams.channel[ch].nof_records =
                    acq->records.at(ch).recordCount() == INFINITE_RECORDS
                        ? ADQ_INFINITE_NOF_RECORDS
                        : acq->records.at(ch).recordCount();
            }
            else
            {
                acqParams.channel[ch].nof_records = 0; // disables channel
            }
            acqParams.channel[ch].record_length = acq->records.at(ch).recordLength();
        }
        this->trigSetupHelperConfig.levelArray[ch] = static_cast<int>(acq->triggers.at(ch).level());
        this->trigSetupHelperConfig.resetArray[ch] = static_cast<int>(acq->triggers.at(ch).reset());
        this->trigSetupHelperConfig.edgeArray[ch] = static_cast<int>(acq->triggers.at(ch).edge());
    }
    result = this->adq->SetParameters(&acqParams);
    if (result == ADQ_EINVAL)
    {
        spdlog::error("Invalid ADQDataAcquisitionParameters paramaters. Could not "
                      "configure acquisition.");
        return false;
    }
    else if (result == ADQ_EIO)
    {
        spdlog::error("Could not configure ADQDataAcquisitionParameters parameters "
                      "due to an I/O error.");
        return false;
    }

    struct ADQDataTransferParameters dtParams;
    if (this->adq->InitializeParameters(ADQParameterId::ADQ_PARAMETER_ID_DATA_TRANSFER,
                                        &dtParams) != sizeof(dtParams))
    {
        spdlog::error("InitializeDataTransferParameters failed.");
        return false;
    };
    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        dtParams.channel[ch].nof_buffers = acq->transfer.bufferCount();
        dtParams.channel[ch].record_buffer_size = acq->transfer.bufferSize();
    }
    result = this->adq->SetParameters(&dtParams);
    if (result == ADQ_EINVAL)
    {
        spdlog::error("Invalid ADQDataTransferParameters paramaters. Could not "
                      "configure acquisition.");
        return false;
    }
    else if (result == ADQ_EIO)
    {
        spdlog::error("Could not configure ADQDataTransferParameters parameters "
                      "due to an I/O error.");
        return false;
    }

    struct ADQDataReadoutParameters roParams;
    if (this->adq->InitializeParameters(ADQParameterId::ADQ_PARAMETER_ID_DATA_READOUT, &roParams) !=
        sizeof(roParams))
    {
        spdlog::error("InitializeDataReadoutParameters failed.");
        return false;
    };
    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        roParams.channel[ch].nof_record_buffers_max = acq->transfer.queueSize();
        roParams.channel[ch].incomplete_records_enabled =
            (acq->collection.isContinuous()); // for continuous streaming there is
                                              // just one header that never ends
    }
    result = this->adq->SetParameters(&roParams);
    if (result == ADQ_EINVAL)
    {
        spdlog::error("Invalid ADQDataReadoutParameters paramaters. Could not "
                      "configure acquisition.");
        return false;
    }
    else if (result == ADQ_EIO)
    {
        spdlog::error("Could not configure ADQDataReadoutParameters parameters due "
                      "to an I/O error.");
        return false;
    }
    if (!this->adq->SetupLevelTrigger(trigSetupHelperConfig.levelArray,
                                      trigSetupHelperConfig.edgeArray,
                                      trigSetupHelperConfig.resetArray,
                                      0, // 0,// the example uses 0 for some reason, might want to
                                         // use config->collection->channelMask
                                      1  // indvidual mode == true,
                                      ))
    {
        spdlog::error("SetupLevelTrigger failed.");
        return false;
    }
    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        if (!this->adq->SetChannelTriggerMode(ch + 1,
                                              static_cast<int>(acq->triggers.at(ch).mode())))
        {
            spdlog::error("SetChannelTriggerMode for channel {} failed", ch);
            return false;
        }
    }
    for (auto rp : *this->recordProcessors)
    {
        rp->startNewAcquisition(acq);
    }
    spdlog::info("Configured acquisition successfully.");
    this->acquisitionStabilizationTimer.setInterval(acq->collection.stabilizationDelay());
    this->acquisitionTimer.setInterval(acq->collection.duration());
    return true;
}
