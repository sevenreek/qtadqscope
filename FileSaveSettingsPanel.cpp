#include "FileSaveSettingsPanel.h"
#include "ui_FileSaveSettingsPanel.h"

FileSaveSettingsPanel::FileSaveSettingsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileSaveSettingsPanel)
{
    ui->setupUi(this);
}

FileSaveSettingsPanel::~FileSaveSettingsPanel()
{
    delete ui;
}

void FileSaveSettingsPanel::initialize(ApplicationContext *context)
{
    this->DigitizerGUIComponent::initialize(context);
    this->autosetFileSaver();
    this->ui->fileTypeSelector->connect(
        this->ui->fileTypeSelector,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [=](int index){
            this->config->setFileSaveMode(static_cast<ApplicationConfiguration::FILE_SAVE_MODES>(index));
            this->autosetFileSaver();
        }
    );
}

void FileSaveSettingsPanel::enableVolatileSettings(bool en)
{
    this->ui->fileTypeSelector->setEnabled(en);
}
void FileSaveSettingsPanel::reloadUI()
{
    this->ui->fileTypeSelector->setCurrentIndex(this->config->getFileSaveMode());
}

void FileSaveSettingsPanel::autosetFileSaver()
{
    if(this->fileSaver)
    {
        this->digitizer->removeRecordProcessor(this->fileSaver.get());
    }
    switch(this->config->getFileSaveMode())
    {
        default:
            spdlog::error("Unsupported file save mode. Defaulting to disabled.");
        case ApplicationConfiguration::FILE_SAVE_MODES::DISABLED:
            this->fileSaver.reset();
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BINARY:
            this->fileSaver = std::unique_ptr<IRecordProcessor>(new BinaryFileWriter(this->digitizer->getFileSizeLimit()));
        break;
        case ApplicationConfiguration::FILE_SAVE_MODES::BINARY_VERBOSE:
            this->fileSaver = std::unique_ptr<IRecordProcessor>(new VerboseBinaryWriter(this->digitizer->getFileSizeLimit()));
        break;
    }
    if(this->fileSaver)
    {
        this->digitizer->appendRecordProcessor(this->fileSaver.get());
    }
    this->context->fileSaver = this->fileSaver.get();
}