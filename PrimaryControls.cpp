#include "PrimaryControls.h"
#include "DigitizerConstants.h"
#include "ui_PrimaryControls.h"
#include <algorithm>

PrimaryControls::PrimaryControls(QWidget *parent)
    : QWidget(parent), ui(new Ui::PrimaryControls) {
  ui->setupUi(this);
}

PrimaryControls::~PrimaryControls() {
  this->periodicUpdateTimer.stop();
  delete ui;
}
void PrimaryControls::initialize(ApplicationContext *context) {
  this->DigitizerGUIComponent::initialize(context);
  // Create and append the log sink for printing logs to the UI
  this->logSink = std::shared_ptr<QGUILogSink_mt>(
      new QGUILogSink_mt(this->ui->notificationsTextArea));
  this->logSink->set_pattern(LOGGER_PATTERN);
  this->context->primaryLogger->sinks().push_back(this->logSink);

  // React to acquisition state changes
  this->digitizer->connect(this->digitizer,
                           &Digitizer::acquisiitionStateChanged, this,
                           [=](AcquisitionStates oldS, AcquisitionStates newS) {
                             this->onAcquisitionStateChanged(oldS, newS);
                           });

  // Connect the UI elements behaviour.
  this->ui->streamStartStopButton->connect(
      this->ui->streamStartStopButton, &QAbstractButton::clicked, this,
      &PrimaryControls::primaryButtonClicked);
  this->ui->timedRunCheckbox->connect(this->ui->timedRunCheckbox,
                                      &QCheckBox::stateChanged, this,
                                      &PrimaryControls::enableTimedRun);
  this->ui->timedRunValue->connect(
      this->ui->timedRunValue,
      static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
      &PrimaryControls::changeRunDuration);
  this->periodicUpdateTimer.connect(&this->periodicUpdateTimer,
                                    &QTimer::timeout, this,
                                    &PrimaryControls::periodicUIUpdate);
  this->reloadUI();
}
void PrimaryControls::enableTimedRun(int enabled) {
  auto &acq = this->digitizer->cfg().acq();
  if (enabled) {
    acq.collection.setDuration(this->ui->timedRunValue->value());
    this->ui->timedRunValue->setEnabled(true);
  } else {
    acq.collection.disableDuration();
    this->ui->timedRunValue->setEnabled(false);
  }
}
void PrimaryControls::changeRunDuration(int duration) {
  auto &acq = this->digitizer->cfg().acq();
  if (this->ui->timedRunCheckbox->isChecked()) {
    acq.collection.setDuration(duration);
  } else {
    acq.collection.disableDuration();
  }
}
void PrimaryControls::reloadUI() {
  auto &acq = this->digitizer->cfg().acq();
  this->resetFillIndicators();
  this->ui->notificationsTextArea->clear();
  if (acq.collection.duration()) {
    this->ui->timedRunCheckbox->setChecked(true);
    this->ui->timedRunValue->setEnabled(true);
    this->ui->timedRunValue->setValue(this->digitizer->getDuration());
  } else {
    this->ui->timedRunCheckbox->setChecked(false);
    this->ui->timedRunValue->setEnabled(false);
  }
}
void PrimaryControls::resetFillIndicators() {
  this->ui->dmaUsage->setValue(0);
  this->ui->FileFillStatus->setValue(0);
  this->ui->RAMFillStatus->setValue(0);
}

void PrimaryControls::dumpAppConfig() {
  this->digitizer->cfg()
  QFile file("last_app_config.json");
  file.open(QFile::OpenModeFlag::WriteOnly);
  QJsonDocument doc;
  QJsonObject cfg = this->context->config->toJson();
  QJsonObject acq = this->context->digitizer->getAcquisition().toJson();
  cfg.insert("acquisition", acq);
  doc.setObject(cfg);
  file.write(doc.toJson());
  file.close();
}

void PrimaryControls::primaryButtonClicked() {
  switch (this->digitizer->getDigitizerState()) {
  case Digitizer::DIGITIZER_STATE::ACTIVE:
  case Digitizer::DIGITIZER_STATE::STABILIZING:
    this->ui->streamStartStopButton->setEnabled(false);
    this->digitizer->stopAcquisition();
    break;
  case Digitizer::DIGITIZER_STATE::READY:
    this->ui->streamStartStopButton->setText("STOP");
    this->ui->streamStatusLabel->setText("STARTING");
    this->ui->streamStartStopButton->setEnabled(false);
    this->dumpAppConfig();
    this->digitizer->runAcquisition();
    break;
  case Digitizer::DIGITIZER_STATE::STOPPING:
  case Digitizer::DIGITIZER_STATE::STARTING:

    break;
  }
}

void PrimaryControls::digitizerStateChanged(Digitizer::DIGITIZER_STATE state) {
  switch (state) {
  case Digitizer::DIGITIZER_STATE::ACTIVE:
    this->ui->streamStartStopButton->setEnabled(true);
    this->ui->streamStatusLabel->setText("ACTIVE");
    this->ui->streamStartStopButton->setText("STOP");
    this->periodicUpdateTimer.start(this->config->getPeriodicUpdatePeriod());
    break;
  case Digitizer::DIGITIZER_STATE::READY:
    this->ui->streamStartStopButton->setEnabled(true);
    this->ui->streamStatusLabel->setText("READY");
    this->periodicUpdateTimer.stop();
    this->ui->streamStartStopButton->setText("START");
    this->resetFillIndicators();
    break;
  case Digitizer::DIGITIZER_STATE::STOPPING:
    this->ui->streamStartStopButton->setEnabled(false);
    this->ui->streamStatusLabel->setText("STOPPING");
    break;
  case Digitizer::DIGITIZER_STATE::STARTING:
    this->ui->streamStartStopButton->setEnabled(false);
    this->ui->streamStatusLabel->setText("STARTING");
    break;
  case Digitizer::DIGITIZER_STATE::STABILIZING:
    this->ui->streamStartStopButton->setEnabled(true);
    this->ui->streamStatusLabel->setText("STABILIZING");
    break;
  }
}

void PrimaryControls::periodicUIUpdate() {
  if (this->context->fileSaver)
    this->ui->FileFillStatus->setValue(
        100 * double(this->context->fileSaver->getProcessedBytes()) /
        this->digitizer->getFileSizeLimit());
  long long starvation = this->digitizer->getMillisFromLastStarve().count();
  this->ui->dmaUsage->setValue(std::min(100LL - starvation, 100LL));
  this->ui->RAMFillStatus->setValue(
      100 * double(this->digitizer->getDeviceRamFillLevel()));
}

void PrimaryControls::enableVolatileSettings(bool enabled) {
  this->ui->timedRunCheckbox->setEnabled(enabled);
  this->ui->timedRunValue->setEnabled(enabled);
}
