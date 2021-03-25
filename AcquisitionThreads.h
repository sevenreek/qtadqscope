#ifndef ACQUISITIONTHREADS_H
#define ACQUISITIONTHREADS_H
#include "StreamingBuffers.h"
#include "ADQAPI.h"
#include <QObject>
#include "BufferProcessor.h"
#define SLEEP_TIME 3
class DMAChecker : public QObject
{
    Q_OBJECT
private:
    bool loopActive = false;
    WriteBuffers &writeBuffers;
    ADQInterface &adqDevice;
    unsigned long transferBufferCount;
public:
    DMAChecker(WriteBuffers &writeBuffers, ADQInterface &adqDevice, unsigned long transferBufferCount);

public slots:
    void setTransferBufferCount(unsigned long count);
    void runLoop();
    void stopLoop();
signals:
    void onLoopStopped();
    void onBuffersFilled(unsigned long buffersFilled);
    void onError();
};

class LoopBufferProcessor : public QObject
{
    Q_OBJECT
private:
    bool loopActive = false;
    WriteBuffers &writeBuffers;
    unsigned long transferBufferCount;
    BufferProcessor& processor;
    bool isTriggeredStreaming;
public:
    LoopBufferProcessor(WriteBuffers &writeBuffers, BufferProcessor& processor);

public slots:
    void changeStreamingType(bool isTriggered);
    void runLoop();
    void stopLoop();
signals:
    void onLoopStopped();
    void onError();
};

#endif // ACQUISITIONTHREADS_H
