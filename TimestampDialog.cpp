#include "TimestampDialog.h"
#include "ui_TimestampDialog.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "ADQAPIIncluder.h"

TimestampDialog::TimestampDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimestampDialog)
{
    ui->setupUi(this);
}

TimestampDialog::~TimestampDialog()
{
    delete ui;
}

void TimestampDialog::reloadUI()
{
    this->readTimestamp();
    this->loadClockSource();

}

void TimestampDialog::initialize(ApplicationContext *context)
{
    DigitizerGUIComponent::initialize(context);
    connect(this->ui->armOnStart, &QCheckBox::stateChanged, this, &TimestampDialog::changeArmOnStart);
    connect(this->ui->updateTimestamp, &QCheckBox::stateChanged, this, &TimestampDialog::changeUpdateTimestamp);
    connect(this->ui->readTimestamp, &QAbstractButton::pressed, this, &TimestampDialog::readTimestamp);
    connect(this->ui->rearmSync, &QAbstractButton::pressed, this, &TimestampDialog::rearmTimestamp);
    connect(this->ui->disarmSync, &QAbstractButton::pressed, this, &TimestampDialog::disarmTimestamp);
    connect(this->ui->syncOnce, &QCheckBox::stateChanged, this, &TimestampDialog::changeSyncOnce);
    connect(this->ui->clockOutput, &QCheckBox::stateChanged, this, &TimestampDialog::onClockOutputEnableChanged);
    connect(
        this->ui->syncSource,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &TimestampDialog::changeSyncSource
    );
    connect(this->ui->pxiTrig0, &QCheckBox::stateChanged, this, [=](int state) { this->changePXITriggerMask(); });
    connect(this->ui->pxiTrig1, &QCheckBox::stateChanged, this, [=](int state) { this->changePXITriggerMask(); });
    connect(
        this->ui->clockSource,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &TimestampDialog::changeClockSource
    );
    connect(&this->timer, &QTimer::timeout, this, &TimestampDialog::onUpdateTimestamp);
    this->timer.setInterval(250);
    this->timer.setSingleShot(false);
}

void TimestampDialog::enableVolatileSettings(bool enabled)
{

}

void TimestampDialog::onUpdateTimestamp()
{
    this->ui->timestamp->setText(QString::fromStdString(fmt::format("{}", this->tsReader.getLastTimestamp())));
}

void TimestampDialog::readTimestamp()
{
    unsigned long long value;
    if(!this->digitizer->adq.GetTimestampValue(&value))
    {
        spdlog::error("Failed to read timstamp value.");
        return;
    }
    this->ui->timestamp->setText(QString::fromStdString(fmt::format("{}", value)));
}

void TimestampDialog::rearmTimestamp()
{
    this->digitizer->adq.DisarmTimestampSync();
    this->digitizer->adq.SetupTimestampSync(this->timestampConfig.syncOnMultipleEvents, this->timestampConfig.source);
    this->digitizer->adq.ArmTimestampSync();
}

void TimestampDialog::disarmTimestamp()
{
    this->digitizer->adq.DisarmTimestampSync();
}
void TimestampDialog::changeArmOnStart(int state)
{
    this->timestampConfig.rearmOnAcquisitionStart = state;
    this->updateDigitizerConfig();
}
void TimestampDialog::changeUpdateTimestamp(int state)
{
    if(state) {
        this->digitizer->appendRecordProcessor(&this->tsReader);
        this->timer.start();
    }
    else {
        this->timer.stop();
        this->digitizer->removeRecordProcessor(&this->tsReader);
    }
}
void TimestampDialog::changeSyncOnce(int state)
{
    this->timestampConfig.syncOnMultipleEvents = !state;
    this->updateDigitizerConfig();
}
void TimestampDialog::updateDigitizerConfig()
{
    this->digitizer->setTimestampSyncConfig(this->timestampConfig);
}
void TimestampDialog::changeSyncSource(int source)
{
    switch(source)
    {
        case 0: this->timestampConfig.source = ADQ_EVENT_SOURCE_SOFTWARE; break;
        case 1: this->timestampConfig.source = ADQ_EVENT_SOURCE_PXIE_STARB; break;
        case 2: this->timestampConfig.source = ADQ_EVENT_SOURCE_SYNC; break;
        case 3: this->timestampConfig.source = ADQ_EVENT_SOURCE_PXI_TRIG; break;
    }
    this->updateDigitizerConfig();
}
void TimestampDialog::changePXITriggerMask()
{
    bool pxi0 = this->ui->pxiTrig0->checkState();
    bool pxi1 = this->ui->pxiTrig1->checkState();
    this->digitizer->adq.SetTriggerMaskPXI(pxi1<<1 | pxi0<<0);
}

void TimestampDialog::changeClockSource(int index)
{
    bool switchResult = true;
    switch(index)
    {
        default: spdlog::warn("Unsupported value for clock source.");
        case 0: switchResult = this->digitizer->adq.SetClockSource(ADQ_CLOCK_SOURCE_INTREF); break;
        case 1: switchResult = this->digitizer->adq.SetClockSource(ADQ_CLOCK_SOURCE_EXTREF); break;
        case 2: switchResult = this->digitizer->adq.SetClockSource(ADQ_CLOCK_SOURCE_PXIE_10M); break;
        case 3: switchResult = this->digitizer->adq.SetClockSource(ADQ_CLOCK_SOURCE_PXIE_100M); break;
    }
    if(!switchResult) spdlog::error("Failed to switch to clock source {}.", index);
}
void TimestampDialog::loadClockSource()
{
    unsigned int clockSource = this->digitizer->adq.GetClockSource();
    switch(clockSource)
    {
        default:
            spdlog::warn("Unsupported value for clock source. Restoring to internal reference clock.");
            this->digitizer->adq.SetClockSource(ADQ_CLOCK_SOURCE_INTREF);
        case ADQ_CLOCK_SOURCE_INTREF: this->ui->clockSource->setCurrentIndex(0); break;
        case ADQ_CLOCK_SOURCE_EXTREF: this->ui->clockSource->setCurrentIndex(1); break;
        case ADQ_CLOCK_SOURCE_PXIE_10M: this->ui->clockSource->setCurrentIndex(2); break;
        case ADQ_CLOCK_SOURCE_PXIE_100M: this->ui->clockSource->setCurrentIndex(3); break;
    }
}
void TimestampDialog::onClockOutputEnableChanged(int state)
{
    if(!this->digitizer->adq.EnableClockRefOut(state))
        spdlog::error("Failed to enable/disable clock reference output");
}
