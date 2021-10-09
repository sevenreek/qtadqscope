#ifndef GUIAPPLICATION_H
#define GUIAPPLICATION_H
#include "PrimaryWindow.h"
#include "Digitizer.h"
#include "StreamingHeader.h"
#include "ApplicationContext.h"
#include <QObject>
#include <QApplication>
#include "spdlog/spdlog.h"
#include "ApplicationContext.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "GUILogger.h"
class ScopeApplication : public QObject
{
    Q_OBJECT
protected:
    std::shared_ptr<spdlog::sinks::stdout_sink_mt> stdSink;
    std::shared_ptr<spdlog::logger> primaryLogger;
    ApplicationConfiguration config;
    void* adqControlUnit;
    std::unique_ptr<ADQInterfaceWrapper> adqWrapper;
    std::unique_ptr<Digitizer> digitizer;
    static spdlog::level::level_enum getFileLevel(LOGGING_LEVELS lvl);
public:
    virtual bool start(ApplicationConfiguration cfg, Acquisition acq);
    virtual ~ScopeApplication();
};

class GUIApplication : public ScopeApplication
{
protected:
    std::unique_ptr<ScopeUpdater> scopeUpdater;
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
    std::unique_ptr<FileWriter> fileSaver;
public:
    CLIApplication();
    bool start(ApplicationConfiguration cfg, Acquisition acq);
};

#endif // UIAPPLICATION_H
