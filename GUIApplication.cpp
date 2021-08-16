#include "GUIApplication.h"
#include "BinaryFileWriter.h"
bool ScopeApplication::start(QString acquisitionFile)
{
    this->adqControlUnit = CreateADQControlUnit();
#ifdef MOCK_ADQAPI
    spdlog::warn("Using mock ADQAPI");
#else
    if(this->adqControlUnit == NULL)
    {
        spdlog::critical("Failed to create ADQControlUnit. Exiting...");
        return false;
    }
#endif
    QFile file(acquisitionFile);
    Acquisition acq;
    if(file.exists()) {
        QJsonParseError err;
        QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &err);
        if(err.error == QJsonParseError::NoError) {
            this->config = ApplicationConfiguration::fromJson(json.object());
            acq = Acquisition::fromJson(json.object()["acquisition"].toObject());
        }
    }
    ADQControlUnit_EnableErrorTrace(this->adqControlUnit, std::max((int)this->config.getAdqLoggingLevel(), 3), "."); // log to root dir, LOGGING_LEVEL::DEBUG is 4 but API only supports INFO=3
    ADQControlUnit_FindDevices(this->adqControlUnit);
    int numberOfDevices = ADQControlUnit_NofADQ(adqControlUnit);
    if(numberOfDevices == 0)
    {
        spdlog::critical("No ADQ devices found. Exiting.");
        return false;
    }
    else if(numberOfDevices != 1)
    {
        spdlog::warn("Found {} devices. Using {}.", numberOfDevices, this->config.getDeviceNumber());
    }
    this->adqWrapper = std::unique_ptr<ADQInterfaceWrapper>(new MutexADQWrapper(this->adqControlUnit, this->config.getDeviceNumber()));
    this->digitizer = std::unique_ptr<Digitizer>(new Digitizer(*this->adqWrapper.get()));
    this->digitizer->setAcquisition(acq);
    return true;
}

GUIApplication::GUIApplication()
{

}

bool GUIApplication::start(QString acqusitionFile)
{
    if(!this->ScopeApplication::start(acqusitionFile)) { return false; }
    this->scopeUpdater = std::unique_ptr<ScopeUpdater>(new ScopeUpdater(this->digitizer->getRecordLength()));
    this->context = std::unique_ptr<ApplicationContext>(new ApplicationContext(&this->config, this->digitizer.get(), this->scopeUpdater.get()));
    this->primaryWindow = std::unique_ptr<PrimaryWindow>(new PrimaryWindow(this->context.get()));
    this->primaryWindow->reloadUI();
    this->primaryWindow->show();
    return true;
}
CLIApplication::CLIApplication()
{

}
bool CLIApplication::start(QString acqusitionFile)
{
    if(!this->ScopeApplication::start(acqusitionFile)) { return false; }
    if(this->digitizer->getDuration())
    {
        spdlog::info("Acquisition duration: {}", this->digitizer->getDuration());
    }
    else
    {
        spdlog::info("Acquisition duration not set.");
        return false;
    }
    this->digitizer->connect(
        this->digitizer.get(), &Digitizer::digitizerStateChanged,
        this, [=](Digitizer::DIGITIZER_STATE s) {
                switch(s)
                {
                    case Digitizer::READY:
                        spdlog::info("Acquisition finished. Exiting...");
                        QApplication::exit();
                    break;
                    case Digitizer::STOPPING:
                        spdlog::info("Acquisition stopping...");
                    break;
                    case Digitizer::ACTIVE:
                        spdlog::info("Acquisition started.");
                    break;
                }
            }
    );
    switch(this->config.getFileSaveMode())
    {
        case ApplicationConfiguration::FILE_SAVE_MODES::DISABLED:
            this->fileSaver.reset();
        //break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BINARY:
            this->fileSaver = std::unique_ptr<IRecordProcessor>(new BinaryFileWriter(this->digitizer->getFileSizeLimit()));
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BINARY_VERBOSE:
            this->fileSaver = std::unique_ptr<IRecordProcessor>(new VerboseBinaryWriter(this->digitizer->getFileSizeLimit()));
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BUFFERED_BINARY:
            this->fileSaver = std::unique_ptr<IRecordProcessor>(new BufferedBinaryFileWriter(this->digitizer->getFileSizeLimit()));
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BUFFERED_BINARY_VERBOSE:
            this->fileSaver = std::unique_ptr<IRecordProcessor>(new VerboseBufferedBinaryWriter(this->digitizer->getFileSizeLimit()));
        break;
    }
    if(this->fileSaver)
    {
        this->digitizer->appendRecordProcessor(this->fileSaver.get());
    }
    this->digitizer->runAcquisition();
    return true;
}

