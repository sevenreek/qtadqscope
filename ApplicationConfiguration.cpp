#include "ApplicationConfiguration.h"

ApplicationConfiguration ApplicationConfiguration::fromJson(QJsonObject json)
{
    ApplicationConfiguration returnValue;
    returnValue.deviceNumber = json["device_number"].toInt(1);
    returnValue.periodicUpdatePeriod = json["ui_buffer_status_update"].toInt(100);
    returnValue.dmaCheckMinimumTimeout = json["dma_flush_period"].toInt(500);

    returnValue.fileSaveMode = static_cast<FILE_SAVE_MODES>(json["file_saver"].toInt(FILE_SAVE_MODES::DISABLED));

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
    returnValue.insert("dma_flush_period", int(this->dmaCheckMinimumTimeout));
    returnValue.insert("log_adq" , this->adqLoggingLevel);
    returnValue.insert("log_ui"  , this->uiLoggingLevel);
    returnValue.insert("log_file", this->fileLoggingLevel);
    returnValue.insert("file_saver", this->fileSaveMode);
    returnValue.insert("offset_from_zero", this->offsetTriggerFromZero);
    returnValue.insert("update_scope", this->updateScopeEnabled);
    return returnValue;
}

ApplicationConfiguration::FILE_SAVE_MODES ApplicationConfiguration::getFileSaveMode() const
{
    return fileSaveMode;
}

void ApplicationConfiguration::setFileSaveMode(const FILE_SAVE_MODES &value)
{
    fileSaveMode = value;
}


bool ApplicationConfiguration::getOffsetTriggerFromZero() const
{
    return offsetTriggerFromZero;
}

void ApplicationConfiguration::setOffsetTriggerFromZero(bool value)
{
    offsetTriggerFromZero = value;
}

bool ApplicationConfiguration::getUpdateScopeEnabled() const
{
    return updateScopeEnabled;
}

void ApplicationConfiguration::setUpdateScopeEnabled(bool value)
{
    updateScopeEnabled = value;
}

bool ApplicationConfiguration::getStartGUI() const
{
    return startGUI;
}

void ApplicationConfiguration::setStartGUI(bool value)
{
    startGUI = value;
}



unsigned int ApplicationConfiguration::getDeviceNumber() const
{
    return deviceNumber;
}

void ApplicationConfiguration::setDeviceNumber(unsigned int value)
{
    deviceNumber = value;
}

unsigned int ApplicationConfiguration::getPeriodicUpdatePeriod() const
{
    return periodicUpdatePeriod;
}

void ApplicationConfiguration::setPeriodicUpdatePeriod(unsigned int value)
{
    periodicUpdatePeriod = value;
}

unsigned long ApplicationConfiguration::getDmaCheckMinimumTimeout() const
{
    return dmaCheckMinimumTimeout;
}

void ApplicationConfiguration::setDmaCheckMinimumTimeout(unsigned long value)
{
    dmaCheckMinimumTimeout = value;
}

LOGGING_LEVELS ApplicationConfiguration::getAdqLoggingLevel() const
{
    return adqLoggingLevel;
}

void ApplicationConfiguration::setAdqLoggingLevel(const LOGGING_LEVELS &value)
{
    adqLoggingLevel = value;
}

LOGGING_LEVELS ApplicationConfiguration::getUiLoggingLevel() const
{
    return uiLoggingLevel;
}

void ApplicationConfiguration::setUiLoggingLevel(const LOGGING_LEVELS &value)
{
    uiLoggingLevel = value;
}

LOGGING_LEVELS ApplicationConfiguration::getFileLoggingLevel() const
{
    return fileLoggingLevel;
}

void ApplicationConfiguration::setFileLoggingLevel(const LOGGING_LEVELS &value)
{
    fileLoggingLevel = value;
}
