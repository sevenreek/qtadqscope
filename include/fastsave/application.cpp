#include "application.hpp"
#include "hdf5_write.hpp"
#include <csignal>
Application::Application(int argc, char *argv[], int deviceNumber)
{
    Application::appInstance = this;

    signal(SIGINT, Application::sigintHandler);

    this->adqControlUnit = CreateADQControlUnit();
    if(this->adqControlUnit == NULL)
    {
        spdlog::error("Failed to create ADQControlUnit. Exiting...");
        this->cleanup(true);
    }
    this->settings = ApplicationSettings();
    this->settings.parseSysArgs(argc, argv);

    switch(this->settings.loggingLevel)
    {
        case LOGGING_LEVEL::DISABLED: spdlog::set_level(spdlog::level::level_enum::off); break;
        case LOGGING_LEVEL::ERROR: spdlog::set_level(spdlog::level::level_enum::err); break;
        case LOGGING_LEVEL::WARN: spdlog::set_level(spdlog::level::level_enum::warn); break;
        case LOGGING_LEVEL::INFO: spdlog::set_level(spdlog::level::level_enum::info); break;
        case LOGGING_LEVEL::DEBUG: spdlog::set_level(spdlog::level::level_enum::debug); break;
        default: spdlog::set_level(spdlog::level::level_enum::debug); break;
    }
    ADQControlUnit_EnableErrorTrace(adqControlUnit, std::max((int)this->settings.loggingLevel, 3), "."); // log to root dir, LOGGING_LEVEL::DEBUG is 4 but API only supports INFO=3
    ADQControlUnit_FindDevices(adqControlUnit);
    int numberOfDevices = ADQControlUnit_NofADQ(adqControlUnit);
    if(numberOfDevices == 0)
    {
        spdlog::error("No ADQ devices found. Exiting.");
        this->cleanup(true);
    }
    else if(numberOfDevices != 1)
    {
        spdlog::warn("Found {} devices. Using {}.", numberOfDevices, deviceNumber);
    }
    this->adqDevice = ADQControlUnit_GetADQ(adqControlUnit, deviceNumber);
    
}
Application::~Application()
{
    this->cleanup(false);
}
Application* Application::appInstance = NULL;
unsigned int Application::sigintCount = 0;
void Application::start()
{
    spdlog::debug("Application::start()");
    this->adqDevice->StopStreaming();
    this->recordProcessor = new RamBufferBinaryWriter("continuous.dat", "triggered.dat", this->settings.fileSizeLimit);
    if(this->recordProcessor == nullptr) {spdlog::critical("Out of memory for recordProcessor."); this->cleanup(true);}
    this->bufferProcessor = new RecordStoringProcessor(this->settings.recordLength, recordProcessor);
    if(this->bufferProcessor == nullptr) {spdlog::critical("Out of memory for bufferProcessor."); this->cleanup(true);}
    this->writeBuffers = new WriteBuffers(this->settings.writeBufferCount, this->settings.transferBufferSize, this->settings.channelMask);
    if(this->writeBuffers == nullptr) {spdlog::critical("Out of memory for writeBuffers."); this->cleanup(true);}
    this->streaming = new SemaphoreStreaming(
        this->adqDevice, 
        &(this->settings), 
        this->recordProcessor, 
        this->bufferProcessor, 
        this->writeBuffers
    );
    if(this->streaming == nullptr) {spdlog::critical("Out of memory for this->streaming."); this->cleanup(true);}
    if(!this->streaming->configure())
    {
        spdlog::critical("Streaming configuration failed. Exiting...");
        return;
    }
    printf("Press r to run acquisition or e to exit. ");
    char c = getchar();
    if(c == 'e') { return; }
    this->streaming->start(this->settings.streamDuration?this->settings.streamDuration/5:1000);
    unsigned long long bytesSaved = this->recordProcessor->finish();
    spdlog::info("Saved {} bytes. Data rate: {:.2f} GB/s.", bytesSaved, bytesSaved/1024.0/1024.0/this->settings.streamDuration);
    //this->cleanup(false);
}
void Application::cleanup(bool quit)
{
    spdlog::debug("Application::cleanup()");
    if(Application::sigintCount > 5)
    {
        this->adqDevice->StopStreaming();
        spdlog::error("Hard exit.");
        exit(1);
    }
    if(this->recordProcessor != nullptr)
    {
        spdlog::debug("Deleting recordProcessor");
        delete this->recordProcessor;
        this->recordProcessor = nullptr;
    }
    if(this->bufferProcessor != nullptr)
    {
        spdlog::debug("Deleting bufferProcessor");
        delete this->bufferProcessor;
    }
    if(this->streaming != nullptr) {
        spdlog::debug("Deleting streaming");
        this->streaming->stop();
        while(!this->streaming->stopped);
        delete this->streaming;
        this->streaming = nullptr;
    }
    if(this->writeBuffers != nullptr)
    {
        spdlog::debug("Deleting writeBuffers");
        delete this->writeBuffers;
        this->writeBuffers = nullptr;
    }
    //free(this->adqDevice);    // how and if to delete ADQInterface free() dumps core
    if(this->adqControlUnit) {
        DeleteADQControlUnit(this->adqControlUnit);
        this->adqControlUnit = NULL;
    }

    if(quit)
    {
        spdlog::info("Exiting");
        delete this;
        exit(0);
    }
}

void Application::sigintHandler(int s)
{
    spdlog::debug("Application::sigintHandler()");
    Application::sigintCount++;
    if(Application::sigintCount>5)
        Application::appInstance->cleanup(true);
}