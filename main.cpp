#include "Application.h"
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
    return appres;
}

