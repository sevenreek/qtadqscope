#ifndef CONFIGURATIONCONTROLLER_H
#define CONFIGURATIONCONTROLLER_H

#include <QObject>
#include "AcquisitionConfiguration.h"
#include "ApplicationConfiguration.h"
#include "qjsondocument.h"
#include <QFile>
#include <QJsonDocument>

class QConfigurationController : public QObject 
{
    Q_OBJECT
protected:
    AcquisitionConfiguration acquisitionConfiguration;
    ApplicationConfiguration applicationConfiguration;
    void loadFromV0JSON(QJsonObject &json);
public:
    explicit QConfigurationController(QObject *parent = nullptr);
    AcquisitionConfiguration &acq();
    void setAcquisition(AcquisitionConfiguration acq);
    ApplicationConfiguration &app();
    void setApplication(ApplicationConfiguration app);
    void notifyCollectionChanged();
    void notifySpectroscopeChanged();
    void notifyStorageChanged();
    void notifyTransferChanged();
    void notifyRecordChanged(int ch);
    void notifyCalibrationChanged(int ch);
    void notifyAFEChanged(int ch);
    void notifyTriggerChanged(int ch);
    void loadFromJSONDocument(const QJsonDocument &doc);
    bool loadFromFile(const std::string &path);
    bool saveToFile(const std::string &path);
    void hookUpConfigChangedListeners();

signals:
    void collectionChanged();
    void spectroscopeChanged();
    void storageChanged();
    void transferChanged();
    void recordChanged(int ch);
    void calibrationChanged(int ch);
    void afeChanged(int ch);
    void triggerChanged(int ch);

};

#endif // CONFIGURATIONCONTROLLER_H
