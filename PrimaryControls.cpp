#include "PrimaryControls.h"
#include "ui_PrimaryControls.h"

PrimaryControls::PrimaryControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrimaryControls)
{
    ui->setupUi(this);
}

PrimaryControls::~PrimaryControls()
{
    delete ui;
}
void PrimaryControls::reloadUI()
{
    this->resetFillIndicators();
    this->ui->notificationsTextArea->clear();

    if(this->digitizer->getDuration())
    {
        this->ui->timedRunCheckbox->setChecked(true);
        this->ui->timedRunValue->setEnabled(true);
        this->ui->timedRunValue->setValue(this->digitizer->getDuration());
    }
    else
    {
        this->ui->timedRunCheckbox->setChecked(false);
        this->ui->timedRunValue->setEnabled(false);
    }

}
void PrimaryControls::resetFillIndicators()
{
    this->ui->DMAFillStatus->setValue(0);
    this->ui->FileFillStatus->setValue(0);
    this->ui->RAMFillStatus->setValue(0);
}

void PrimaryControls::initialize(ApplicationContext *context)
{
    this->DigitizerGUIComponent::initialize(context);
    this->context = context;
    this->ui->streamStartStopButton->connect(
        this->ui->streamStartStopButton,
        &QAbstractButton::clicked,
        this,
        &PrimaryControls::primaryButtonClicked
    );
    this->digitizer->connect(
        this->digitizer,
        &Digitizer::digitizerStateChanged,
        this,
        &PrimaryControls::digitizerStateChanged
    );
    this->ui->timedRunCheckbox->connect(
        this->ui->timedRunCheckbox, &QCheckBox::stateChanged,
        this, [=](int state){
            if(state) {
                this->digitizer->setDuration(this->ui->timedRunValue->value());
                this->ui->timedRunValue->setEnabled(true);
            }
            else {
                this->digitizer->setDuration(Acquisition::NO_DURATION);
                this->ui->timedRunValue->setEnabled(false);
            }
        }
    );
    this->ui->timedRunValue->connect(
        this->ui->timedRunValue, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this, [=](int val) {
            if(this->ui->timedRunCheckbox->isChecked()) this->digitizer->setDuration(val);
            else this->digitizer->setDuration(Acquisition::NO_DURATION);
        }
    );
    this->periodicUpdateTimer.connect(
        &this->periodicUpdateTimer,
        &QTimer::timeout,
        this,
        &PrimaryControls::periodicUIUpdate
    );
}

void PrimaryControls::primaryButtonClicked()
{
    switch(this->digitizer->getDigitizerState())
    {
        case Digitizer::DIGITIZER_STATE::ACTIVE:
            this->ui->streamStartStopButton->setEnabled(false);
            this->digitizer->stopAcquisition();
        break;
        case Digitizer::DIGITIZER_STATE::READY:
            this->ui->streamStartStopButton->setText("STOP");
            this->ui->streamStatusLabel->setText("STARTING");
            this->ui->streamStartStopButton->setEnabled(false);
            this->digitizer->runAcquisition();
        break;
        case Digitizer::DIGITIZER_STATE::STOPPING:

        break;
    }
    if(this->ui->resetPlot->isChecked())
    {
        emit this->resetPlot();
    }
}

void PrimaryControls::digitizerStateChanged(Digitizer::DIGITIZER_STATE state)
{
    switch(state)
    {
        case Digitizer::DIGITIZER_STATE::ACTIVE:
            this->ui->streamStartStopButton->setEnabled(true);
            this->ui->streamStatusLabel->setText("ACTIVE");
            this->ui->streamStartStopButton->setText("STOP");
            this->periodicUpdateTimer.start(this->config->getPeriodicUpdatePeriod());
        break;
        case Digitizer::DIGITIZER_STATE::READY:
            this->ui->streamStartStopButton->setEnabled(true);
            this->ui->streamStatusLabel->setText("READY");
            this->periodicUpdateTimer.stop();
            this->ui->streamStartStopButton->setText("START");
            this->resetFillIndicators();
        break;
        case Digitizer::DIGITIZER_STATE::STOPPING:
            this->ui->streamStartStopButton->setEnabled(false);
            this->ui->streamStatusLabel->setText("STOPPING");
        break;
    }
}

void PrimaryControls::periodicUIUpdate()
{
    if(this->context->fileSaver)
        this->ui->FileFillStatus->setValue(100*double(this->context->fileSaver->getProcessedBytes())/this->digitizer->getFileSizeLimit());
    this->ui->DMAFillStatus->setValue(100*double(this->digitizer->getLastBuffersFill()/(this->digitizer->getTransferBufferCount()-1)));
    this->ui->RAMFillStatus->setValue(100*double(this->digitizer->getQueueFill()/this->digitizer->getTransferBufferQueueSize()));
}