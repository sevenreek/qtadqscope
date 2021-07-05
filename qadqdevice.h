#ifndef QADQDEVICE_H
#define QADQDEVICE_H

#include <QObject>
#include "ADQAPI.h"
#include <memory>
#include "QADQConfig.h"
#include "StreamingBuffers.h"
#include <QTimer>

class QADQDevice;

class QAcquisition : public QObject
{
    Q_OBJECT
public:
    enum STATES {
        READY,
        CONFIGURING,
        RUNNING,
        DISARMED,
        FORCESTOPPED
    };
    enum RESULT {
        FINISHED,
        USER_STOP,
        OVERFLOWED,
        OTHER_ERROR
    };
    QAcquisition(QADQDevice &device, WriteBuffers &writeBuffers);
private:
    ADQRecordHeader incompleteHeaders[MAX_NOF_CHANNELS];
    QTimer timer;
    QAcquisition::STATES state = STATES::READY;
    WriteBuffers &writeBuffers;
    QADQDevice &device;
    bool run(unsigned int duration=0);
    bool stop(bool dumpUnprocessed=false);
signals:
    void onStart();
    void onStopped(RESULT res);
    void onBuffersFilled(unsigned long count);
    void onBufferWritten();
};

class QADQDevice : public QObject
{
    Q_OBJECT
private:
    unsigned int deviceNumber = 1;
    float obtainedInputRange[MAX_NOF_CHANNELS];
    ADQInterface * device;
    std::unique_ptr<QADQConfiguration> activeConfiguration;
    std::unique_ptr<QADQConfiguration> nextConfiguration;
    std::shared_ptr<WriteBuffers> writeBuffers;
    std::unique_ptr<QAcquisition> acquisition;
    QADQDevice();
public:
    struct BufferDimensions{
        unsigned int count;
        unsigned int size;
    };
    static QADQDevice *create(void* adqapi, QThread *thread=nullptr);

    // ACQUISITION
    bool configureAcquisition();
    bool startAcquisition();
    bool stopAcquisition(bool dumpData=false);
    QAcquisition::STATES getAcquisitionState();
    bool setExclusiveChannel(int zeroIndexedChannel);
    bool setChannelMask(unsigned char mask);
    unsigned char getChannelMask();

    // TRIGGER
    TRIGGER_MODES getTriggerMode(); bool setTriggerMode(TRIGGER_MODES mode);
    int getTriggerLevel();          bool setTriggerLevel(int level);
    TRIGGER_EDGES getTriggerEdge(); bool setTriggerEdge(TRIGGER_EDGES edges);
    int getPretrigger();            bool setPretrigger(int pretrigger);
    int getTriggerDelay();          bool setTriggerDelay(int delay);
    int getTriggerReset();          bool setTriggerReset(int reset);
    bool enableTriggered();
    bool enableContinuous();
    bool isTriggeredStreaming();

    // RECORD
    unsigned int getRecordLength(); bool setRecordLength(unsigned int length);
    unsigned int getRecordCount();  bool setRecordCount(unsigned int count);

    // BUFFERS
    BufferDimensions getTransferBuffersDimensions();
    bool setTransferBuffersDimensions(unsigned int count, unsigned int size);
    unsigned int getQueueSize(); bool setQueueSize(unsigned int length);

    // DMA
    bool flushDMA();
    unsigned int getFilledBuffersCount();
    bool isOverflowed();
    StreamingBuffers * getNextBuffer();


    // HELPERS
    int milivoltsToCodes(float mv);
    float codesToMilivolts(int codes);
};

#endif // QADQDEVICE_H
