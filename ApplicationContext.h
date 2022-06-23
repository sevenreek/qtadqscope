#ifndef APPLICATIONCONTEXT_H
#define APPLICATIONCONTEXT_H
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
public:
    ApplicationContext(ApplicationConfiguration * config, Digitizer * digitizer, ScopeUpdater *scope, spdlog::logger *logger);
    ApplicationConfiguration * appConfiguration = nullptr;
    Digitizer * digitizer = nullptr;
    ScopeUpdater *scopeUpdater = nullptr;
    IRecordProcessor *fileSaver = nullptr;
    spdlog::logger *primaryLogger = nullptr;
};
#endif // APPLICATIONCONFIGURATION_H
