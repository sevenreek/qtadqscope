#include "BuffersDialog.h"
#include "Digitizer.h"
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
#include "ui_BuffersDialog.h"

// 256 is the maximum buffer count
// buffer size limit is unknown might be 2^24, must be a multiple of 1024
// queue size is limited by user's ram

const BuffersDialog::ConfigPreset
    BuffersDialog::DEFAULT_CONFIG_VALUES[BuffersDialog::DEFAULT_CONFIG_COUNT] =
        {
            {64, 16ul * 1024ul, 256},        // NONE
            {64, 16ul * 1024ul, 256},        // BALANCED
            {256, 4ul * 1024ul, 4ul * 1024}, // SHORT PULSE
            {128, 8ul * 1024ul, 512},        // LONG PULSE
            {64, 1024ul * 1024ul, 512},      // FILE SAVE
            {64, 4ul * 1024ul * 1024ul, 256} // LARGE FILE SAVE
};

BuffersDialog::BuffersDialog(QWidget *parent)
    : QDialog(parent), DigitizerGUIComponent(), ui(new Ui::BuffersDialog) {
  ui->setupUi(this);
  connect(this, &QDialog::accepted, this,
          &BuffersDialog::saveValuesToActiveAcquisition);
  connect(
      this->ui->defaultRestore,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &BuffersDialog::loadPreconfiguredBuffer);
  connect(this->context.digitizer(), &Digitizer::acquisitionStateChanged, this,
          &BuffersDialog::onAcquisitionStateChanged);
}
void BuffersDialog::saveValuesToActiveAcquisition() {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  acq.transfer.setBufferCount(this->ui->dmaBufferCount->value());
  acq.transfer.setBufferSize(this->ui->dmaBufferSize->value());
  acq.transfer.setQueueSize(this->ui->writeBufferCount->value());
  acq.storage.setFileSizeLimit(this->ui->maximumFileSize->value() *
                               FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER);
}
void BuffersDialog::loadPreconfiguredBuffer(int index) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  if (index != 0) {
    acq.transfer.setBufferCount(DEFAULT_CONFIG_VALUES[index].bufferCount);
    acq.transfer.setBufferSize(DEFAULT_CONFIG_VALUES[index].bufferSize);
    acq.transfer.setQueueSize(DEFAULT_CONFIG_VALUES[index].queueSize);
  }
}
double BuffersDialog::FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER = 1e9; // in GBs
BuffersDialog::~BuffersDialog() { delete ui; }

void BuffersDialog::reloadUI() {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  this->ui->dmaBufferCount->setValue(acq.transfer.bufferCount());
  this->ui->dmaBufferSize->setValue(acq.transfer.bufferSize());
  this->ui->writeBufferCount->setValue(acq.transfer.queueSize());
  this->ui->maximumFileSize->setValue(acq.storage.fileSizeLimit() /
                                      FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER);
}
void BuffersDialog::enableAcquisitionSettings(bool enabled) {
  this->setEnabled(enabled);
}

void BuffersDialog::onAcquisitionStateChanged(AcquisitionStates os,
                                              AcquisitionStates ns) {
  switch (ns) {
  case AcquisitionStates::INACTIVE:
    this->enableAcquisitionSettings(true);
    break;
  case AcquisitionStates::STARTING:
    this->enableAcquisitionSettings(false);
    break;
  default:
    break;
  }
}