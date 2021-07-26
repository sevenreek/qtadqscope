#include "BuffersDialog.h"
#include "ui_BuffersDialog.h"
const BuffersDialog::DefaultConfig BuffersDialog::DEFAULT_CONFIG_VALUES[BuffersDialog::DEFAULT_CONFIG_COUNT]  = {
    (BuffersDialog::DefaultConfig){.bufferCount=64, .bufferSize=2ul*1024ul, .queueSize=256}, // NONE
    (BuffersDialog::DefaultConfig){.bufferCount=64, .bufferSize=2ul*1024ul, .queueSize=256}, // BALANCED
    (BuffersDialog::DefaultConfig){.bufferCount=512, .bufferSize=256ul, .queueSize=4ul*1024}, // SHORT PULSE
    (BuffersDialog::DefaultConfig){.bufferCount=128, .bufferSize=1ul*1024ul, .queueSize=512}, // LONG PULSE
    (BuffersDialog::DefaultConfig){.bufferCount=64, .bufferSize=1024ul*1024ul, .queueSize=512}, // FILE SAVE
    (BuffersDialog::DefaultConfig){.bufferCount=64, .bufferSize=4ul*1024ul*1024ul, .queueSize=256} // LARGE FILE SAVE
};

BuffersDialog::BuffersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuffersDialog)
{
    ui->setupUi(this);
    connect(
        this->ui->defaultRestore,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
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
double BuffersDialog::FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER = 1000000000.0;
BuffersDialog::~BuffersDialog()
{
    delete ui;
}
