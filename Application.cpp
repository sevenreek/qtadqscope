#include "Application.h"
#include "AcquisitionConfiguration.h"
#include "BinaryFileWriter.h"
#include "qapplication.h"
#include <algorithm>
Application::Application(int argc, char* argv[]) : qapp(argc, argv)
{
}

int Application::start()
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("config_file", "Configuration JSON to load.");
    QCommandLineOption commandLineMode("c", "Start acqusition in command line mode.");
    parser.addOption(commandLineMode);
    parser.process(this->qapp);

    const QStringList args = parser.positionalArguments();
    QString acquisitionFile = "lastconfig.json";

    if(args.count() > 0)
        acquisitionFile = args.at(0);
    QFile file(acquisitionFile);
    if(file.exists()) {
        file.open(QFile::OpenModeFlag::ReadOnly);
        QJsonParseError err;
        QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &err);
        if(err.error == QJsonParseError::NoError) {
            this->appConfiguration = ApplicationConfiguration::fromJSON(json.object()["app"].toObject());
            acq = AcquisitionConfiguration::fromJSON(json.object()["acquisition"].toObject());
        } else {
            spdlog::warn("Failed to load configuration {}. JSON parsing error.", acquisitionFile.toStdString());
        }
    }
    cfg.setStartGUI(!parser.isSet(commandLineMode));

    this->stdSink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
    this->primaryLogger = std::make_shared<spdlog::logger>("primary", stdSink);
    spdlog::set_default_logger(this->primaryLogger);
    this->primaryLogger->set_pattern(LOGGER_PATTERN);
    this->primaryLogger->set_level(this->getFileLoggingLevel(this->appConfiguration.fileLoggingLevel));
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

    ADQControlUnit_EnableErrorTrace(this->adqControlUnit, std::max((int)this->appConfiguration.adqLoggingLevel, 3), "."); // log to root dir, LOGGING_LEVEL::DEBUG is 4 but API only supports INFO=3
    ADQControlUnit_FindDevices(this->adqControlUnit);
    int numberOfDevices = ADQControlUnit_NofADQ(adqControlUnit);
    if(numberOfDevices == 0)
    {
        spdlog::critical("No ADQ devices found. Exiting.");
        return false;
    }
    else if(numberOfDevices != 1)
    {
        spdlog::warn("Found {} devices. Using {}.", numberOfDevices, this->appConfiguration.deviceNumber);
    }
    this->digitizer = std::unique_ptr<Digitizer>(new Digitizer(this->adq));
    this->digitizer->cfg().
    this->config = cfg;
    return true;
}
spdlog::level::level_enum Application::getFileLoggingLevel(LOGGING_LEVELS lvl)
{
    switch(lvl)
    {
        case LOGGING_LEVELS::DISABLED:
            return spdlog::level::level_enum::off;
        break;
        case LOGGING_LEVELS::DEBUG:
            return spdlog::level::level_enum::debug;
        break;
        case LOGGING_LEVELS::INFO:
            return spdlog::level::level_enum::info;
        break;
        case LOGGING_LEVELS::WARN:
            return spdlog::level::level_enum::warn;
        break;
        case LOGGING_LEVELS::ERR:
            return spdlog::level::level_enum::err;
        break;
    }
    return spdlog::level::trace;
}

Application::~Application()
{
    DeleteADQControlUnit(this->adqControlUnit);
}

GUIApplication::GUIApplication()
{

}

bool GUIApplication::start(ApplicationConfiguration cfg, Acquisition acq)
{
    if(!this->ScopeApplication::start(cfg, acq)) { return false; }
    this->scopeUpdater = std::unique_ptr<ScopeUpdater>(new ScopeUpdater(this->digitizer->getRecordLength()));
    this->context = std::unique_ptr<ApplicationContext>(
        new ApplicationContext(
            &this->config, this->digitizer.get(), this->scopeUpdater.get(), this->primaryLogger.get()
        )
    );
    this->primaryWindow = std::unique_ptr<PrimaryWindow>(new PrimaryWindow(this->context.get()));
    this->primaryWindow->reloadUI();
    this->primaryWindow->show();
    return true;
}
CLIApplication::CLIApplication()
{

}
bool CLIApplication::start(ApplicationConfiguration cfg, Acquisition acq)
{
    if(!this->ScopeApplication::start(cfg, acq)) { return false; }
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
                        this->periodicUpdateTimer.stop();
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
            spdlog::warn("No file save mode selected. No data will be acquired.");
            this->fileSaver.reset();
        //break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BINARY:
            this->fileSaver = std::unique_ptr<IRecordProcessor>(new BinaryFileWriter(this->digitizer->getFileSizeLimit()));
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BINARY_VERBOSE:
            this->fileSaver = std::unique_ptr<IRecordProcessor>(new VerboseBinaryWriter(this->digitizer->getFileSizeLimit()));
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BUFFERED_BINARY:
        case ApplicationConfiguration::FILE_SAVE_MODES::BUFFERED_BINARY_VERBOSE:
            spdlog::critical("Buffered file writers no longer supported.");
            return false;
        break;
    }
    if(this->fileSaver)
    {
        this->digitizer->appendRecordProcessor(this->fileSaver.get());
    }
    this->periodicUpdateTimer.connect(&this->periodicUpdateTimer, &QTimer::timeout, [=](){
        spdlog::info("Update: {}B acquired. Remaining {:.2f} seconds.",
            doubleToPrefixNotation(this->fileSaver->getProcessedBytes()),
            this->digitizer->durationRemaining()/1000.0
        );
    });
    this->periodicUpdateTimer.setInterval(this->digitizer->getDuration()/10);
    this->periodicUpdateTimer.start();

    if(this->digitizer->getDuration()>3000)
    {
        spdlog::debug("Fast update timer active.");
        this->fastUpdateTimer.connect(&this->fastUpdateTimer, &QTimer::timeout, [=](){
            std::cout << fmt::format("@{}B {:.2f} s left\r",
                doubleToPrefixNotation(this->fileSaver->getProcessedBytes()),
                this->digitizer->durationRemaining()/1000.0
            );
        });
        this->fastUpdateTimer.setInterval(500);
        this->fastUpdateTimer.start();
    }

    this->digitizer->runAcquisition();
    return true;
}

