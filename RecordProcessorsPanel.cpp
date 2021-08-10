#include "RecordProcessorsPanel.h"
#include "ui_RecordProcessorsPanel.h"
#include "BinaryFileWriter.h"
RecordProcessorsPanel::RecordProcessorsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordProcessorsPanel)
{
    ui->setupUi(this);
}
void RecordProcessorsPanel::initialize(ApplicationContext *context)
{
    this->DigitizerGUIComponent::initialize(context);
    this->context = context;
    this->scopeUpdater = context->scopeUpdater;
    this->autosetFileSaver();
    this->ui->fileTypeSelector->connect(
        this->ui->fileTypeSelector,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [=](int index){
            this->config->setFileSaveMode(static_cast<ApplicationConfiguration::FILE_SAVE_MODES>(index));
            this->autosetFileSaver();
        }
    );
    this->ui->updateScopeCB->connect(
        this->ui->updateScopeCB, &QCheckBox::stateChanged,
        [=](int state) {
            this->config->setUpdateScopeEnabled(state?true:false);
            this->autosetUpdateScope();
        }
    );
}

void RecordProcessorsPanel::reloadUI()
{
    this->ui->fileTypeSelector->setCurrentIndex(this->config->getFileSaveMode());
    this->ui->updateScopeCB->setChecked(this->config->getUpdateScopeEnabled());
}

void RecordProcessorsPanel::autosetFileSaver()
{
    if(this->fileSaver)
    {
        this->digitizer->removeRecordProcessor(this->fileSaver.get());
    }
    switch(this->config->getFileSaveMode())
    {
        case ApplicationConfiguration::FILE_SAVE_MODES::DISABLED:
            this->fileSaver.reset();
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BINARY:
            this->fileSaver = std::unique_ptr<FileWriter>(new BinaryFileWriter(this->digitizer->getFileSizeLimit()));
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BINARY_VERBOSE:
            this->fileSaver = std::unique_ptr<FileWriter>(new VerboseBinaryWriter(this->digitizer->getFileSizeLimit()));
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BUFFERED_BINARY:
            this->fileSaver = std::unique_ptr<FileWriter>(new BufferedBinaryFileWriter(this->digitizer->getFileSizeLimit()));
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BUFFERED_BINARY_VERBOSE:
            this->fileSaver = std::unique_ptr<FileWriter>(new VerboseBufferedBinaryWriter(this->digitizer->getFileSizeLimit()));
        break;
    }
    if(this->fileSaver)
    {
        this->digitizer->appendRecordProcessor(this->fileSaver.get());
    }
    this->context->fileSaver = this->fileSaver.get();
}

void RecordProcessorsPanel::autosetUpdateScope()
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
RecordProcessorsPanel::~RecordProcessorsPanel()
{
    delete ui;
}

