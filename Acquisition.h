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
#include <QThread>
#include <iostream>
#include <list>
#include "AcquisitionThreads.h"
enum ACQUISITION_STATES {
    STOPPED,
    STOPPING,
    RUNNING
};
enum FILE_WRITERS {
    NONE,
    BINARY,
};
class Acquisition : public QObject
{
    Q_OBJECT
protected:
    ApplicationConfiguration& appConfig;
    ADQInterface& adqDevice;
    std::list<std::shared_ptr<RecordProcessor>> recordProcessors;
    WriteBuffers writeBuffers;
    DMAChecker *dmaChecker;
    std::unique_ptr<BufferProcessor> bufferProcessor;
    LoopBufferProcessor *bufferProcessorHandler;

    bool dmaCheckingActive = false;
    bool bufferProcessingActive = false;

    bool dmaLoopStopped = true;
    bool processingLoopStopped = true;

    bool configured = false;
    QThread dmaCheckingThread;
    QThread bufferProcessingThread;
    ACQUISITION_STATES state = ACQUISITION_STATES::STOPPED;
    void stopDMAChecker();
    void stopProcessor();
    void joinThreads();
    void setState(ACQUISITION_STATES state);
    unsigned long lastBuffersFilled;

public:
    ~Acquisition();
    Acquisition(
        ApplicationConfiguration& appConfig,
        ADQInterface& adqDevice
    );
    ACQUISITION_STATES getState();
    bool configure();
    bool start();
    unsigned long checkDMA();
public slots:
    void appendRecordProcessor(std::shared_ptr<RecordProcessor> rp);
    void removeRecordProcessor(std::shared_ptr<RecordProcessor> rp);
    bool stop();
    void error();
    void onAcquisitionThreadStopped();
    void onProcessingThreadStopped();
    void buffersFilled(unsigned long filled);
signals:
    void onStateChanged(ACQUISITION_STATES newState);
    void onStart();
    void periodicUpdate(unsigned long averageBuffersFilled, unsigned long queueFill);
};

#endif // ACQUISITION_H
