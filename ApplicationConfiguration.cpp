#include "ApplicationConfiguration.h"

ApplicationConfiguration ApplicationConfiguration::fromJSON(const QJsonObject& json)
{
    ApplicationConfiguration returnValue;
    returnValue.deviceNumber = json["device_number"].toInt(1);
    returnValue.periodicUpdatePeriod = json["ui_buffer_status_update"].toInt(100);

    returnValue.adqLoggingLevel  = static_cast<LOGGING_LEVELS>(json["log_adq"] .toInt(int(LOGGING_LEVELS::DEBUG)));
    returnValue.uiLoggingLevel   = static_cast<LOGGING_LEVELS>(json["log_ui"]  .toInt(int(LOGGING_LEVELS::DEBUG)));
    returnValue.fileLoggingLevel = static_cast<LOGGING_LEVELS>(json["log_file"].toInt(int(LOGGING_LEVELS::DEBUG)));

    returnValue.offsetTriggerFromZero = json["offset_from_zero"].toBool(true);
    returnValue.updateScopeEnabled = json["update_scope"].toBool(true);

    return returnValue;
}

QJsonObject ApplicationConfiguration::toJson()
{
    QJsonObject returnValue;
    returnValue.insert("device_number", int(this->deviceNumber));
    returnValue.insert("ui_buffer_status_update", int(this->periodicUpdatePeriod));
    returnValue.insert("log_adq" , this->adqLoggingLevel);
    returnValue.insert("log_ui"  , this->uiLoggingLevel);
    returnValue.insert("log_file", this->fileLoggingLevel);
    returnValue.insert("offset_from_zero", this->offsetTriggerFromZero);
    returnValue.insert("update_scope", this->updateScopeEnabled);
    return returnValue;
}
