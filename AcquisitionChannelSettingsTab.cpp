#include "AcquisitionChannelSettingsTab.h"
#include "Digitizer.h"
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
#include "ui_AcquisitionChannelSettingsTab.h"
#include "util.h"
#include <QRadioButton>

//

AcquisitionChannelSettingsTab::AcquisitionChannelSettingsTab(int index,
                                                             QWidget *parent)
    : QWidget(parent), DigitizerGUIComponent(), channel(index),
      ui(new Ui::AcquisitionChannelSettingsTab) {
  ui->setupUi(this);
  this->ui->channelActive->connect(
      this->ui->channelActive, &QCheckBox::stateChanged, this, [=](int state) {
        emit this->channelActiveChanged(state ? true : false);
      });
  this->ui->useTriggerSource->connect(
      this->ui->useTriggerSource, &QCheckBox::stateChanged, this,
      [=](int state) {
        emit this->triggerActiveChanged(state ? true : false);
      });
  this->ui->triggerOffsetFromZero->connect(
      this->ui->triggerOffsetFromZero, &QRadioButton::clicked, this,
      [=]() { this->setOffsetSource(true); });
  this->ui->triggerOffsetFromBias->connect(
      this->ui->triggerOffsetFromBias, &QRadioButton::clicked, this,
      [=]() { this->setOffsetSource(false); });
  this->ui->triggerLevelCode->connect(
      this->ui->triggerLevelCode,
      static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
      &AcquisitionChannelSettingsTab::setTriggerLevel);
  this->ui->triggerLevelMv->connect(
      this->ui->triggerLevelMv,
      static_cast<void (QDoubleSpinBox::*)(double)>(
          &QDoubleSpinBox::valueChanged),
      this, &AcquisitionChannelSettingsTab::setTriggerLevelMv);
  this->ui->triggerEdge->connect(
      this->ui->triggerEdge,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &AcquisitionChannelSettingsTab::setTriggerEdge);
  this->ui->triggerReset->connect(
      this->ui->triggerReset,
      static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
      &AcquisitionChannelSettingsTab::setTriggerReset);
  this->ui->inputRange->connect(
      this->ui->inputRange,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &AcquisitionChannelSettingsTab::setInputRange);
  this->invalidateTriggerLevels();
  this->invalidateDCOffset();
  this->ui->dcOffsetCode->connect(
      this->ui->dcOffsetCode,
      static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
      &AcquisitionChannelSettingsTab::setDCOffset);
  this->ui->dcOffsetMv->connect(this->ui->dcOffsetMv,
                                static_cast<void (QDoubleSpinBox::*)(double)>(
                                    &QDoubleSpinBox::valueChanged),
                                this,
                                &AcquisitionChannelSettingsTab::setDCOffsetMv);
  this->connect(&this->digitizer, &Digitizer::acquisitionStateChanged, this,
                [this](AcquisitionStates os, AcquisitionStates ns) {
                  this->onAcquisitionStateChanged(os, ns);
                });
}

void AcquisitionChannelSettingsTab::setOffsetSource(bool fromZero) {
  this->context.config()->app().offsetTriggerFromZero = fromZero;
  this->invalidateTriggerLevels();
}

void AcquisitionChannelSettingsTab::setTriggerLevel(int val) {
  auto &acq = this->context.config()->acq();
  auto &afe = acq.AFEs.at(this->channel);
  bool offsetFromZero = this->context.config()->app().offsetTriggerFromZero;
  this->ui->triggerLevelCode->blockSignals(true);
  this->ui->triggerLevelMv->blockSignals(true);

  double channelInputRange = afe.obtainedInputRange();
  int bias = afe.dcBias();
  int relative, absolute;

  if (offsetFromZero) {
    absolute = val;
    relative = absolute - bias;
  } else {
    relative = val;
    absolute = relative + bias;
  }
  double absoluteMv = codeToMv(absolute, channelInputRange);
  double relativeMv = codeToMv(relative, channelInputRange);
  acq.triggers.at(this->channel).setLevel(absolute);
  if (offsetFromZero) {
    this->ui->triggerLevelMv->setValue(absoluteMv);
  } else {
    this->ui->triggerLevelMv->setValue(relativeMv);
  }
  this->ui->triggerLevelCodeAbsolute->setText(
      QString::fromStdString(fmt::format("{}", absolute)));
  this->ui->triggerLevelMvAbsolute->setText(
      QString::fromStdString(fmt::format("{:.2f} mV", absoluteMv)));

  this->ui->triggerLevelCode->blockSignals(false);
  this->ui->triggerLevelMv->blockSignals(false);
}

void AcquisitionChannelSettingsTab::setTriggerLevelMv(double d) {
  auto &acq = this->context.config()->acq();
  auto &afe = acq.AFEs.at(this->channel);
  bool offsetFromZero = this->context.config()->app().offsetTriggerFromZero;
  this->ui->triggerLevelCode->blockSignals(true);
  this->ui->triggerLevelMv->blockSignals(true);
  double channelInputRange = afe.obtainedInputRange();
  int bias = afe.dcBias();
  double biasMv = codeToMv(bias, channelInputRange);
  double relativeMv, absoluteMv;
  if (offsetFromZero) {
    absoluteMv = d;
    relativeMv = absoluteMv - biasMv;
  } else {
    relativeMv = d;
    absoluteMv = relativeMv + biasMv;
  }
  int absolute = mvToCode(absoluteMv, channelInputRange);
  int relative = mvToCode(relativeMv, channelInputRange);
  if (offsetFromZero) {
    this->ui->triggerLevelCode->setValue(absolute);
  } else {
    this->ui->triggerLevelCode->setValue(relative);
  }
  acq.triggers.at(this->channel).setLevel(absolute);
  this->ui->triggerLevelCodeAbsolute->setText(
      QString::fromStdString(fmt::format("{}", absolute)));
  this->ui->triggerLevelMvAbsolute->setText(
      QString::fromStdString(fmt::format("{:.2f} mV", absoluteMv)));

  this->ui->triggerLevelCode->blockSignals(false);
  this->ui->triggerLevelMv->blockSignals(false);
}

void AcquisitionChannelSettingsTab::setDCOffset(int val) {
  auto &acq = this->context.config()->acq();
  auto &afe = acq.AFEs.at(this->channel);
  this->ui->dcOffsetCode->blockSignals(true);
  this->ui->dcOffsetMv->blockSignals(true);
  afe.setDcBias(val);
  double channelInputRange = afe.obtainedInputRange();
  int bias = afe.dcBias();
  double biasmv = codeToMv(bias, channelInputRange);
  this->ui->dcOffsetCode->setValue(bias);
  this->ui->dcOffsetMv->setValue(biasmv);
  this->ui->approximateOffset->setText(
      QString::fromStdString(fmt::format("{:.2f} mV", biasmv)));
  this->ui->dcOffsetCode->blockSignals(false);
  this->ui->dcOffsetMv->blockSignals(false);
  this->invalidateTriggerLevels();
}

void AcquisitionChannelSettingsTab::setDCOffsetMv(double mv) {
  auto &acq = this->context.config()->acq();
  auto &afe = acq.AFEs.at(this->channel);
  this->ui->dcOffsetCode->blockSignals(true);
  this->ui->dcOffsetMv->blockSignals(true);
  double channelInputRange = afe.obtainedInputRange();
  int bias = mvToCode(mv, channelInputRange);
  afe.setDcBias(bias);
  this->ui->dcOffsetCode->setValue(bias);
  double biasmv = codeToMv(bias, channelInputRange);
  this->ui->approximateOffset->setText(
      QString::fromStdString(fmt::format("{:.2f} mV", biasmv)));
  this->ui->dcOffsetCode->blockSignals(false);
  this->ui->dcOffsetMv->blockSignals(false);
  this->invalidateTriggerLevels();
}

void AcquisitionChannelSettingsTab::invalidateTriggerLevels() {
  auto &acq = this->context.config()->acq();
  auto &afe = acq.AFEs.at(this->channel);
  auto &trig = acq.triggers.at(this->channel);
  int absolute, relative, bias;
  double absoluteMv, relativeMv, biasMv;
  double channelInputRange = afe.obtainedInputRange();
  bool offsetFromZero = this->context.config()->app().offsetTriggerFromZero;
  absolute = trig.level();
  bias = afe.dcBias();
  relative = absolute - bias;
  absoluteMv = codeToMv(absolute, channelInputRange);
  biasMv = codeToMv(bias, channelInputRange);
  relativeMv = absoluteMv - biasMv;
  this->ui->triggerLevelCode->blockSignals(true);
  this->ui->triggerLevelMv->blockSignals(true);
  if (offsetFromZero) {
    this->ui->triggerLevelCode->setValue(absolute);
    this->ui->triggerLevelMv->setValue(absoluteMv);
  } else {
    this->ui->triggerLevelCode->setValue(relative);
    this->ui->triggerLevelMv->setValue(relativeMv);
  }
  this->ui->triggerLevelCodeAbsolute->setText(
      QString::fromStdString(fmt::format("{}", absolute)));
  this->ui->triggerLevelMvAbsolute->setText(
      QString::fromStdString(fmt::format("{:.2f} mV", absoluteMv)));
  this->ui->triggerLevelCode->blockSignals(false);
  this->ui->triggerLevelMv->blockSignals(false);
}

void AcquisitionChannelSettingsTab::invalidateDCOffset() {
  auto &acq = this->context.config()->acq();
  auto &afe = acq.AFEs.at(this->channel);
  this->ui->dcOffsetCode->blockSignals(true);
  this->ui->dcOffsetMv->blockSignals(true);
  double channelInputRange = afe.obtainedInputRange();
  int dcbias = afe.dcBias();
  this->ui->dcOffsetCode->setValue(dcbias);
  this->ui->dcOffsetMv->setValue(codeToMv(dcbias, channelInputRange));
  this->ui->dcOffsetCode->blockSignals(false);
  this->ui->dcOffsetMv->blockSignals(false);
}

int AcquisitionChannelSettingsTab::inputRangeToSelectBoxPosition(
    int inputRange) {
  INPUT_RANGES inrange = INPUT_RANGES::MV_10000;
  switch (inputRange) {
  case 100:
    inrange = INPUT_RANGES::MV_100;
    break;
  case 250:
    inrange = INPUT_RANGES::MV_250;
    break;
  case 500:
    inrange = INPUT_RANGES::MV_500;
    break;
  case 1000:
    inrange = INPUT_RANGES::MV_1000;
    break;
  case 2000:
    inrange = INPUT_RANGES::MV_2000;
    break;
  case 5000:
    inrange = INPUT_RANGES::MV_5000;
    break;
  case 10000:
    inrange = INPUT_RANGES::MV_10000;
    break;
  default:
    spdlog::error("Input range {} not supported by the UI.", inputRange);
    break;
  }
  return static_cast<int>(inrange);
}

AcquisitionChannelSettingsTab::~AcquisitionChannelSettingsTab() { delete ui; }

void AcquisitionChannelSettingsTab::reloadUI() {
  auto &acq = this->context.config()->acq();
  auto &afe = acq.AFEs.at(this->channel);
  auto &col = acq.collection;
  auto &trig = acq.triggers.at(this->channel);
  bool offsetFromZero = this->context.config()->app().offsetTriggerFromZero;
  this->ui->triggerEdge->setCurrentIndex(static_cast<int>(trig.edge()));
  if (isBitSet(this->channel, col.channelMask())) {
    this->setChannelActive(true);
  }
  if (isBitSet(this->channel, col.triggerMask())) {
    this->setTriggerActive(true);
  }
  if (offsetFromZero) {
    this->ui->triggerOffsetFromZero->setChecked(true);
  } else {
    this->ui->triggerOffsetFromBias->setChecked(true);
  }
  this->invalidateTriggerLevels();
  this->ui->inputRange->setCurrentIndex(
      this->inputRangeToSelectBoxPosition(afe.desiredInputRange()));
  this->setDCOffset(afe.dcBias());
  this->ui->triggerReset->setValue(trig.reset());
}

void AcquisitionChannelSettingsTab::setTriggerChangeAllowed(bool allowed) {
  this->allowTriggerChange = allowed;
  this->ui->useTriggerSource->setEnabled(allowed);
}
void AcquisitionChannelSettingsTab::setActiveChangeAllowed(bool allowed) {
  this->allowAcqChange = allowed;
  this->ui->channelActive->setEnabled(allowed);
}
void AcquisitionChannelSettingsTab::setChannelActive(bool act) {
  this->ui->channelActive->blockSignals(true);
  this->ui->channelActive->setChecked(act);
  this->ui->channelActive->blockSignals(false);
}

void AcquisitionChannelSettingsTab::setTriggerActive(bool act) {
  this->ui->useTriggerSource->blockSignals(true);
  this->ui->useTriggerSource->setChecked(act);
  this->ui->useTriggerSource->blockSignals(false);
}

void AcquisitionChannelSettingsTab::setObtainedRange(double val) {
  this->ui->obtainedInputRange->setText(
      QString::fromStdString(fmt::format("{:.2f} mV", val)));
}

void AcquisitionChannelSettingsTab::enableAcquisitionSettings(bool enabled) {
  this->ui->AFE->setEnabled(enabled);
  this->ui->trigger->setEnabled(enabled);
  this->ui->inputRange->setEnabled(enabled);
  this->ui->channelActive->setEnabled(enabled ? this->allowAcqChange : false);
  this->ui->useTriggerSource->setEnabled(enabled ? this->allowTriggerChange
                                                 : false);
}
void AcquisitionChannelSettingsTab::onAcquisitionStateChanged(
    AcquisitionStates os, AcquisitionStates ns) {
  switch (ns) {
  case AcquisitionStates::STARTING:
    this->enableAcquisitionSettings(false);
    break;
  case AcquisitionStates::INACTIVE:
    this->enableAcquisitionSettings(true);
    break;
  default:
    break;
  }
}

void AcquisitionChannelSettingsTab::setTriggerEdge(int edge)
{
  auto &acq = this->context.config()->acq();
  acq.triggers.at(this->channel).setEdge(static_cast<TRIGGER_EDGES>(edge));
}
void AcquisitionChannelSettingsTab::setTriggerReset(int reset)
{
  auto &acq = this->context.config()->acq();
  acq.triggers.at(this->channel).setReset(reset);
}

void AcquisitionChannelSettingsTab::setInputRange(int irEnum)
{
  auto &acq = this->context.config()->acq();
  acq.AFEs.at(this->channel).setDesiredInputRange(inputRangeEnumToValue(irEnum));
}