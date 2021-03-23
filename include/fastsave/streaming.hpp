#pragma once
#include <chrono>
#include <ctime>
#include <thread>
#include "application_settings.hpp"
#include "streaming_headers.hpp"
#include "buffer_processor.hpp"

enum BUFFER_PROCESSOR {
    COPYING
};

class SemaphoreStreaming {
private:
    WriteBuffers *writeBuffers;
protected:
    bool active;
    ADQInterface *adqDevice;
    ApplicationSettings *settings;
    BufferProcessor* bufferProcessor;
    RecordProcessor* recordProcessor;
    bool configured = false;
    std::chrono::_V2::system_clock::time_point lastFilledBufferReceivedOn;
    unsigned int dmaCheckWaitTime = 10; // ms
public:
    bool stopped = true;
    SemaphoreStreaming(
        ADQInterface *adqDevice,
        ApplicationSettings *settings,
        RecordProcessor *recordProcessor,
        BufferProcessor* bufferProcessor,
        WriteBuffers *writeBuffers
    );
    ~SemaphoreStreaming();
    void start(unsigned int flushTimeoutMs);
    bool configure();
    void stop();
    unsigned int check_dma(unsigned int flushTimeoutMs);
};


class ThreadedRecordProcessTask {
public:
    ThreadedRecordProcessTask(WriteBuffers* writeBuffers, BufferProcessor* bufferProcessor, bool isTriggeredStreaming);
    
    void stop();
    void run();
private:
    WriteBuffers* writeBuffers;
    BufferProcessor* bufferProcessor;
    bool isTriggeredStreaming;
};