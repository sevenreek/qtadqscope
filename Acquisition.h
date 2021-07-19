#ifndef ACQUISITION_H
#define ACQUISITION_H
#include "ADQDeviceConfiguration.h"
#include "ApplicationConfiguration.h"
#ifdef MOCK_ADQAPI
    #include "MockADQAPI.h"
#else
    #include "ADQAPI.h"
#endif
#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "BufferProcessor.h"
#include "RecordProcessor.h"
#include "StreamingBuffers.h"
#include "ScopeUpdater.h"
#include <QThread>
#include <iostream>
#include <list>
#include <chrono>
#include "AcquisitionThreads.h"
#include "QADQWrapper.h"
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
    std::unique_ptr<QTimer> acqusitionTimer;
    std::shared_ptr<ApplicationConfiguration> appConfig;
    std::shared_ptr<ADQInterfaceWrapper> adqDevice;
    std::list<std::shared_ptr<RecordProcessor>> recordProcessors;
    std::shared_ptr<WriteBuffers> writeBuffers;
    std::unique_ptr<DMAChecker> dmaChecker;
    std::shared_ptr<BufferProcessor> bufferProcessor;
    std::unique_ptr<LoopBufferProcessor> bufferProcessorHandler;

    bool dmaCheckingActive = false;
    bool bufferProcessingActive = false;

    bool dmaLoopStopped = true;
    bool processingLoopStopped = true;

    bool configured = false;
    QThread adqThread;
    QThread bufferProcessingThread;
    ACQUISITION_STATES state = ACQUISITION_STATES::STOPPED;
    void stopDMAChecker();
    void stopProcessor();
    void joinThreads();
    void setState(ACQUISITION_STATES state);
    unsigned long lastBuffersFilled;
    void initialize();
    bool isStreamFullyStopped();
public:
    virtual ~Acquisition();
    Acquisition(
        std::shared_ptr<ApplicationConfiguration> appConfig,
        std::shared_ptr<ADQInterfaceWrapper> adqDevice
    );
    ACQUISITION_STATES getState();
    bool configure(std::shared_ptr<ApplicationConfiguration> providedConfig, std::list<std::shared_ptr<RecordProcessor>> recordProcessors);
    bool start(bool needSwTrig, unsigned int dmaFlushTimeout=100);
    bool startTimed(unsigned long msDuration, bool needSwTrig);
    unsigned long checkDMA();
    std::chrono::time_point<std::chrono::high_resolution_clock> timeStarted;
    std::chrono::time_point<std::chrono::high_resolution_clock> timeStopped;
    unsigned long getBuffersFill();
    int getReadQueueFill();
    int getWriteQueueFill();

public slots:
    bool stop();
    void error();
    void onAcquisitionThreadStopped();
    void onProcessingThreadStopped();
    void buffersFilled(unsigned long filled);
    void finishRecordProcessors();
    void setStoppedState();
signals:
    void onStateChanged(ACQUISITION_STATES newState);
    void onStart();
};

#endif // ACQUISITION_H
