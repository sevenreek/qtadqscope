#include "GUIApplication.h"
#include <QApplication>
#include "spdlog/spdlog.h"
#include <thread>
#include <iostream>
#include <QCommandLineParser>
#include <QCommandLineOption>
int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::flush_on(spdlog::level::debug);
    QApplication a(argc, argv);


    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("acquisition", "Acqusition configuration to load.");

    QCommandLineOption commandLineMode("c", "Start acqusition in command line mode.");
    parser.addOption(commandLineMode);

    parser.process(a);

    const QStringList args = parser.positionalArguments();
    QString acquisitionFile = "defaultconfig.json";

    if(args.count() > 0)
        acquisitionFile = args.at(0);
    std::unique_ptr<ScopeApplication> app;
    if(parser.isSet(commandLineMode))
    {
        app = std::unique_ptr<ScopeApplication>(new CLIApplication());
        spdlog::info("Starting CLI.");
    }
    else
    {
        app = std::unique_ptr<ScopeApplication>(new GUIApplication());
        spdlog::info("Starting GUI.");
    }
    app->start(acquisitionFile);
    return a.exec();
}
