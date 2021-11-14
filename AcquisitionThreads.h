#ifndef ACQUISITIONTHREADS_H
#define ACQUISITIONTHREADS_H
#include "ADQAPIIncluder.h"
#include <QObject>
#include "QADQWrapper.h"
#include "BufferProcessor.h"
#include <chrono>
#define SLEEP_TIME 3
class DMAChecker : public QObject
{
    Q_OBJECT
private:
    bool shouldLoopRun = false;
    bool loopStopped = true;
    WriteBuffers &writeBuffers;
    unsigned long transferBufferCount;
    bool lastStatus[MAX_NOF_CHANNELS] = {true, true, true, true};
    ADQRecordHeader lastHeaders[MAX_NOF_CHANNELS];
    unsigned int lastFilledBufferCount = 1;
    unsigned long long totalRecordsGathered = 0;
    std::chrono::system_clock::time_point nextBufferCheckTime;
    unsigned int flushTimeout = 50;
    ADQInterfaceWrapper &adqDevice;
public:
    DMAChecker(WriteBuffers &writeBuffers, ADQInterfaceWrapper &adqDevice, unsigned long transferBufferCount);

    unsigned int getFlushTimeout() const;

    void setFlushTimeout(unsigned int value);

    bool isLoopStopped() const;

    unsigned int getLastFilledBufferCount() const;

public slots:
    void setTransferBufferCount(unsigned long count);
    void runLoop();
    void stopLoop();
signals:
    void onLoopStopped();
    void onBuffersFilled(unsigned long buffersFilled);
    void onBufferWritten(unsigned long count);
    void onError();
};



class LoopBufferProcessor : public QObject
{
    Q_OBJECT
private:
    bool loopStopped = true;
    bool shouldLoopRun = false;
    WriteBuffers &writeBuffers;
    unsigned long transferBufferCount;
    IBufferProcessor &processor;
    bool isTriggeredStreaming;
public:
    LoopBufferProcessor(WriteBuffers &writeBuffers, IBufferProcessor &processor);

    bool isLoopStopped() const;

public slots:
    void changeStreamingType(bool isContinuous);
    void runLoop();
    void stopLoop();
signals:
    void onLoopStopped();
    void onError();
};

#endif // ACQUISITIONTHREADS_H
