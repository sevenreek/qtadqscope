#ifndef QADQDEVICE_H
#define QADQDEVICE_H

#include <QObject>
#include "ADQAPI.h"
#include <memory>
#include "StreamingBuffers.h"
#include <QTimer>
#include <QThread>
#include "spdlog/spdlog.h"
#include <vector>
#include "RecordProcessor.h"
#include <QThreadPool>
#include "ApplicationConfiguration.h"
#include "BufferProcessor.h"



class QADQDevice;

class QADQStreamProcessor : public QObject
{
    Q_OBJECT
private:
    bool loopActive = false;
    WriteBuffers &writeBuffers;
    unsigned long transferBufferCount;
    BufferProcessor &processor;
public:
    QADQStreamProcessor(WriteBuffers &writeBuffers, BufferProcessor &processor);
    void run(bool triggered);
    void stop();
signals:
    void onStopped();
};

class QAcquisition : public QObject
{
    Q_OBJECT
public:
    enum STATES {
        STOPPED,
        RUNNING,
        STOPPING
    };
    enum RESULT {
        FINISHED,
        USER_STOP,
        OVERFLOWED,
        OTHER_ERROR
    };
    QAcquisition(QADQDevice &device, WriteBuffers &writeBuffers);
    class SmartBuffer {
        // adheres to RAII
        WriteBuffers &wb;
        StreamingBuffers &sb;
    public:
        SmartBuffer(WriteBuffers &wb, StreamingBuffers &sb);
        ~SmartBuffer();
        StreamingBuffers &data();
    };
    std::unique_ptr<SmartBuffer> requestBuffer(unsigned int timeout);
    QAcquisition::STATES getState() const;
public slots:
    bool run(unsigned int bufferCount, unsigned int dmaFlushDuration, unsigned int duration=0);
    bool stop(bool dumpUnprocessed=false);

private:
    ADQRecordHeader incompleteHeaders[MAX_NOF_CHANNELS];
    bool loopDMA = false;
    QElapsedTimer acquisitionTimer;
    QElapsedTimer dmaFlushTimer;
    QAcquisition::STATES state = STATES::STOPPED;
    QADQDevice &device;
    WriteBuffers &writeBuffers;
    void setState(STATES state);
signals:
    void onStateChanged(STATES state);
    void onStopped(RESULT res);
    void onBuffersFilled(unsigned long count);
    void onBufferWritten();
};

class QADQDevice : public QObject
{
    Q_OBJECT
private:
    unsigned int deviceNumber;
    void * adqCU;
    std::shared_ptr<ADQInterface> device;
    bool configured = false;
    ApplicationConfiguration & appConfig;
    WriteBuffers writeBuffers;
    QAcquisition acquisition;
public:
    struct BufferDimensions{
        unsigned int count;
        unsigned int size;
    };
    QADQDevice(void* adqapi, unsigned int deviceNumber, ApplicationConfiguration &conf, QThread *thread=nullptr);

    // ACQUISITION
    bool configureAcquisition();
    bool overrideAcquisition(ApplicationConfiguration &conf);
    bool startAcquisition(unsigned int bufferCount, unsigned int dmaFlush, unsigned int duration);
    bool stopAcquisition(bool dumpData=false);
    QAcquisition::STATES getAcquisitionState();

    // STREAMING
    bool startStreaming();
    bool stopStreaming();

    // TRIGGER
    TRIGGER_MODES getTriggerMode(); bool setTriggerMode(TRIGGER_MODES mode);
    int getTriggerLevel();          bool setTriggerLevel(int level);
    TRIGGER_EDGES getTriggerEdge(); bool setTriggerEdge(TRIGGER_EDGES edges);
                                    bool setTriggerReset(int reset);
    unsigned char getTriggerMask(); bool setTriggerMask(unsigned char mask);
    bool SWTrig();

    // DMA
    bool flushDMA();
    unsigned int getFilledBuffersCount();
    bool isOverflowed();
    bool acquireBuffer(StreamingBuffers& to);

    // INPUT
    float getInputRangeMilvolts(int ch);
    bool setInputRange(int ch, float target, float &result);
    int getDCBias(int ch);       bool setDCBias(int ch, int offset);
    bool setGainAndOffset(int ch, int gain, int offset);

    // USER LOGIC
    bool writeUserRegister(unsigned int target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int &retval);

    // HELPERS
    int milivoltsToCodes(int ch, float mv);
    float codesToMilivolts(int ch, int codes);

    std::unique_ptr<QAcquisition::SmartBuffer> requestBuffer(unsigned int timeout);

signals:
    void onAcquisitionStateChanged(QAcquisition::STATES state);
};

#endif // QADQDEVICE_H
