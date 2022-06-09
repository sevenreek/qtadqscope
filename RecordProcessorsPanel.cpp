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
    this->autosetFileSaver();
    this->ui->fileTypeSelector->connect(
        this->ui->fileTypeSelector,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [=](int index){
            this->config->setFileSaveMode(static_cast<ApplicationConfiguration::FILE_SAVE_MODES>(index));
            this->autosetFileSaver();
        }
    );
    this->ui->softwareTrigger->connect(
        this->ui->softwareTrigger, &QAbstractButton::clicked,
        [=]() {
            this->context->digitizer->SWTrig();
        }
    );
}

void RecordProcessorsPanel::reloadUI()
{
    this->ui->fileTypeSelector->setCurrentIndex(this->config->getFileSaveMode());
}

void RecordProcessorsPanel::autosetFileSaver()
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

RecordProcessorsPanel::~RecordProcessorsPanel()
{
    delete ui;
}



void RecordProcessorsPanel::enableVolatileSettings(bool enabled)
{
    this->ui->updateScopeCB->setEnabled(enabled);
    this->ui->fileTypeSelector->setEnabled(enabled);
}
