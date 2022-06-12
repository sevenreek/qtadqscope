#ifndef APPLICATIONCONFIGURATION_H
#define APPLICATIONCONFIGURATION_H
#include "Configuration.h"


class ApplicationConfiguration: public IConfiguration
{
public:
    bool startGUI = true;
    unsigned int deviceNumber = 1;
    unsigned int periodicUpdatePeriod = 10;
    bool offsetTriggerFromZero = true;
    bool updateScopeEnabled = true;
    LOGGING_LEVELS adqLoggingLevel = LOGGING_LEVELS::DEBUG;
    LOGGING_LEVELS uiLoggingLevel = LOGGING_LEVELS::DEBUG;
    LOGGING_LEVELS fileLoggingLevel = LOGGING_LEVELS::DEBUG;
    static ApplicationConfiguration fromJSON(const QJsonObject &json);
    QJsonObject toJSON() override;
};

#endif // APPLICATIONCONFIGURATION_H
