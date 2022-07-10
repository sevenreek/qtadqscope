#include "ConfigurationController.h"
#include "AcquisitionConfiguration.h"
#include "ApplicationConfiguration.h"
#include "DigitizerConstants.h"
#include "qjsonobject.h"
#include <stdexcept>
QConfigurationController::QConfigurationController(QObject *parent)
    : QObject(parent) {
  this->hookUpConfigChangedListeners();
}
AcquisitionConfiguration &QConfigurationController::acq() {
  return this->acquisitionConfiguration;
}

void QConfigurationController::setAcquisition(AcquisitionConfiguration acq) {
  this->acquisitionConfiguration = acq;
}
ApplicationConfiguration &QConfigurationController::app() {
  return this->applicationConfiguration;
}
void QConfigurationController::setApplication(ApplicationConfiguration app) {
  this->applicationConfiguration = app;
}
void QConfigurationController::loadFromV0JSON(QJsonObject &json) {
  // handle legacy code...
  throw std::runtime_error("Legacy JSON loading not implemented yet.");
}
bool QConfigurationController::loadFromFile(const std::string &path) {
  QFile file(QString::fromStdString(path));
  if (!file.exists()) {
    spdlog::error("Acquisition file {} does not exist.", path);
    return false;
  }
  file.open(QFile::OpenModeFlag::ReadOnly);
  QJsonParseError err;
  QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &err);
  if (err.error != QJsonParseError::NoError) {
    spdlog::error("Failed to parse acquisition JSON file {}.", path);
    return false;
  }
  this->loadFromJSONDocument(json);
  return true;
}
void QConfigurationController::loadFromJSONDocument(const QJsonDocument &doc) {
  QJsonObject jsono = doc.object();
  // Check the version. If absent default to 0.
  int version = jsono["version"].toInt(0);
  if (version >= 4) {
    // The file may contain only acquisition or only app configuration
    QJsonValue acq = jsono["acquisition"].toObject();
    if (!acq.isUndefined())
      this->setAcquisition(AcquisitionConfiguration::fromJSON(acq.toObject()));
    QJsonValue app = jsono["application"].toObject();
    if (!app.isUndefined())
      this->setApplication(ApplicationConfiguration::fromJSON(app.toObject()));
  } else {
    // If version is < 4 use the legacy mode to obtain the values.
    this->loadFromV0JSON(jsono);
  }
  // Since the objects are copied, the callbacks modified callbacks must be set again.
  this->hookUpConfigChangedListeners();
}

void QConfigurationController::hookUpConfigChangedListeners() {
  this->acq().collection.setListenerCallback(
      [this]() { this->notifyCollectionChanged(); });
  this->acq().spectroscope.setListenerCallback(
      [this]() { this->notifySpectroscopeChanged(); });
  this->acq().storage.setListenerCallback(
      [this]() { this->notifyStorageChanged(); });
  this->acq().transfer.setListenerCallback(
      [this]() { this->notifyTransferChanged(); });
  for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++) {
    this->acq().AFEs.at(ch).setListenerCallback(
        [this, ch]() { this->notifyAFEChanged(ch); });
    this->acq().triggers.at(ch).setListenerCallback(
        [this, ch]() { this->notifyTriggerChanged(ch); });
    this->acq().calibrations.at(ch).setListenerCallback(
        [this, ch]() { this->notifyCalibrationChanged(ch); });
    this->acq().records.at(ch).setListenerCallback(
        [this, ch]() { this->notifyRecordChanged(ch); });
  }
}
bool QConfigurationController::saveToFile(const std::string &path) {
  QFile file(QString::fromStdString(path));
  file.open(QFile::OpenModeFlag::WriteOnly);
  QJsonDocument doc;
  QJsonObject root = QJsonObject();
  QJsonObject acq = this->acquisitionConfiguration.toJSON();
  QJsonObject app = this->applicationConfiguration.toJSON();
  root.insert("acquisition", acq);
  root.insert("application", app);
  doc.setObject(root);
  file.write(doc.toJson());
  file.close();
  return true;
}
void QConfigurationController::notifyCollectionChanged() {
  emit this->collectionChanged();
}
void QConfigurationController::notifySpectroscopeChanged() {
  emit this->spectroscopeChanged();
}
void QConfigurationController::notifyStorageChanged() {
  emit this->storageChanged();
}
void QConfigurationController::notifyTransferChanged() {
  emit this->transferChanged();
}
void QConfigurationController::notifyRecordChanged(int ch) {
  emit this->recordChanged(ch);
}
void QConfigurationController::notifyCalibrationChanged(int ch) {
  emit this->calibrationChanged(ch);
}
void QConfigurationController::notifyAFEChanged(int ch) {
  emit this->afeChanged(ch);
}
void QConfigurationController::notifyTriggerChanged(int ch) {
  emit this->triggerChanged(ch);
}