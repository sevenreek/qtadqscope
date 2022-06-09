#include "ScopeUpdateSettingsPanel.h"
#include "ui_ScopeUpdateSettingsPanel.h"

ScopeUpdateSettingsPanel::ScopeUpdateSettingsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScopeUpdateSettingsPanel)
{
    ui->setupUi(this);
}

ScopeUpdateSettingsPanel::~ScopeUpdateSettingsPanel()
{
    delete ui;
}

void ScopeUpdateSettingsPanel::initialize(ApplicationContext *context) 
{
    this->DigitizerGUIComponent::initialize(context);
    this->scopeUpdater = context->scopeUpdater;
    this->ui->updateScopeCB->connect(
        this->ui->updateScopeCB, &QCheckBox::stateChanged,
        [=](int state) {
            this->config->setUpdateScopeEnabled(state?true:false);
            this->autosetUpdateScope();
        }
    );
    this->ui->plotChannel->connect(
        this->ui->plotChannel,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &ScopeUpdateSettingsPanel::onPlotChannelCheckboxChanged
    );
}
void ScopeUpdateSettingsPanel::reloadUI()
{
    this->ui->updateScopeCB->setChecked(this->config->getUpdateScopeEnabled());
}
void ScopeUpdateSettingsPanel::autosetUpdateScope()
{
    if(this->config->getUpdateScopeEnabled() && !this->scopeUpdaterAdded)
    {
        this->digitizer->appendRecordProcessor(this->scopeUpdater);
        this->scopeUpdaterAdded = true;
    }
    else if(this->scopeUpdaterAdded)
    {
        this->digitizer->removeRecordProcessor(this->scopeUpdater);
        this->scopeUpdaterAdded = false;
    }
}

void ScopeUpdateSettingsPanel::onPlotChannelCheckboxChanged(int ch)
{
    this->scopeUpdater->changeChannel(ch);
}
void ScopeUpdateSettingsPanel::enableVolatileSettings(bool en)
{
    // no dangerous settings in this panel
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