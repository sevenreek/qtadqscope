#include "Application.h"
#include "spdlog/spdlog.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <iostream>
#include <thread>


class InputParameters {
public:
  bool startCLI = false;
  bool initConfigProvided = false;
  QJsonDocument json;
};

InputParameters parseInputArugments(QApplication &app) {
  InputParameters returnValue;
  QCommandLineParser parser;
  parser.setApplicationDescription("Test helper");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("config_file", "Configuration JSON to load.");

  QCommandLineOption commandLineMode("c",
                                     "Start acqusition in command line mode.");
  parser.addOption(commandLineMode);

  parser.process(app);

  const QStringList args = parser.positionalArguments();
  QString acquisitionFile = "defaultconfig.json";

  if (args.count() > 0)
    acquisitionFile = args.at(0);
  QFile file(acquisitionFile);
  if (file.exists()) {
    file.open(QFile::OpenModeFlag::ReadOnly);
    QJsonParseError err;
    returnValue.json = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
      returnValue.initConfigProvided = true;
    }
  }
  returnValue.startCLI = parser.isSet(commandLineMode);
  return returnValue;
}

int main(int argc, char *argv[]) {
  spdlog::set_level(spdlog::level::debug); // Set global log level to debug
  spdlog::flush_on(spdlog::level::debug);
  QApplication a(argc, argv);

  InputParameters inputParams = parseInputArugments(a);
  std::unique_ptr<ScopeApplication> app;
  if (inputParams.startCLI) {
    app = std::unique_ptr<ScopeApplication>(new CLIApplication());
    spdlog::info("Starting CLI.");
  } else {
    app = std::unique_ptr<ScopeApplication>(new GUIApplication());
    spdlog::info("Starting GUI.");
  }
  if (inputParams.initConfigProvided) {
    app->start(&inputParams.json);
  } else {
    app->start();
 }
  qRegisterMetaType<AcquisitionStates>("AcquisitionStates");
  int appres = a.exec();
  spdlog::info("Application exit");
  return appres;
}
