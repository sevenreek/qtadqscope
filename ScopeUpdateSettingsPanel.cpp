#include "ScopeUpdateSettingsPanel.h"
#include "ScopeUpdater.h"
#include "ui_ScopeUpdateSettingsPanel.h"

ScopeUpdateSettingsPanel::ScopeUpdateSettingsPanel(QWidget *parent)
    : QWidget(parent), ui(new Ui::ScopeUpdateSettingsPanel) {
  ui->setupUi(this);
  this->scopeUpdater = std::unique_ptr<ScopeUpdater>(new ScopeUpdater(0));
  this->connect(this->ui->updateScopeCB, &QCheckBox::stateChanged, this,
                &ScopeUpdateSettingsPanel::setUpdateScope);
  this->connect(
      this->ui->plotChannel,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &ScopeUpdateSettingsPanel::changePlotChannel);

  this->connect(
      &this->digitizer, &Digitizer::acquisitionStateChanged, this,
      [this](auto os, auto ns) { this->onAcquisitionStateChanged(os, ns); });
}

ScopeUpdateSettingsPanel::~ScopeUpdateSettingsPanel() { delete ui; }

void ScopeUpdateSettingsPanel::setUpdateScope(int enable) {
  this->context.config()->app().updateScopeEnabled = enable;
  this->digitizer.removeRecordProcessor(this->scopeUpdater.get());
  if (enable) {
    this->digitizer.appendRecordProcessor(this->scopeUpdater.get());
  }
}
void ScopeUpdateSettingsPanel::reloadUI() {
  this->ui->updateScopeCB->setChecked(
      this->context.config()->app().updateScopeEnabled);
}

void ScopeUpdateSettingsPanel::changePlotChannel(int ch) {
  this->scopeUpdater->changeChannel(ch);
}
void ScopeUpdateSettingsPanel::enableAcquisitionSettings(bool en) {
  // no dangerous settings in this panel
}
void ScopeUpdateSettingsPanel::onAcquisitionStateChanged(AcquisitionStates os,
                                                         AcquisitionStates ns) {
  // do nothing for now
}
/*
void ScopeUpdateSettingsPanel::allowChangePlotChannel(bool allow)
{
    this->ui->plotChannel->setEnabled(allow);
}

void ScopeUpdateSettingsPanel::changePlotChannel(int ch)
{
    if(this->digitizer->getTriggerApproach() == TRIGGER_APPROACHES::SINGLE)
        this->ui->plotChannel->setCurrentIndex(ch);
}
*/