#pragma once
#include "streaming.hpp"
class Application {
public:
    Application(int argc, char *argv[], int deviceNumber=1);
    ~Application();
    ApplicationSettings settings;
    void start();
    void cleanup(bool quit);
    static Application* appInstance;
    static unsigned int sigintCount;
    void static sigintHandler(int s);
private:
    void* adqControlUnit;
    RecordProcessor * recordProcessor;
    BufferProcessor* bufferProcessor;
    SemaphoreStreaming * streaming;
    WriteBuffers* writeBuffers;
    ADQInterface* adqDevice;
};
