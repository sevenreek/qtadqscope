#ifndef BUFFERPROCESSORTHREAD_H
#define BUFFERPROCESSORTHREAD_H
#include "ADQInterfaceWrappers.h"
#include "RecordProcessor.h"
#include <vector>
#include <list>
#include <QObject>
#include <thread>
#include <chrono>
// RAM fill factor is specified on bits 4-6 of an ADQRecordHeader.
// This gives an idea of how much RAM is filled on the ADQ board.
extern const float RAM_FILL_LEVELS[];

class BufferProcessor: public QObject
{
    Q_OBJECT
public:
    BufferProcessor(std::list<IRecordProcessor*> &recordProcessors, ADQInterfaceWrapper & adq, QObject* parent = nullptr);
    ~BufferProcessor();
    enum STATE {
        INACTIVE,
        ACTIVE,
        STOPPING,
        SERROR,
    };
    //Q_ENUM(STATE)
    BufferProcessor::STATE getLoopState() const;
    void reset();
    void stop();
    float getRamFillLevel();
    float getAverageThreadStarvation();
    void configureNewAcquisition(Acquisition *acq);
public slots:
    void startBufferProcessLoop();
private:
    std::mutex stateMutex;
    float threadStarved = 0.0f;
    unsigned long recordLength;
    bool isContinuous;
    int lastRAMFillLevel = 0;
    BufferProcessor::STATE loopState = BufferProcessor::STATE::INACTIVE;
    unsigned long long recordsStored = 0;
    unsigned long long lastRecordNumber = 0;
    // record completion listeners
    std::list<IRecordProcessor*> &recordProcessors;
    ADQInterfaceWrapper & adq;
    // notify record listeners (processors)
    bool completeRecord(ADQRecord* record, size_t bufferSize);
    bool handleWaitForRecordErrors(long long returnValue);
    void enterErrorCondition();
    void changeState(BufferProcessor::STATE newState);
signals:
    void ramFillChanged(float ramFill);
    void stateChanged(BufferProcessor::STATE newState);
};

#endif // BUFFERPROCESSORTHREAD_H
