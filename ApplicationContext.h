#ifndef APPLICATIONCONFIGURATION_H
#define APPLICATIONCONFIGURATION_H


#include "Digitizer.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QFile>
#include "ScopeUpdater.h"
#include "RecordProcessor.h"


class ApplicationConfiguration
{
public:
    enum FILE_SAVE_MODES {
        DISABLED,
        BINARY,
        BINARY_VERBOSE,
        BUFFERED_BINARY,
        BUFFERED_BINARY_VERBOSE
    };
    unsigned int getDeviceNumber() const;
    void setDeviceNumber(unsigned int value);

    unsigned int getPeriodicUpdatePeriod() const;
    void setPeriodicUpdatePeriod(unsigned int value);

    unsigned long getDmaCheckMinimumTimeout() const;
    void setDmaCheckMinimumTimeout(unsigned long value);

    LOGGING_LEVELS getAdqLoggingLevel() const;
    void setAdqLoggingLevel(const LOGGING_LEVELS &value);

    LOGGING_LEVELS getUiLoggingLevel() const;
    void setUiLoggingLevel(const LOGGING_LEVELS &value);

    LOGGING_LEVELS getFileLoggingLevel() const;
    void setFileLoggingLevel(const LOGGING_LEVELS &value);

    static ApplicationConfiguration fromJson(QJsonObject json);
    QJsonObject toJson();

    FILE_SAVE_MODES getFileSaveMode() const;
    void setFileSaveMode(const FILE_SAVE_MODES &value);

    bool getAllowMultichannel() const;
    void setAllowMultichannel(bool value);

    bool getOffsetTriggerFromZero() const;
    void setOffsetTriggerFromZero(bool value);

    bool getUpdateScopeEnabled() const;
    void setUpdateScopeEnabled(bool value);

    bool getStartGUI() const;
    void setStartGUI(bool value);

private:
    bool startGUI = true;
    unsigned int deviceNumber = 1;
    unsigned int periodicUpdatePeriod = 10;
    unsigned long dmaCheckMinimumTimeout = 500;
    bool allowMultichannel = false;
    bool offsetTriggerFromZero = true;
    bool updateScopeEnabled = true;
    LOGGING_LEVELS adqLoggingLevel = LOGGING_LEVELS::DEBUG;
    LOGGING_LEVELS uiLoggingLevel = LOGGING_LEVELS::DEBUG;
    LOGGING_LEVELS fileLoggingLevel = LOGGING_LEVELS::DEBUG;
    FILE_SAVE_MODES fileSaveMode = FILE_SAVE_MODES::DISABLED;
};
class ApplicationContext {
public:
    ApplicationContext(ApplicationConfiguration * config, Digitizer * digitizer, ScopeUpdater *scope, spdlog::logger *logger);
    ApplicationConfiguration * config = nullptr;
    Digitizer * digitizer = nullptr;
    ScopeUpdater *scopeUpdater = nullptr;
    IRecordProcessor *fileSaver = nullptr;
    spdlog::logger *primaryLogger = nullptr;
};
#endif // APPLICATIONCONFIGURATION_H
