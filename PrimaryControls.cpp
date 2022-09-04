#include "PrimaryControls.h"
#include "DigitizerConstants.h"
#include "ui_PrimaryControls.h"
#include <algorithm>

PrimaryControls::PrimaryControls(QWidget *parent)
    : QWidget(parent), ui(new Ui::PrimaryControls) {
  ui->setupUi(this);
  // Create and append the log sink for printing logs to the UI
  this->logSink = std::shared_ptr<QGUILogSink_mt>(
      new QGUILogSink_mt(this->ui->notificationsTextArea));
  this->logSink->set_pattern(LOGGER_PATTERN);
  this->context.logger()->sinks().push_back(this->logSink);

  // React to acquisition state changes
  this->digitizer.connect(&this->digitizer, &Digitizer::acquisitionStateChanged,
                          this,
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
  connect(this->ui->swTrig, &QAbstractButton::clicked, this, &PrimaryControls::SWTrig);
  this->ui->timedRunValue->connect(
      this->ui->timedRunValue,
      static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
      &PrimaryControls::changeRunDuration);
  this->periodicUpdateTimer.connect(&this->periodicUpdateTimer,
                                    &QTimer::timeout, this,
                                    &PrimaryControls::periodicUIUpdate);
  this->reloadUI();
}

PrimaryControls::~PrimaryControls() {
  this->periodicUpdateTimer.stop();
  delete ui;
}
void PrimaryControls::enableTimedRun(int enabled) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  if (enabled) {
    acq.collection.setDuration(this->ui->timedRunValue->value());
    this->ui->timedRunValue->setEnabled(true);
  } else {
    acq.collection.disableDuration();
    this->ui->timedRunValue->setEnabled(false);
  }
}
void PrimaryControls::changeRunDuration(int duration) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  if (this->ui->timedRunCheckbox->isChecked()) {
    acq.collection.setDuration(duration);
  } else {
    acq.collection.disableDuration();
  }
}
void PrimaryControls::reloadUI() {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  this->resetFillIndicators();
  this->ui->notificationsTextArea->clear();
  if (acq.collection.duration()) {
    this->ui->timedRunCheckbox->setChecked(true);
    this->ui->timedRunValue->setEnabled(true);
    this->ui->timedRunValue->setValue(acq.collection.duration());
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
  this->context.config()->saveToFile("last_app_config.json");
}

void PrimaryControls::primaryButtonClicked() {
  switch (this->digitizer.state()) {
  case AcquisitionStates::ACTIVE:
  case AcquisitionStates::STARTING:
    this->ui->streamStartStopButton->setEnabled(false);
    this->digitizer.stopAcquisition();
    break;
  case AcquisitionStates::INACTIVE:
    this->ui->streamStartStopButton->setText("STOP");
    this->ui->streamStatusLabel->setText("STARTING");
    this->ui->streamStartStopButton->setEnabled(false);
    this->dumpAppConfig();
    this->digitizer.startAcquisition();
    break;
  default:
    break;
  }
}

void PrimaryControls::onAcquisitionStateChanged(AcquisitionStates os,
                                                AcquisitionStates ns) {
  switch (ns) {
  case AcquisitionStates::ACTIVE:
    this->ui->streamStartStopButton->setEnabled(true);
    this->ui->streamStatusLabel->setText("ACTIVE");
    this->ui->streamStartStopButton->setText("STOP");
    this->periodicUpdateTimer.start(this->context.config()->app().periodicUpdatePeriod);
    break;
  case AcquisitionStates::INACTIVE:
    this->ui->streamStartStopButton->setEnabled(true);
    this->ui->streamStatusLabel->setText("READY");
    this->periodicUpdateTimer.stop();
    this->ui->streamStartStopButton->setText("START");
    this->resetFillIndicators();
    break;
  case AcquisitionStates::STOPPING:
    this->ui->streamStartStopButton->setEnabled(false);
    this->ui->streamStatusLabel->setText("STOPPING");
    break;
  case AcquisitionStates::STARTING:
    this->ui->streamStartStopButton->setEnabled(true);
    this->ui->streamStatusLabel->setText("STARTING");
    break;
  }
}

void PrimaryControls::periodicUIUpdate() {
  /*
  if (this->context->fileSaver)
    this->ui->FileFillStatus->setValue(
        100 * double(this->context->fileSaver->getProcessedBytes()) /
        this->digitizer->getFileSizeLimit());
        */
  this->ui->dmaUsage->setValue(this->digitizer.dmaUsage() * 100.0);
  this->ui->RAMFillStatus->setValue(100.0 * this->digitizer.ramFill());
}

void PrimaryControls::enableAcquisitionSettings(bool enabled) {
  this->ui->timedRunCheckbox->setEnabled(enabled);
  this->ui->timedRunValue->setEnabled(enabled);
}

void PrimaryControls::SWTrig() {
 this->digitizer.SWTrig();
}
