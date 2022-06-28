#include "ConfigurationController.h"
#include "AcquisitionConfiguration.h"
#include "DigitizerConstants.h"
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
AcquisitionConfiguration
QConfigurationController::acqFromV0JSON(QJsonObject &json) {
  // handle legacy code...
  throw std::runtime_error("Legacy JSON loading not implemented yet.");
}
bool QConfigurationController::loadAcquisitionFromFile(
    const std::string &path) {
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
  QJsonObject jsono = json.object();
  int version = jsono["version"].toInt(0);
  if (version >= 4) {
    this->acquisitionConfiguration = AcquisitionConfiguration::fromJSON(jsono);
  } else {
    this->acquisitionConfiguration = this->acqFromV0JSON(jsono);
  }
  this->hookUpConfigChangedListeners();

  return true;
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
bool QConfigurationController::saveAcquisitionToFile(const std::string &path) {
  QFile file(QString::fromStdString(path));
  file.open(QFile::OpenModeFlag::WriteOnly);
  QJsonDocument doc;
  QJsonObject acq = this->acquisitionConfiguration.toJSON();
  doc.setObject(acq);
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