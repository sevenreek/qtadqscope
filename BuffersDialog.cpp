#include "BuffersDialog.h"
#include "ui_BuffersDialog.h"

// 256 is the maximum buffer count
// buffer size limit is unknown might be 2^24, must be a multiple of 1024
// queue size is limited by user's ram

const BuffersDialog::ConfigPreset BuffersDialog::DEFAULT_CONFIG_VALUES[BuffersDialog::DEFAULT_CONFIG_COUNT]  = {
    (BuffersDialog::ConfigPreset){.bufferCount=64, .bufferSize=4ul*1024ul, .queueSize=256}, // NONE
    (BuffersDialog::ConfigPreset){.bufferCount=64, .bufferSize=4ul*1024ul, .queueSize=256}, // BALANCED
    (BuffersDialog::ConfigPreset){.bufferCount=256, .bufferSize=1ul*1024ul, .queueSize=4ul*1024}, // SHORT PULSE
    (BuffersDialog::ConfigPreset){.bufferCount=128, .bufferSize=2ul*1024ul, .queueSize=512}, // LONG PULSE
    (BuffersDialog::ConfigPreset){.bufferCount=64, .bufferSize=1024ul*1024ul, .queueSize=512}, // FILE SAVE
    (BuffersDialog::ConfigPreset){.bufferCount=64, .bufferSize=4ul*1024ul*1024ul, .queueSize=256} // LARGE FILE SAVE
};

BuffersDialog::BuffersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuffersDialog)
{
    ui->setupUi(this);
}
double BuffersDialog::FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER = 1e9; // in GBs
BuffersDialog::~BuffersDialog()
{
    delete ui;
}

void BuffersDialog::initialize(ApplicationContext *context)
{
    this->DigitizerGUIComponent::initialize(context);
    this->connect(
        this,
        &QDialog::accepted,
        this->digitizer,
        [=]{
            this->digitizer->setTransferBufferCount(this->ui->dmaBufferCount->value());
            this->digitizer->setTransferBufferSize(this->ui->dmaBufferSize->value());
            this->digitizer->setTransferBufferQueueSize(this->ui->writeBufferCount->value());
            this->digitizer->setFileSizeLimit(this->ui->maximumFileSize->value() * FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER);
        }
    );
    connect(
        this->ui->defaultRestore,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        [this](int index) {
            if(index != 0)
            {
                this->ui->dmaBufferCount->setValue(DEFAULT_CONFIG_VALUES[index].bufferCount);
                this->ui->dmaBufferSize->setValue(DEFAULT_CONFIG_VALUES[index].bufferSize);
                this->ui->writeBufferCount->setValue(DEFAULT_CONFIG_VALUES[index].queueSize);
            }
        }
    );
}

void BuffersDialog::reloadUI()
{
    this->ui->dmaBufferCount->setValue(this->digitizer->getTransferBufferCount());
    this->ui->dmaBufferSize->setValue(this->digitizer->getTransferBufferSize());
    this->ui->writeBufferCount->setValue(this->digitizer->getTransferBufferQueueSize());
    this->ui->maximumFileSize->setValue(this->digitizer->getFileSizeLimit() / FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER);
}


void BuffersDialog::enableVolatileSettings(bool enabled)
{
    this->setEnabled(enabled);
}
