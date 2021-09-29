#include "GUIApplication.h"
#include <QApplication>
#include "spdlog/spdlog.h"
#include <thread>
#include <iostream>
#include <QCommandLineParser>
#include <QCommandLineOption>

void processArguments(QApplication &app, ApplicationConfiguration &cfg, Acquisition &acq)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("config_file", "Configuration JSON to load.");

    QCommandLineOption commandLineMode("c", "Start acqusition in command line mode.");
    parser.addOption(commandLineMode);


    parser.process(app);

    const QStringList args = parser.positionalArguments();
    QString acquisitionFile = "defaultconfig.json";

    if(args.count() > 0)
        acquisitionFile = args.at(0);
    QFile file(acquisitionFile);
    if(file.exists()) {
        file.open(QFile::OpenModeFlag::ReadOnly);
        QJsonParseError err;
        QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &err);
        if(err.error == QJsonParseError::NoError) {
            cfg = ApplicationConfiguration::fromJson(json.object());
            acq = Acquisition::fromJson(json.object()["acquisition"].toObject());
        }
    }
    cfg.setStartGUI(!parser.isSet(commandLineMode));
}

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::flush_on(spdlog::level::debug);
    QApplication a(argc, argv);



    ApplicationConfiguration config;
    Acquisition acq;
    processArguments(a, config, acq);
    std::unique_ptr<ScopeApplication> app;
    if(config.getStartGUI())
    {
        app = std::unique_ptr<ScopeApplication>(new GUIApplication());
        spdlog::info("Starting GUI.");
    }
    else
    {
        app = std::unique_ptr<ScopeApplication>(new CLIApplication());
        spdlog::info("Starting CLI.");
    }
    app->start(config, acq);
    int appres = a.exec();
    spdlog::info("Application exit");
    app.reset();
    return appres;
}

