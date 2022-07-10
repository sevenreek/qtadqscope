#ifndef APPLICATION_H
#define APPLICATION_H
#include "ConfigurationController.h"
#include "PrimaryWindow.h"
#include "ADQAPIIncluder.h"
#include "Digitizer.h"
#include "ApplicationContext.h"
#include <QObject>
#include <QApplication>
#include <QJsonDocument>
#include "spdlog/spdlog.h"
#include "ApplicationContext.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "GUILogger.h"
class ScopeApplication : public QObject
{
    Q_OBJECT
protected:
    bool createConfigurationController(QJsonDocument * initialJSON);
    bool createLogger();
    bool createDigitizer();
    std::shared_ptr<spdlog::sinks::stdout_sink_mt> stdSink;
    std::shared_ptr<spdlog::logger> primaryLogger;
    std::unique_ptr<QConfigurationController> configurationController; 
    void* adqControlUnit = nullptr;
    ADQInterface * adq = nullptr;
    std::unique_ptr<Digitizer> digitizer;
    static spdlog::level::level_enum getFileLevel(LOGGING_LEVELS lvl);
public:
    virtual bool start(QJsonDocument *json=nullptr);
    virtual ~ScopeApplication();
};

class GUIApplication : public ScopeApplication
{
protected:
    std::unique_ptr<PrimaryWindow> primaryWindow;
public:
    GUIApplication();
    bool start(QJsonDocument *json=nullptr) override;
};

class CLIApplication : public ScopeApplication
{
private:
    QTimer periodicUpdateTimer;
    QTimer fastUpdateTimer;
    std::unique_ptr<IRecordProcessor> fileSaver;
public:
    CLIApplication();
    bool start(QJsonDocument *json=nullptr) override;
};

#endif // APPLICATION_H