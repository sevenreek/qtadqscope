#include "Application.h"
#include "AcquisitionConfiguration.h"
#include "ApplicationConfiguration.h"
#include "BinaryFileWriter.h"
#include "ConfigurationController.h"
#include "DigitizerConstants.h"
#include <algorithm>
#include <exception>
#include <memory>
#include <qjsondocument.h>
spdlog::level::level_enum ScopeApplication::getFileLevel(LOGGING_LEVELS lvl) {
  switch (lvl) {
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

bool ScopeApplication::start(QJsonDocument *json) {
  // Popualate the application context
  if (!this->createConfigurationController(json))
    return false;
  if (!this->createLogger())
    return false;
  if (!this->createDigitizer())
    return false;
  return true;
}

bool ScopeApplication::createConfigurationController(
    QJsonDocument *initialDoc) {
  auto &appContext = ApplicationContext::get();
  // Create the configuration controller
  this->configurationController =
      std::unique_ptr<QConfigurationController>(new QConfigurationController());
  // And load the initial config from the json document if provided.
  // Defaults are automatically used in other case.
  if (initialDoc) { // initialDoc can be nullptr
    try {
      this->configurationController->loadFromJSONDocument(*initialDoc);
    } catch (const std::exception &e) {
      spdlog::warn("Encountered an error when loading JSON configuration. {}", e.what());
    }
  }
  // Feed the config controller to the context for use in child components.
  appContext.setConfig(this->configurationController.get());
  return true;
}

bool ScopeApplication::createLogger() {
  auto &appContext = ApplicationContext::get();
  // Create the primary logger to attach sinks to
  this->stdSink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
  this->primaryLogger = std::make_shared<spdlog::logger>("primary", stdSink);
  spdlog::set_default_logger(this->primaryLogger);
  this->primaryLogger->set_pattern(LOGGER_PATTERN);
  this->primaryLogger->set_level(this->getFileLevel(
      this->configurationController->app().fileLoggingLevel));
  // Feed the logger to the application context for subcomponents
  appContext.setLogger(this->primaryLogger.get());
  return true;
}

bool ScopeApplication::createDigitizer() {
  auto &appContext = ApplicationContext::get();
  int deviceNumber = appContext.config()->app().deviceNumber;
  // Create the ADQAPI interface for controlling the digitizer board
  this->adqControlUnit = CreateADQControlUnit();
#ifdef MOCK_ADQAPI
  spdlog::warn("Using mock ADQAPI");
#else
  if (this->adqControlUnit == NULL) {
    spdlog::critical("Failed to create ADQControlUnit. Exiting...");
    return false;
  }
#endif
  ADQControlUnit_EnableErrorTrace(
      this->adqControlUnit,
      std::max((int)this->configurationController->app().adqLoggingLevel, 3),
      "."); // log to root dir, LOGGING_LEVEL::DEBUG is 4 but API only supports
            // INFO=3
  ADQControlUnit_FindDevices(this->adqControlUnit);
  int numberOfDevices = ADQControlUnit_NofADQ(adqControlUnit);
  if (numberOfDevices == 0) {
    spdlog::critical("No ADQ devices found. Exiting.");
    return false;
  } else if (numberOfDevices != 1) {
    spdlog::warn("Found {} ADQ devices. Using {}.", numberOfDevices,
                 deviceNumber);
  }
  this->adq = ADQControlUnit_GetADQ(this->adqControlUnit, deviceNumber);
  if (!this->adq) {
    spdlog::critical("Failed to open ADQ device number {}.",
                     appContext.config()->app().deviceNumber);
    return false;
  }
  this->digitizer = std::unique_ptr<Digitizer>(new Digitizer(this->adq));
  appContext.setDigitizer(this->digitizer.get());
  return true;
}

ScopeApplication::~ScopeApplication() {
  DeleteADQControlUnit(this->adqControlUnit);
}

GUIApplication::GUIApplication() : ScopeApplication() {}

bool GUIApplication::start(QJsonDocument *json) {
  if (!ScopeApplication::start(json)) {
    return false;
  }
  this->primaryWindow = std::unique_ptr<PrimaryWindow>(new PrimaryWindow());
  this->primaryWindow->show();
  this->primaryWindow->reloadUI();
  return true;
}

CLIApplication::CLIApplication() {}
bool CLIApplication::start(QJsonDocument *json) {
  if (!this->ScopeApplication::start(json)) {
    return false;
  }
  auto &appContext = ApplicationContext::get();
  if (appContext.config()->acq().collection.duration()) {
    spdlog::info("Acquisition duration: {}",
                 appContext.config()->acq().collection.duration());
  } else {
    spdlog::info("Acquisition duration not set.");
    return false;
  }
  this->digitizer->connect(this->digitizer.get(),
                           &Digitizer::acquisitionStateChanged, this,
                           [=](AcquisitionStates o, AcquisitionStates n) {
                             switch (n) {
                             case AcquisitionStates::INACTIVE:
                               spdlog::info("Acquisition finished. Exiting...");
                               this->periodicUpdateTimer.stop();
                               QApplication::exit();
                               break;
                             case AcquisitionStates::STOPPING:
                               spdlog::info("Acquisition stopping...");
                               break;
                             case AcquisitionStates::ACTIVE:
                               spdlog::info("Acquisition started.");
                               break;
                             }
                           });
  this->fileSaver =
      createFileSaverFromConfig(appContext.config()->acq());
  if (this->fileSaver) {
    this->digitizer->appendRecordProcessor(this->fileSaver.get());
  }
  this->periodicUpdateTimer.connect(
      &this->periodicUpdateTimer, &QTimer::timeout, [=]() {
        spdlog::info(
            "Update: {}B acquired. Remaining {:.2f} seconds.",
            doubleToPrefixNotation(this->fileSaver->getProcessedBytes()),
            this->digitizer->durationRemaining().count() / 1000.0);
      });

  this->periodicUpdateTimer.setInterval(
      appContext.config()->acq().collection.duration() / 10);
  this->periodicUpdateTimer.start();

  if (appContext.config()->acq().collection.duration() > 3000) {
    spdlog::debug("Fast update timer active.");
    this->fastUpdateTimer.connect(
        &this->fastUpdateTimer, &QTimer::timeout, [=]() {
          spdlog::info(
              "@{}B {:.2f} s left\r",
              doubleToPrefixNotation(this->fileSaver->getProcessedBytes()),
              this->digitizer->durationRemaining().count() / 1000.0);
        });
    this->fastUpdateTimer.setInterval(500);
    this->fastUpdateTimer.start();
  }

  this->digitizer->startAcquisition();
  return true;
}
