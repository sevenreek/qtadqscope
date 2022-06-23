#ifndef GUIAPPLICATION_H
#define GUIAPPLICATION_H
#include "ApplicationConfiguration.h"
#include "PrimaryWindow.h"
#include "Digitizer.h"
#include "ApplicationContext.h"
#include <QObject>
#include <QApplication>
#include "spdlog/spdlog.h"
#include "ApplicationContext.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "GUILogger.h"

class Application : public QObject
{
    Q_OBJECT
protected:
    QApplication qapp;
    std::shared_ptr<spdlog::sinks::stdout_sink_mt> stdSink;
    std::shared_ptr<spdlog::logger> primaryLogger;
    void* adqControlUnit;
    ADQInterface *adq;
    std::unique_ptr<Digitizer> digitizer;
    ApplicationConfiguration appConfiguration;
    static spdlog::level::level_enum getFileLoggingLevel(LOGGING_LEVELS lvl);
    void processArguments();
public:
    int start();
    Application(int argc, char* argv[]);
    ~Application();
};

class GUIApplication : public ScopeApplication
{
protected:
    std::unique_ptr<ApplicationContext> context;
    std::unique_ptr<PrimaryWindow> primaryWindow;
    std::unique_ptr<QGUILogSink_mt> guiSink;
public:
    GUIApplication();
    bool start(ApplicationConfiguration cfg, Acquisition acq);
};

class CLIApplication : public ScopeApplication
{
private:
    QTimer periodicUpdateTimer;
    QTimer fastUpdateTimer;
    std::unique_ptr<IRecordProcessor> fileSaver;
public:
    CLIApplication();
    bool start(ApplicationConfiguration cfg, Acquisition acq);
};

#endif // UIAPPLICATION_H
