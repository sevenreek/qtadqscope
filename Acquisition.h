#ifndef ACQUISITION_H
#define ACQUISITION_H
#include "ADQDeviceConfiguration.h"
#include "ApplicationConfiguration.h"
#include "ADQAPI.h"
#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "BufferProcessor.h"
#include "RecordProcessor.h"
#include "StreamingBuffers.h"
#include "ScopeUpdater.h"
#include <thread>

enum ACQUISITION_STATES {
    STOPPED,
    STOPPING,
    RUNNING
};

class Acquisition : public QObject
{
    Q_OBJECT
protected:
    ApplicationConfiguration& appConfig;
    ADQInterface& adqDevice;
    std::unique_ptr<WriteBuffers> writeBuffers;
    std::shared_ptr<ScopeUpdater> scopeUpdater;
    bool dmaCheckingActive = false;
    bool bufferProcessingActive = false;
    bool dmaThreadActive = false;
    bool bufferThreadActive = false;
    bool configured = false;
    bool isTriggeredStreaming = false;
    std::unique_ptr<std::thread> dmaCheckingThread;
    std::unique_ptr<std::thread> bufferProcessingThread;
    ACQUISITION_STATES state = ACQUISITION_STATES::STOPPED;
    void runDMAChecker();
    void stopDMAChecker();
    void joinDMAChecker();
    void runProcessor();
    void stopProcessor();
    void joinProcessor();
    void setState(ACQUISITION_STATES state);
    void error();
public:
    std::unique_ptr<BufferProcessor> bufferProcessor;
    Acquisition() = delete;
    Acquisition(
        ApplicationConfiguration& appConfig,
        ADQInterface& adqDevice,
        std::shared_ptr<ScopeUpdater> scopeUpdater
    );
    ACQUISITION_STATES getState();
    bool configure();
    bool start();
    unsigned long checkDMA();
    bool getDMAThreadActive();
    bool getProcessingThreadActive();
public slots:
    bool stop();
signals:
    void onStateChanged(ACQUISITION_STATES newState);
    void onAcquisitionThreadStopped();
    void onProcessingThreadStopped();
};

#endif // ACQUISITION_H
