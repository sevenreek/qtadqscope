#include "AcquisitionSettings.h"
#include "ui_AcquisitionSettings.h"
#include "spdlog/fmt/fmt.h"
#include "util.h"
#include <cmath>
AcquisitionSettings::AcquisitionSettings( QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AcquisitionSettings)
{
    ui->setupUi(this);

}

AcquisitionSettings::~AcquisitionSettings()
{
    this->ui->channelTabs->disconnect(this);
    delete ui;
}

void AcquisitionSettings::reloadUI()
{
    this->ui->recordProcessorsPanel->reloadUI();
    this->ui->bypassUL1->setChecked((this->digitizer->getUserLogicBypass()&(1<<0))?true:false);
    this->ui->bypassUL1->setChecked((this->digitizer->getUserLogicBypass()&(1<<1))?true:false);
    this->ui->acquisitionTag->setText(QString::fromStdString(this->digitizer->getAcquisitionTag()));
    this->ui->delay->setValue(this->digitizer->getTriggerDelay());

    this->ui->pretrigger->setValue(this->digitizer->getPretrigger());
    this->ui->triggerMode->setCurrentIndex(this->digitizer->getTriggerMode());

    this->ui->sampleSkip->setValue(this->digitizer->getSampleSkip());
    this->ui->frequency->setText(QString::fromStdString(this->calculateFrequency(MAX_SAMPLING_RATE, this->digitizer->getSampleSkip())));
    this->ui->recordLength->setValue(this->digitizer->getRecordLength());

    if(this->digitizer->getRecordCount() == Acquisition::INFINITE_RECORDS)
    {
        this->ui->recordCount->setValue(0);
        this->ui->limitRecords->setChecked(false);
        this->ui->recordCount->setEnabled(false);
    }
    if(isOnlyOneBitSet(this->digitizer->getChannelMask()))
    {
        this->ui->enableMultichannel->setChecked(this->config->getAllowMultichannel());
        int index = 0;
        for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
        {
            if(this->digitizer->getChannelMask() & (1<<ch))
            {
                index = ch; break;
            }
        }
        this->ui->channelTabs->setCurrentIndex(index);
    }
    else
    {
        this->config->setAllowMultichannel(true);
        this->ui->enableMultichannel->setChecked(true);
    }
    for(int ch=0; ch < MAX_NOF_CHANNELS; ch++)
    {
        tabs.at(ch)->reloadUI();
    }
    this->handleTabNameChange(0, true);
}

void AcquisitionSettings::initialize(ApplicationContext * context)
{
    this->DigitizerGUIComponent::initialize(context);
    for(int ch=0; ch < MAX_NOF_CHANNELS; ch++)
    {
        tabs.at(ch) = std::unique_ptr<AcquisitionChannelSettingsTab>(new AcquisitionChannelSettingsTab(this));
        tabs.at(ch)->initialize(context, ch);
        this->ui->channelTabs->addTab(tabs.at(ch).get(), QString::fromStdString(fmt::format("CH{}",ch+1)));
    }
    this->ui->bypassUL1->connect(
        this->ui->bypassUL1, &QCheckBox::stateChanged,
        this, [=](int state){
            if(state) this->digitizer->setUserLogicBypass(this->digitizer->getUserLogicBypass() | 0b01);
            else this->digitizer->setUserLogicBypass(this->digitizer->getUserLogicBypass() & ~0b01);
        }
    );
    this->ui->bypassUL2->connect(
        this->ui->bypassUL2, &QCheckBox::stateChanged,
        this, [=](int state){
            if(state) this->digitizer->setUserLogicBypass(this->digitizer->getUserLogicBypass() | 0b10);
            else this->digitizer->setUserLogicBypass(this->digitizer->getUserLogicBypass() & ~0b10);
        }
    );
    this->ui->delay->connect(
        this->ui->delay, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this, [=](int val) {
            this->digitizer->setTriggerDelay(val);
        }
    );
    this->ui->pretrigger->connect(
        this->ui->pretrigger, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this, [=](int val) {
            this->digitizer->setPretrigger(val);
        }
    );
    this->ui->recordLength->connect(
        this->ui->recordLength, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this, [=](int val) {
            this->digitizer->setRecordLength(val);
        }
    );
    this->ui->recordCount->connect(
        this->ui->recordCount, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this, [=](int val){
            if(this->ui->limitRecords->isChecked())
            {
                this->digitizer->setRecordCount(val);
            }
            else
            {
                this->digitizer->setRecordCount(Acquisition::INFINITE_RECORDS);
            }
        }
    );
    this->ui->sampleSkip->connect(
        this->ui->sampleSkip, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this, [=](int val){
            if(val == 3)
            {
                this->ui->sampleSkip->setValue(this->digitizer->getSampleSkip()==2?4:2);
                return;
            }
            this->digitizer->setSampleSkip(val);
            this->ui->frequency->setText(QString::fromStdString(this->calculateFrequency(MAX_SAMPLING_RATE, val)));
        }
    );
    this->ui->limitRecords->connect(
        this->ui->limitRecords, &QCheckBox::stateChanged,
        this, [=](int state){
            if(state)
            {
                this->digitizer->setRecordCount(this->ui->recordCount->value());
                this->ui->recordCount->setEnabled(true);
            }
            else
            {
                this->digitizer->setRecordCount(Acquisition::INFINITE_RECORDS);
                this->ui->recordCount->setEnabled(false);
            }
        }
    );
    this->ui->triggerMode->connect(
        this->ui->triggerMode,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ this->digitizer->setTriggerMode(static_cast<Digitizer::DIGITIZER_TRIGGER_MODE>(index));}
    );
    this->ui->enableMultichannel->connect(
        this->ui->enableMultichannel, &QCheckBox::stateChanged,
        [=](int state) {
            this->config->setAllowMultichannel(state?true:false);
            if(!state)
            {
                this->tabs.at(this->ui->channelTabs->currentIndex())->setTriggerActive(true, true);
                this->tabs.at(this->ui->channelTabs->currentIndex())->setChannelActive(true, true);
                this->handleSetChannelActive(this->ui->channelTabs->currentIndex(), true);
                this->handleSetChannelTriggerActive(this->ui->channelTabs->currentIndex(), true);
                this->tabs.at(this->ui->channelTabs->currentIndex())->setExclusive(true);
            }
            else if(state)
            {
                for(size_t ch=0; ch < this->tabs.size(); ch++)
                {
                    this->tabs.at(ch)->setExclusive(false);
                }
            }
        }
    );
    this->ui->channelTabs->connect(this->ui->channelTabs, &QTabWidget::currentChanged, this, &AcquisitionSettings::handleTabChanged);
    for(int ch=0; ch < MAX_NOF_CHANNELS; ch++)
    {
        this->tabs.at(ch)->connect(
            this->tabs.at(ch).get(), &AcquisitionChannelSettingsTab::channelActiveChanged,
            this, [this, ch](bool act){this->handleSetChannelActive(ch, act);}
        );
        this->tabs.at(ch)->connect(
            this->tabs.at(ch).get(), &AcquisitionChannelSettingsTab::triggerActiveChanged,
            this, [this, ch](bool act){this->handleSetChannelTriggerActive(ch, act);}
        );
    }
    this->digitizer->connect(
        this->digitizer,
        &Digitizer::inputRangeChanged,
        this,
        [=](int ch, INPUT_RANGES r) {
            this->tabs.at(ch)->setObtainedRange(this->digitizer->getObtainedRange(ch));
        }
    );
    this->ui->acquisitionTag->connect(
        this->ui->acquisitionTag,
        &QLineEdit::textChanged,
        this,
        [=](const QString &text)
            {
                this->digitizer->setAcquisitionTag(text.toStdString());
            }
     );
    this->ui->recordProcessorsPanel->initialize(context);
}

std::string AcquisitionSettings::calculateFrequency(unsigned long long samplingRate, unsigned long long sampleSkip)
{
    int timesDivided = 0;
    double sr = double(samplingRate)/sampleSkip;
    while(sr >= 1000)
    {
        sr/=1000.0;
        timesDivided++;
    }
    return fmt::format("{:.2f} {}Hz", sr, UNIT_PREFIXES[timesDivided]);
}

void AcquisitionSettings::handleTabChanged(int tab)
{
    if(clip(tab, 0, int(this->tabs.size())-1) != tab) return;
    if(!this->config->getAllowMultichannel())
    {
        this->tabs.at(this->lastActiveChannel)->setChannelActive(false, false);
        this->tabs.at(this->lastActiveChannel)->setTriggerActive(false, false);
        this->tabs.at(tab)->setChannelActive(true, true);
        this->tabs.at(tab)->setTriggerActive(true, true);
        this->handleSetChannelActive(tab, true);
        this->handleSetChannelTriggerActive(tab, true);
    }
    this->tabs.at(tab)->reloadUI();
}

void AcquisitionSettings::handleSetChannelActive(int channel, bool active)
{
    if(this->config->getAllowMultichannel())
    {
        if(active) this->digitizer->setChannelMask(this->digitizer->getChannelMask() | (1<<channel));
        else this->digitizer->setChannelMask(this->digitizer->getChannelMask() & ~(1<<channel));
    }
    else
    {
        this->digitizer->setChannelMask((1<<channel));
    }
    this->handleTabNameChange(channel, true);
    this->lastActiveChannel = channel;
}

void AcquisitionSettings::handleSetChannelTriggerActive(int channel, bool active)
{
    if(this->config->getAllowMultichannel())
    {
        if(active) this->digitizer->setTriggerMask(this->digitizer->getTriggerMask() | (1<<channel));
        else this->digitizer->setTriggerMask(this->digitizer->getTriggerMask() & ~(1<<channel));
    }
    else
    {
        this->digitizer->setTriggerMask((1<<channel));
    }

    this->handleTabNameChange(channel, true);
}

void AcquisitionSettings::handleTabNameChange(int channel, bool recreateAll)
{
    int chMask = this->digitizer->getChannelMask();
    int trMask = this->digitizer->getTriggerMask();
    if(recreateAll)
    {
        for(int ch = 0; ch < this->ui->channelTabs->count(); ch++)
        {
            bool active = isBitSet(ch, chMask);
            std::string tabname;
            if(active) tabname = fmt::format("CH{}{}{}", ch+1, CHANNEL_ACTIVE_EMOJI, isBitSet(ch, trMask)?TRIGGER_ACTIVE_EMOJI:"");/*▶🗲*/
            else tabname = fmt::format("CH{}{}", ch+1, isBitSet(ch, trMask)?TRIGGER_ACTIVE_EMOJI:"");/*▶🗲*/
            this->ui->channelTabs->setTabText(ch, QString::fromStdString(tabname));
        }
    }
    else if(channel < this->ui->channelTabs->count())
    {
        bool active = isBitSet(channel, chMask);
        std::string tabname;
        if(active) tabname = fmt::format("CH{}{}{}", channel+1, CHANNEL_ACTIVE_EMOJI, isBitSet(channel, trMask)?TRIGGER_ACTIVE_EMOJI:"");/*▶🗲*/
        else tabname = fmt::format("CH{}{}", channel+1, isBitSet(channel, trMask)?TRIGGER_ACTIVE_EMOJI:"");/*▶🗲*/
        this->ui->channelTabs->setTabText(channel, QString::fromStdString(tabname));
    }
}




void AcquisitionSettings::enableVolatileSettings(bool enabled)
{
    this->ui->bypassUL1->setEnabled(enabled);
    this->ui->bypassUL2->setEnabled(enabled);
    this->ui->acquisitionTag->setEnabled(enabled);
    this->ui->frequency->setEnabled(enabled);
    this->ui->delay->setEnabled(enabled);
    this->ui->enableMultichannel->setEnabled(enabled);
    this->ui->limitRecords->setEnabled(enabled);
    this->ui->pretrigger->setEnabled(enabled);
    this->ui->recordCount->setEnabled(enabled);
    this->ui->recordLength->setEnabled(enabled);
    this->ui->sampleSkip->setEnabled(enabled);
    this->ui->triggerMode->setEnabled(enabled);
    for(int ch = 0; ch < this->ui->channelTabs->count(); ch++)
    {
        this->tabs.at(ch)->enableVolatileSettings(enabled);
    }
    this->ui->recordProcessorsPanel->enableVolatileSettings(enabled);
}
