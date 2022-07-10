#ifndef APPLICATIONCONTEXT_H
#define APPLICATIONCONTEXT_H
#include "ConfigurationController.h"
#include "Digitizer.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QFile>
#include "ScopeUpdater.h"
#include "RecordProcessor.h"
#include "ApplicationConfiguration.h"


class ApplicationContext {
protected:
    static ApplicationContext instance;
    QConfigurationController * config_ = nullptr;
    Digitizer * digitizer_ = nullptr;
    spdlog::logger *logger_ = nullptr;
public:
    QConfigurationController * config();
    Digitizer * digitizer();
    spdlog::logger *logger();
    void setConfig(QConfigurationController *config);
    void setDigitizer(Digitizer *digitizer);
    void setLogger(spdlog::logger *logger);
    static ApplicationContext &get();
};
#endif // APPLICATIONCONFIGURATION_H
