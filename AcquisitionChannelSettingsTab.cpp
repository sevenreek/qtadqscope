#include "AcquisitionChannelSettingsTab.h"
#include "ui_AcquisitionChannelSettingsTab.h"
#include <QRadioButton>
#include "util.h"
//            

AcquisitionChannelSettingsTab::AcquisitionChannelSettingsTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AcquisitionChannelSettingsTab)
{
    ui->setupUi(this);
}
void AcquisitionChannelSettingsTab::initialize(ApplicationContext * context, int index)
{
    this->DigitizerGUIComponent::initialize(context);
    this->channel = index;
    this->ui->channelActive->connect(
        this->ui->channelActive, &QCheckBox::stateChanged,
        this, [=](int state){
            emit this->channelActiveChanged(state?true:false);
        }
    );
    this->ui->useTriggerSource->connect(
        this->ui->useTriggerSource, &QCheckBox::stateChanged,
        this, [=](int state){
            emit this->triggerActiveChanged(state?true:false);
        }
    );
    this->ui->triggerOffsetFromZero->connect(
        this->ui->triggerOffsetFromZero,
        &QRadioButton::clicked,
        this,
        [=](){this->setOffsetSource(true);}
    );
    this->ui->triggerOffsetFromBias->connect(
        this->ui->triggerOffsetFromBias,
        &QRadioButton::clicked,
        this,
        [=](){this->setOffsetSource(false);}
    );
    this->ui->triggerLevelCode->connect(
        this->ui->triggerLevelCode, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this, [=](int val) {
            this->setTriggerLevel(val);
        }
    );
    this->ui->triggerLevelMv->connect(
        this->ui->triggerLevelMv,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        [=](double d) {
            this->setTriggerLevelMv(d);
        }
    );
    this->ui->triggerEdge->connect(
        this->ui->triggerEdge,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        [=](int index){ this->digitizer->setTriggerEdge(static_cast<TRIGGER_EDGES>(index));}
    );
    this->ui->triggerReset->connect(
        this->ui->triggerReset, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        [=](int val) {
            this->digitizer->setTriggerReset(val);
        }
    );
    this->ui->inputRange->connect(
        this->ui->inputRange,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        [=](int index){
            this->digitizer->setInputRange(this->channel, static_cast<INPUT_RANGES>(index));
            this->invalidateTriggerLevels();
            this->invalidateDCOffset();
        }
    );
    this->ui->dcOffsetCode->connect(
        this->ui->dcOffsetCode, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        [=](int val) {
            this->setDCOffset(val);
        }
    );
    this->ui->dcOffsetMv->connect(
        this->ui->dcOffsetMv,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        [=](double d){
            this->setDCOffsetMv(d);
        }
    );
}

void AcquisitionChannelSettingsTab::setOffsetSource(bool fromZero)
{
    this->config->setOffsetTriggerFromZero(fromZero);
    this->invalidateTriggerLevels();
}

void AcquisitionChannelSettingsTab::setTriggerLevel(int val)
{
    this->ui->triggerLevelCode->blockSignals(true);
    this->ui->triggerLevelMv->blockSignals(true);

    double channelInputRange = this->digitizer->getObtainedRange(this->channel);
    int bias = this->digitizer->getDCBias(this->channel);
    int relative, absolute;

    if(this->config->getOffsetTriggerFromZero())
    {
        absolute = val;
        relative = absolute - bias;
    }
    else
    {
        relative = val;
        absolute = relative + bias;
    }
    double absoluteMv = codeToMv(absolute, channelInputRange);
    double relativeMv = codeToMv(relative, channelInputRange);
    this->digitizer->setTriggerLevel(absolute);
    if(this->config->getOffsetTriggerFromZero()) {
        this->ui->triggerLevelMv->setValue(absoluteMv);
    }
    else {
        this->ui->triggerLevelMv->setValue(relativeMv);
    }
    this->ui->triggerLevelCodeAbsolute->setText(QString::fromStdString(fmt::format("{}", absolute)));
    this->ui->triggerLevelMvAbsolute->setText(QString::fromStdString(fmt::format("{:.2f} mV", absoluteMv)));

    this->ui->triggerLevelCode->blockSignals(false);
    this->ui->triggerLevelMv->blockSignals(false);
}

void AcquisitionChannelSettingsTab::setTriggerLevelMv(double d)
{
    this->ui->triggerLevelCode->blockSignals(true);
    this->ui->triggerLevelMv->blockSignals(true);

    double channelInputRange = this->digitizer->getObtainedRange(this->channel);
    int bias = this->digitizer->getDCBias(this->channel);
    double biasMv = codeToMv(bias, channelInputRange);

    double relativeMv, absoluteMv;

    if(this->config->getOffsetTriggerFromZero())
    {
        absoluteMv = d;
        relativeMv = absoluteMv - biasMv;
    }
    else
    {
        relativeMv = d;
        absoluteMv = relativeMv + biasMv;
    }
    int absolute = mvToCode(absoluteMv, channelInputRange);
    int relative = mvToCode(relativeMv, channelInputRange);
    if(this->config->getOffsetTriggerFromZero()) {
        this->ui->triggerLevelCode->setValue(absolute);
    }
    else {
        this->ui->triggerLevelCode->setValue(relative);
    }
    this->digitizer->setTriggerLevel(absolute);
    this->ui->triggerLevelCodeAbsolute->setText(QString::fromStdString(fmt::format("{}", absolute)));
    this->ui->triggerLevelMvAbsolute->setText(QString::fromStdString(fmt::format("{:.2f} mV", absoluteMv)));

    this->ui->triggerLevelCode->blockSignals(false);
    this->ui->triggerLevelMv->blockSignals(false);
}

void AcquisitionChannelSettingsTab::setDCOffset(int val)
{
    this->ui->dcOffsetCode->blockSignals(true);
    this->ui->dcOffsetMv->blockSignals(true);
    this->digitizer->setDCBias(this->channel, val);
    double channelInputRange = this->digitizer->getObtainedRange(this->channel);
    int code = this->digitizer->getDCBias(this->channel);
    double mv = codeToMv(code, channelInputRange);
    this->ui->dcOffsetCode->setValue(code);
    this->ui->dcOffsetMv->setValue(mv);
    this->ui->approximateOffset->setText(QString::fromStdString(fmt::format("{:.2f} mV", mv)));
    this->ui->dcOffsetCode->blockSignals(false);
    this->ui->dcOffsetMv->blockSignals(false);
    this->invalidateTriggerLevels();
}

void AcquisitionChannelSettingsTab::setDCOffsetMv(double mv)
{
    this->ui->dcOffsetCode->blockSignals(true);
    this->ui->dcOffsetMv->blockSignals(true);
    double range = this->digitizer->getObtainedRange(this->channel);
    int code = mvToCode(mv, range);
    this->digitizer->setDCBias(this->channel, code);
    this->ui->dcOffsetCode->setValue(code);
    double approxRealValue = codeToMv(code, range);
    this->ui->approximateOffset->setText(QString::fromStdString(fmt::format("{:.2f} mV", approxRealValue)));
    this->ui->dcOffsetCode->blockSignals(false);
    this->ui->dcOffsetMv->blockSignals(false);
    this->invalidateTriggerLevels();
}


void AcquisitionChannelSettingsTab::invalidateTriggerLevels()
{
    int absolute, relative, bias;
    double absoluteMv, relativeMv, biasMv;
    absolute = this->digitizer->getTriggerLevel();
    bias = this->digitizer->getDCBias(this->channel);
    relative = absolute - bias;
    absoluteMv = codeToMv(absolute, this->digitizer->getObtainedRange(this->channel));
    biasMv = codeToMv(bias, this->digitizer->getObtainedRange(this->channel));
    relativeMv = absoluteMv - biasMv;
    this->ui->triggerLevelCode->blockSignals(true);
    this->ui->triggerLevelMv->blockSignals(true);
    if(this->config->getOffsetTriggerFromZero()) {
        this->ui->triggerLevelCode->setValue(absolute);
        this->ui->triggerLevelMv->setValue(absoluteMv);
    }
    else {
        this->ui->triggerLevelCode->setValue(relative);
        this->ui->triggerLevelMv->setValue(relativeMv);
    }
    this->ui->triggerLevelCodeAbsolute->setText(QString::fromStdString(fmt::format("{}", absolute)));
    this->ui->triggerLevelMvAbsolute->setText(QString::fromStdString(fmt::format("{:.2f} mV", absoluteMv)));
    this->ui->triggerLevelCode->blockSignals(false);
    this->ui->triggerLevelMv->blockSignals(false);
}

void AcquisitionChannelSettingsTab::invalidateDCOffset()
{
    this->ui->dcOffsetCode->blockSignals(true);
    this->ui->dcOffsetMv->blockSignals(true);
    double channelInputRange = this->digitizer->getObtainedRange(this->channel);
    int code = this->digitizer->getDCBias(this->channel);
    this->ui->dcOffsetCode->setValue(code);
    this->ui->dcOffsetMv->setValue(codeToMv(code, channelInputRange));
    this->ui->dcOffsetCode->blockSignals(false);
    this->ui->dcOffsetMv->blockSignals(false);
}

AcquisitionChannelSettingsTab::~AcquisitionChannelSettingsTab()
{
    delete ui;
}

void AcquisitionChannelSettingsTab::reloadUI()
{
    this->ui->triggerEdge->setCurrentIndex(this->digitizer->getTriggerEdge());
    if(isBitSet(this->channel, this->digitizer->getChannelMask()))
    {
        this->setChannelActive(true);
    }
    if(isBitSet(this->channel, this->digitizer->getTriggerMask()))
    {
        this->setTriggerActive(true);
    }
    if(this->config->getOffsetTriggerFromZero())
    {
        this->ui->triggerOffsetFromZero->setChecked(true);
    }
    else
    {
        this->ui->triggerOffsetFromBias->setChecked(true);
    }
    this->invalidateTriggerLevels();
    this->ui->inputRange->setCurrentIndex(this->digitizer->getInputRange(this->channel));
    this->setDCOffset(this->digitizer->getDCBias(this->channel));
    this->ui->triggerReset->setValue(this->digitizer->getTriggerReset());
    if(this->digitizer->getTriggerApproach() == TRIGGER_APPROACHES::SINGLE)
    {
        this->setTriggerChangeAllowed(false);
        this->setActiveChangeAllowed(false);
    }
    else if (this->digitizer->getTriggerApproach() - TRIGGER_APPROACHES::CH1 == channel)
    {
        this->setTriggerChangeAllowed(false);
        this->setActiveChangeAllowed(true);
    }
    else if(this->digitizer->getTriggerApproach() == TRIGGER_APPROACHES::INDIVIDUAL)
    {
        this->setTriggerChangeAllowed(true);
        this->setActiveChangeAllowed(true);
    }
}

void AcquisitionChannelSettingsTab::setTriggerChangeAllowed(bool allowed)
{
    this->allowTriggerChange = allowed;
    this->ui->useTriggerSource->setEnabled(allowed);
}
void AcquisitionChannelSettingsTab::setActiveChangeAllowed(bool allowed)
{
    this->allowAcqChange = allowed;
    this->ui->channelActive->setEnabled(allowed);
}
void AcquisitionChannelSettingsTab::setChannelActive(bool act)
{
    this->ui->channelActive->blockSignals(true);
    this->ui->channelActive->setChecked(act);
    this->ui->channelActive->blockSignals(false);
}

void AcquisitionChannelSettingsTab::setTriggerActive(bool act)
{
    this->ui->useTriggerSource->blockSignals(true);
    this->ui->useTriggerSource->setChecked(act);
    this->ui->useTriggerSource->blockSignals(false);
}


void AcquisitionChannelSettingsTab::setObtainedRange(double val)
{
    this->ui->obtainedInputRange->setText(QString::fromStdString(fmt::format("{:.2f} mV", val)));
}



void AcquisitionChannelSettingsTab::enableVolatileSettings(bool enabled)
{
    this->ui->AFE->setEnabled(enabled);
    this->ui->trigger->setEnabled(enabled);
    this->ui->inputRange->setEnabled(enabled);
    this->ui->channelActive->setEnabled(enabled?this->allowAcqChange:false);
    this->ui->useTriggerSource->setEnabled(enabled?this->allowTriggerChange:false);
}
