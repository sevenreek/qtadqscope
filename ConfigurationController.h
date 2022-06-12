#ifndef CONFIGURATIONCONTROLLER_H
#define CONFIGURATIONCONTROLLER_H

#include <QObject>
#include "ApplicationConfiguration.h"
#include "AcquisitionConfiguration.h"

class ConfigurationController : public QObject
{
    Q_OBJECT
protected:
    AcquisitionConfiguration defaultAcquisitionConfiguration;
    ApplicationConfiguration applicationConfiguration;
public:
    explicit ConfigurationController(QObject *parent = nullptr);
    AcquisitionConfiguration &acq();
    ApplicationConfiguration &app();
    bool loadConfigurationsFromFile(const QString &path);
    bool saveConfigurationsToFile(const QString &path);
    void notifyDataCollectionChanged();
    void notifySpectroscopeChanged();
    void notifyFileSaveChanged();
    void notifyDataTransferChanged();
    void notifyRecordChanged(int ch);
    void notifyCalibrationChanged(int ch);
    void notifyAFEChanged(int ch);

signals:
    void dataCollectionChanged();
    void spectroscopeChanged();
    void fileSaveChanged();
    void dataTransferChanged();
    void recordChanged(int ch);
    void calibrationChanged(int ch);
    void afeChanged(int ch);

};

#endif // CONFIGURATIONCONTROLLER_H
