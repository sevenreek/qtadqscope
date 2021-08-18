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
class ScopeApplication : public QObject
{
    Q_OBJECT
protected:
    ApplicationConfiguration config;
    void* adqControlUnit;
    std::unique_ptr<ADQInterfaceWrapper> adqWrapper;
    std::unique_ptr<Digitizer> digitizer;
public:
    virtual bool start(ApplicationConfiguration cfg, Acquisition acq);
};

class GUIApplication : public ScopeApplication
{
protected:
    std::unique_ptr<ScopeUpdater> scopeUpdater;
    std::unique_ptr<ApplicationContext> context;
    std::unique_ptr<PrimaryWindow> primaryWindow;
public:
    GUIApplication();
    bool start(ApplicationConfiguration cfg, Acquisition acq);
};

class CLIApplication : public ScopeApplication
{
private:
    std::unique_ptr<IRecordProcessor> fileSaver;
public:
    CLIApplication();
    bool start(ApplicationConfiguration cfg, Acquisition acq);
};

#endif // UIAPPLICATION_H
