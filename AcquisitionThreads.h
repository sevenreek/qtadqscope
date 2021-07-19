#ifndef ACQUISITIONTHREADS_H
#define ACQUISITIONTHREADS_H
#include "StreamingBuffers.h"
#ifdef MOCK_ADQAPI
    #include "MockADQAPI.h"
#else
    #include "ADQAPI.h"
#endif
#include <QObject>
#include "QADQWrapper.h"
#include "BufferProcessor.h"
#include <chrono>
#define SLEEP_TIME 3
class DMAChecker : public QObject
{
    Q_OBJECT
private:
    bool loopActive = false;
    std::shared_ptr<WriteBuffers> writeBuffers;
    unsigned long transferBufferCount;
    StreamingHeader_t lastHeaders[MAX_NOF_CHANNELS];
    unsigned int lastFilledBufferCount = 1;
    unsigned long long totalRecordsGathered = 0;
    std::chrono::high_resolution_clock::time_point nextBufferCheckTime;
    unsigned int flushTimeout = 50;
    std::shared_ptr<ADQInterfaceWrapper> adqDevice;
public:
    DMAChecker(std::shared_ptr<WriteBuffers> writeBuffers, std::shared_ptr<ADQInterfaceWrapper> adqDevice, unsigned long transferBufferCount);

    unsigned int getFlushTimeout() const;

    void setFlushTimeout(unsigned int value);

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
    bool loopActive = false;
    std::shared_ptr<WriteBuffers> writeBuffers;
    unsigned long transferBufferCount;
    std::shared_ptr<BufferProcessor> processor;
    bool isTriggeredStreaming;
public:
    LoopBufferProcessor(std::shared_ptr<WriteBuffers> writeBuffers, std::shared_ptr<BufferProcessor> processor);

public slots:
    void changeStreamingType(bool isTriggered);
    void runLoop();
    void stopLoop();
signals:
    void onLoopStopped();
    void onError();
};

#endif // ACQUISITIONTHREADS_H
