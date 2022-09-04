#include "AcquisitionSettingsSidePanel.h"

#include <cmath>

#include "AcquisitionConfiguration.h"
#include "ConfigurationController.h"
#include "Digitizer.h"
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
#include "spdlog/fmt/fmt.h"
#include "ui_AcquisitionSettingsSidePanel.h"
#include "util.h"

AcquisitionSettingsSidePanel::AcquisitionSettingsSidePanel(QWidget *parent)
    : QWidget(parent), DigitizerGUIComponent(),
      ui(new Ui::AcquisitionSettingsSidePanel) {
  ui->setupUi(this);
  this->createChannelSettingsTabs();
  // Connect UI slots
  this->connect(this->ui->bypassUL1, &QCheckBox::stateChanged, this,
                &AcquisitionSettingsSidePanel::setUL1Bypass);
  this->connect(this->ui->bypassUL2, &QCheckBox::stateChanged, this,
                &AcquisitionSettingsSidePanel::setUL2Bypass);
  this->connect(this->ui->horizontalShift,
                static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                this, &AcquisitionSettingsSidePanel::setHorizontalShift);
  this->connect(this->ui->recordLength,
                static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                this, &AcquisitionSettingsSidePanel::setRecordLength);
  this->connect(this->ui->recordCount,
                static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                this, &AcquisitionSettingsSidePanel::setRecordCount);
  this->connect(this->ui->sampleSkip,
                static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                this, &AcquisitionSettingsSidePanel::setSampleSkip);
  this->connect(this->ui->limitRecords, &QCheckBox::stateChanged, this,
                &AcquisitionSettingsSidePanel::setLimitRecordsEnabled);
  this->connect(
      this->ui->triggerMode,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &AcquisitionSettingsSidePanel::setTriggerMode);
  this->connect(this->ui->channelTabs, &QTabWidget::currentChanged, this,
                &AcquisitionSettingsSidePanel::handleTabChanged);
  this->connect(this->ui->acquisitionTag, &QLineEdit::textChanged, this,
                &AcquisitionSettingsSidePanel::setAcquisitionTag);
  this->connect(&this->digitizer, &Digitizer::acquisitionStateChanged, this,
                &AcquisitionSettingsSidePanel::onAcquisitionStateChanged);
}

AcquisitionSettingsSidePanel::~AcquisitionSettingsSidePanel() {
  this->ui->channelTabs->disconnect(this);
  delete ui;
}

void AcquisitionSettingsSidePanel::reloadUI() {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  this->ui->bypassUL1->setChecked(acq.spectroscope.isBypassed(UserLogic::UL1));
  this->ui->bypassUL2->setChecked(acq.spectroscope.isBypassed(UserLogic::UL2));
  this->ui->acquisitionTag->setText(QString::fromStdString(acq.storage.tag()));
  auto horizontalShift = acq.triggers.at(0).horizontalShift();
  this->ui->horizontalShift->setValue(horizontalShift);
  if (acq.collection.isContinuous()) {
    this->ui->triggerMode->setCurrentIndex(0); // force set software
  } else {
    this->ui->triggerMode->setCurrentIndex(triggerModeToSelectIndex(
        acq.triggers.at(0).mode())); // set depending on the config
  }

  this->ui->sampleSkip->setValue(acq.collection.sampleSkip());
  this->ui->frequency->setText(QString::fromStdString(this->calculateFrequency(
      MAX_SAMPLING_RATE, acq.collection.sampleSkip())));
  this->ui->recordLength->setValue(acq.records.at(0).recordLength());

  if (acq.records.at(0).isInfinite()) {
    this->ui->recordCount->setValue(0);
    this->ui->limitRecords->setChecked(false);
    this->ui->recordCount->setEnabled(false);
  }
  if (isOnlyOneBitSet(acq.collection.channelMask())) // switch tab to the
                                                     // only active channel
  {
    int index = 0;
    for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++) {
      if (acq.collection.channelMask() & (1 << ch)) {
        index = ch;
        break;
      }
    }
    this->ui->channelTabs->setCurrentIndex(index);
  }
  for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++) {
    tabs.at(ch)->reloadUI();
  }
  this->changeTabNames();

  // call sublayouts to reload
  this->ui->fileSavePanel->reloadUI();
  this->ui->scopeUpdatePanel->reloadUI();
}

void AcquisitionSettingsSidePanel::createChannelSettingsTabs() {
  for (int ch = 0; ch < MAX_NOF_CHANNELS; ch++) {
    tabs.at(ch) = std::unique_ptr<AcquisitionChannelSettingsTab>(
        new AcquisitionChannelSettingsTab(ch));
    this->ui->channelTabs->addTab(
        tabs.at(ch).get(), QString::fromStdString(fmt::format("CH{}", ch + 1)));
  }
}

std::string AcquisitionSettingsSidePanel::calculateFrequency(
    unsigned long long samplingRate, unsigned long long sampleSkip) {
  int timesDivided = 0;
  double sr = double(samplingRate) / sampleSkip;
  while (sr >= 1000) {
    sr /= 1000.0;
    timesDivided++;
  }
  return fmt::format("{:.2f} {}Hz", sr, UNIT_PREFIXES[timesDivided]);
}

void AcquisitionSettingsSidePanel::handleTabChanged(int tab) {
  /*
  if (this->digitizer->getTriggerApproach() == TRIGGER_APPROACHES::SINGLE)
  {
      this->tabs.at(this->lastActiveChannel)->setChannelActive(false);
      this->tabs.at(this->lastActiveChannel)->setTriggerActive(false);
      this->tabs.at(tab)->setChannelActive(true);
      this->tabs.at(tab)->setTriggerActive(true);
      this->handleSetChannelActive(tab, true);
      this->handleSetChannelTriggerActive(tab, true);
  }
  */
  this->tabs.at(tab)->reloadUI();
  emit this->onChannelTabChanged(tab);
}
/*
void AcquisitionSettingsSidePanel::handleSetChannelActive(int channel, bool
active)
{
    if (this->digitizer->getTriggerApproach() == TRIGGER_APPROACHES::SINGLE)
    {
        this->digitizer->setChannelMask((1 << channel));
    }
    else
    {
        if (active)
            this->digitizer->setChannelMask(this->digitizer->getChannelMask() |
(1 << channel)); else
            this->digitizer->setChannelMask(this->digitizer->getChannelMask() &
~(1 << channel));
    }
    this->handleTabNameChange(channel, true);
    this->lastActiveChannel = channel;
}

void AcquisitionSettingsSidePanel::handleSetChannelTriggerActive(int channel,
bool active)
{
    if (this->digitizer->getTriggerApproach() == TRIGGER_APPROACHES::SINGLE)
    {
        this->digitizer->setTriggerMask((1 << channel));
    }
    else
    {
        if (active)
            this->digitizer->setTriggerMask(this->digitizer->getTriggerMask() |
(1 << channel)); else
            this->digitizer->setTriggerMask(this->digitizer->getTriggerMask() &
~(1 << channel));
    }

    this->handleTabNameChange(channel, true);
}
*/
void AcquisitionSettingsSidePanel::changeTabNames() {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  int chMask = acq.collection.channelMask();
  int trMask = acq.collection.triggerMask();
  for (int ch = 0; ch < this->ui->channelTabs->count(); ch++) {
    bool active = isBitSet(ch, chMask);
    std::string tabname;
    if (active)
      tabname = fmt::format("CH{}{}{}", ch + 1, CHANNEL_ACTIVE_EMOJI,
                            isBitSet(ch, trMask) ? TRIGGER_ACTIVE_EMOJI
                                                 : ""); /*▶🗲*/
    else
      tabname = fmt::format("CH{}{}", ch + 1,
                            isBitSet(ch, trMask) ? TRIGGER_ACTIVE_EMOJI
                                                 : ""); /*▶🗲*/
    this->ui->channelTabs->setTabText(ch, QString::fromStdString(tabname));
  }
}

void AcquisitionSettingsSidePanel::enableAcquisitionSettings(bool enabled) {
  this->ui->bypassUL1->setEnabled(enabled);
  this->ui->bypassUL2->setEnabled(enabled);
  this->ui->acquisitionTag->setEnabled(enabled);
  this->ui->frequency->setEnabled(enabled);
  this->ui->limitRecords->setEnabled(enabled);
  this->ui->horizontalShift->setEnabled(enabled);
  this->ui->recordCount->setEnabled(enabled);
  this->ui->recordLength->setEnabled(enabled);
  this->ui->sampleSkip->setEnabled(enabled);
  this->ui->triggerMode->setEnabled(enabled);
}
/*
void AcquisitionSettingsSidePanel::handleApproachChanged(int approachi)
{
    this->digitizer->setTriggerApproach(static_cast<TRIGGER_APPROACHES>(approachi));
    TRIGGER_APPROACHES approach = this->digitizer->getTriggerApproach();
    if (approach == TRIGGER_APPROACHES::SINGLE)
    {
        int openChannel = this->ui->channelTabs->currentIndex();
        this->tabs.at(openChannel)->setChannelActive(true);
        this->tabs.at(openChannel)->setTriggerActive(true);
        this->handleSetChannelTriggerActive(openChannel, true);
        this->handleSetChannelActive(openChannel, true);
        for (int ch = 0; ch < this->tabs.size(); ch++)
        {
            this->tabs.at(ch)->setActiveChangeAllowed(false);
            this->tabs.at(ch)->setTriggerChangeAllowed(false);
        }
    }
    else if (approach == TRIGGER_APPROACHES::INDIVIDUAL)
    {
        for (int ch = 0; ch < this->tabs.size(); ch++)
        {
            this->tabs.at(ch)->setActiveChangeAllowed(true);
            this->tabs.at(ch)->setTriggerChangeAllowed(true);
        }
    }
    else
    {
        for (int ch = 0; ch < this->tabs.size(); ch++)
        {
            this->tabs.at(ch)->setActiveChangeAllowed(true);
            this->tabs.at(ch)->setTriggerChangeAllowed(false);
            this->tabs.at(ch)->setTriggerActive(false);
            this->handleSetChannelTriggerActive(ch, false);
        }
        int setChannel = static_cast<int>(approach) - TRIGGER_APPROACHES::CH1;
        this->handleSetChannelTriggerActive(setChannel, true);
        this->tabs.at(setChannel)->setTriggerActive(true);
    }
}
*/

void AcquisitionSettingsSidePanel::setUL1Bypass(bool bypass) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  acq.spectroscope.setBypass(UserLogic::UL1, bypass);
}

void AcquisitionSettingsSidePanel::setUL2Bypass(bool bypass) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  acq.spectroscope.setBypass(UserLogic::UL2, bypass);
}

void AcquisitionSettingsSidePanel::setHorizontalShift(int val) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  acq.triggers.at(0).setHorizontalShift(val);
}

void AcquisitionSettingsSidePanel::setRecordLength(int val) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  acq.records.at(0).setRecordLength(val);
}

void AcquisitionSettingsSidePanel::setRecordCount(int val) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  if (this->ui->limitRecords->isChecked()) {
    acq.records.at(0).setRecordCount(val);
  } else {
    acq.records.at(0).setInfinite();
  }
}

void AcquisitionSettingsSidePanel::setSampleSkip(int val) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  if (val == 3) { // 3 is not supported by ADQ14
    this->ui->sampleSkip->setValue(acq.collection.sampleSkip() == 2 ? 4 : 2);
    return;
  }
  acq.collection.setSampleSkip(val);
  this->ui->frequency->setText(
      QString::fromStdString(this->calculateFrequency(MAX_SAMPLING_RATE, val)));
}

void AcquisitionSettingsSidePanel::setLimitRecordsEnabled(int val) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  if (val) {
    acq.records.at(0).setRecordCount(this->ui->recordCount->value());
    this->ui->recordCount->setEnabled(true);
  } else {
    acq.records.at(0).setInfinite();
    this->ui->recordCount->setEnabled(false);
  }
}

void AcquisitionSettingsSidePanel::setTriggerMode(int val) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  TriggerModeOptions opt =
      static_cast<AcquisitionSettingsSidePanel::TriggerModeOptions>(val);
  switch (opt) {
  case AcquisitionSettingsSidePanel::TriggerModeOptions::CONTINUOUS: {
    acq.triggers.at(0).setMode(TRIGGER_MODES::SOFTWARE);
    acq.triggers.at(1).setMode(TRIGGER_MODES::SOFTWARE);
    acq.triggers.at(2).setMode(TRIGGER_MODES::SOFTWARE);
    acq.triggers.at(3).setMode(TRIGGER_MODES::SOFTWARE);
    acq.collection.setAcquisitionMode(ACQUISITION_MODES::CONTINOUS);
  } break;
  case AcquisitionSettingsSidePanel::TriggerModeOptions::SOFTWARE: {
    acq.triggers.at(0).setMode(TRIGGER_MODES::SOFTWARE);
    acq.triggers.at(1).setMode(TRIGGER_MODES::SOFTWARE);
    acq.triggers.at(2).setMode(TRIGGER_MODES::SOFTWARE);
    acq.triggers.at(3).setMode(TRIGGER_MODES::SOFTWARE);
    acq.collection.setAcquisitionMode(ACQUISITION_MODES::TRIGGERED);
  } break;
  case AcquisitionSettingsSidePanel::TriggerModeOptions::LEVEL: {
    acq.triggers.at(0).setMode(TRIGGER_MODES::LEVEL);
    acq.triggers.at(1).setMode(TRIGGER_MODES::LEVEL);
    acq.triggers.at(2).setMode(TRIGGER_MODES::LEVEL);
    acq.triggers.at(3).setMode(TRIGGER_MODES::LEVEL);
    acq.collection.setAcquisitionMode(ACQUISITION_MODES::TRIGGERED);
  } break;
  case AcquisitionSettingsSidePanel::TriggerModeOptions::INTERNAL: {
    acq.triggers.at(0).setMode(TRIGGER_MODES::INTERNAL);
    acq.triggers.at(1).setMode(TRIGGER_MODES::INTERNAL);
    acq.triggers.at(2).setMode(TRIGGER_MODES::INTERNAL);
    acq.triggers.at(3).setMode(TRIGGER_MODES::INTERNAL);
    acq.collection.setAcquisitionMode(ACQUISITION_MODES::TRIGGERED);
  } break;
  case AcquisitionSettingsSidePanel::TriggerModeOptions::EXTERNAL: {
    acq.triggers.at(0).setMode(TRIGGER_MODES::EXTERNAL);
    acq.triggers.at(1).setMode(TRIGGER_MODES::EXTERNAL);
    acq.triggers.at(2).setMode(TRIGGER_MODES::EXTERNAL);
    acq.triggers.at(3).setMode(TRIGGER_MODES::EXTERNAL);
    acq.collection.setAcquisitionMode(ACQUISITION_MODES::TRIGGERED);
  } break;
  }
}

void AcquisitionSettingsSidePanel::setAcquisitionTag(const QString &val) {
  AcquisitionConfiguration &acq = this->context.config()->acq();
  acq.storage.setTag(val.toStdString());
}

int AcquisitionSettingsSidePanel::triggerModeToSelectIndex(TRIGGER_MODES mode) {
  switch (mode) {
  case TRIGGER_MODES::SOFTWARE:
    return 1;
  case TRIGGER_MODES::EXTERNAL:
    return 2;
  case TRIGGER_MODES::LEVEL:
    return 3;
  case TRIGGER_MODES::INTERNAL:
    return 4;
  case TRIGGER_MODES::EXTERNAL_2:
    return -1;
  case TRIGGER_MODES::EXTERNAL_3:
    return -1;
  }
  return -1;
}

void AcquisitionSettingsSidePanel::onAcquisitionStateChanged(
    AcquisitionStates os, AcquisitionStates ns) {
  switch (ns) {
  case AcquisitionStates::INACTIVE:
    this->enableAcquisitionSettings(true);
    break;
  case AcquisitionStates::STARTING:
  case AcquisitionStates::ACTIVE:
    this->enableAcquisitionSettings(false);
    break;
  default:
    break;
  }
}
ScopeUpdateSettingsPanel *AcquisitionSettingsSidePanel::scopeSettingsPanel() {
  return this->ui->scopeUpdatePanel;
}
