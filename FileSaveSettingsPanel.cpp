#include "FileSaveSettingsPanel.h"
#include "DigitizerConstants.h"
#include "ui_FileSaveSettingsPanel.h"

FileSaveSettingsPanel::FileSaveSettingsPanel(QWidget* parent)
    : QWidget(parent), ui(new Ui::FileSaveSettingsPanel)
{
    ui->setupUi(this);
}

FileSaveSettingsPanel::~FileSaveSettingsPanel()
{
    delete ui;
}

void FileSaveSettingsPanel::initialize(ApplicationContext* context)
{
    this->DigitizerGUIComponent::initialize(context);
    this->ui->fileTypeSelector->connect(
        this->ui->fileTypeSelector,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &FileSaveSettingsPanel::changeFileSaveMode);
}
void FileSaveSettingsPanel::changeFileSaveMode(int mode)
{

    AcquisitionConfiguration& acq = this->context->digitizer->cfg().acq();
    FileSaveSettingsPanel::FileSaveSelectorOptions opt =
        static_cast<FileSaveSettingsPanel::FileSaveSelectorOptions>(mode);
    switch (opt)
    {
    case FileSaveSettingsPanel::FileSaveSelectorOptions::DISABLED: {
        acq.storage.setEnabled(false);
        acq.storage.setStoreHeaders(false);
    }
    break;
    case FileSaveSettingsPanel::FileSaveSelectorOptions::BINARY: {
        acq.storage.setEnabled(true);
        acq.storage.setStoreHeaders(false);
    }
    break;
    case FileSaveSettingsPanel::FileSaveSelectorOptions::BINARY_HEADERS: {
        acq.storage.setEnabled(true);
        acq.storage.setStoreHeaders(true);
    }
    break;
    }
}

void FileSaveSettingsPanel::reloadUI()
{
    AcquisitionConfiguration& acq = this->context->digitizer->cfg().acq();
    FileSaveSettingsPanel::FileSaveSelectorOptions opt;
    if (!acq.storage.enabled())
    {
        opt = FileSaveSettingsPanel::FileSaveSelectorOptions::DISABLED;
    }
    else if (acq.storage.storeHeaders())
    {
        opt = FileSaveSettingsPanel::FileSaveSelectorOptions::BINARY_HEADERS;
    }
    else
    {
        opt = FileSaveSettingsPanel::FileSaveSelectorOptions::BINARY;
    }
    this->ui->fileTypeSelector->setCurrentIndex(static_cast<int>(opt));
}
void FileSaveSettingsPanel::onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns)
{
    if (ns == AcquisitionStates::INACTIVE)
    {
        this->enableUnsafeADQControls(true);
    }
    else if (ns == AcquisitionStates::STARTING)
    {
        this->enableUnsafeADQControls(false);
    }
}
void FileSaveSettingsPanel::enableUnsafeADQControls(bool en)
{
    this->ui->fileTypeSelector->setEnabled(en);
}