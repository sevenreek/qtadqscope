#include "Application.h"
#include "MainWindow.h"
#include "DigitizerConfiguration.h"
#include "./ui_MainWindow.h"
#include "./ui_BuffersDialog.h"
#include "./ui_RegisterDialog.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "BinaryFileWriter.h"
#include <limits>
#include <memory>
Application::Application( MainWindow& mainWindow) : mainWindow(mainWindow)
{
    this->config = std::make_shared<ApplicationConfiguration>();
}
int Application::start(int argc, char *argv[]) {

    this->config.fromFile("default_config.json");
    this->adqControlUnit = CreateADQControlUnit();
#ifdef MOCK_ADQAPI
    spdlog::warn("Using mock ADQAPI");
#else
    if(this->adqControlUnit == NULL)
    {
        spdlog::error("Failed to create ADQControlUnit. Exiting...");
        return -1;
    }
#endif
    // parse args here
    ADQControlUnit_EnableErrorTrace(this->adqControlUnit, std::max((int)this->config.getAdqLoggingLevel(), 3), "."); // log to root dir, LOGGING_LEVEL::DEBUG is 4 but API only supports INFO=3
    ADQControlUnit_FindDevices(this->adqControlUnit);
    int numberOfDevices = ADQControlUnit_NofADQ(adqControlUnit);
    if(numberOfDevices == 0)
    {
        spdlog::error("No ADQ devices found. Exiting.");
        return -1;
    }
    else if(numberOfDevices != 1)
    {
        spdlog::warn("Found {} devices. Using {}.", numberOfDevices, this->config.getDeviceNumber());
    }
    this->adqWrapper = std::unique_ptr<ADQInterfaceWrapper>(new MutexADQWrapper(adqControlUnit, this->config.getDeviceNumber()));
    this->digitizer =
        std::unique_ptr<Digitizer>(
            new Digitizer(
                *this->adqWrapper.get()
            )
        );
    this->scopeUpdater = std::unique_ptr<ScopeUpdater>(
        new ScopeUpdater(
            this->digitizer->getRecordLength(),
            *this->mainWindow.ui->plotArea
        )
    );
    this->buffersConfigurationDialog = std::unique_ptr<BuffersDialog>(new BuffersDialog());
    this->registerDialog = std::unique_ptr<RegisterDialog>(new RegisterDialog());
    this->autoCalibrateDialog = std::unique_ptr<FullCalibrationDialog>(new FullCalibrationDialog(this->config, this->acquisition));
    this->linkSignals();
    this->setUI();
    this->createPeriodicUpdateTimer(this->config.getPeriodicUpdatePeriod());
    //this->config->toFile("test_json.scfg");
    return 0;
}
void Application::setUI()
{
    // This is a temporary(hopefully) hack.
    int actualChannel = this->config->getCurrentChannel();
    this->mainWindow.ui->channelComboBox->setCurrentIndex((this->config->getCurrentChannel()+1)%MAX_NOF_CHANNELS);
    this->mainWindow.ui->channelComboBox->setCurrentIndex(actualChannel);
    this->changeDMABufferCount(this->config->transferBufferCount);
}


void Application::changeFileTag(QString filetag)
{
    this->config->getCurrentChannelConfig().fileTag = filetag.toStdString();
}



void Application::linkSignals()
{
    // CHANNEL
    this->mainWindow.ui->channelComboBox->connect(
        this->mainWindow.ui->channelComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ this->changeChannel(index); }
    );
    // 2NDCHANNEL
    this->mainWindow.ui->secondChannelComboBox->connect(
        this->mainWindow.ui->secondChannelComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ this->changeSecondChannel(index); }
    );
    // SAMPLE SKIP
    this->mainWindow.ui->sampleSkipInput->connect(
        this->mainWindow.ui->sampleSkipInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeSampleSkip(i); }
    );
    // INPUT RANGE
    this->mainWindow.ui->inputRangeSelector->connect(
        this->mainWindow.ui->inputRangeSelector,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ this->changeInputRange(index); }
    );
    // UL BYPASS
    this->mainWindow.ui->ulBypass1->connect(
        this->mainWindow.ui->ulBypass1,
        &QCheckBox::stateChanged,
        this,
        &Application::changeUL1Bypass
    );
    this->mainWindow.ui->ulBypass2->connect(
        this->mainWindow.ui->ulBypass2,
        &QCheckBox::stateChanged,
        this,
        &Application::changeUL2Bypass
    );
    // ANALOG OFFSET
    this->mainWindow.ui->analogOffsetInput->connect(
        this->mainWindow.ui->analogOffsetInput,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [=](double d){ this->changeAnalogOffset(d); }
    );
    this->mainWindow.ui->analogOffsetCodeInput->connect(
        this->mainWindow.ui->analogOffsetCodeInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        [=](int i){ this->changeAnalogOffsetCode(i); }
    );
    // DIGITAL OFFSET & GAIN
    this->mainWindow.ui->digitalOffsetInput->connect(
        this->mainWindow.ui->digitalOffsetInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeDigitalOffset(i); }
    );
    this->mainWindow.ui->digitalGainInput->connect(
        this->mainWindow.ui->digitalGainInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeDigitalGain(i); }
    );
    // TRIGGER MODE
    this->mainWindow.ui->triggerModeSelector->connect(
        this->mainWindow.ui->triggerModeSelector,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ this->changeTriggerMode(index); }
    );
    // LIIMT RECORD COUNT
    this->mainWindow.ui->limitRecordsCB->connect(
        this->mainWindow.ui->limitRecordsCB,
        &QCheckBox::stateChanged,
        this,
        &Application::changeLimitRecords
    );
    // RECORD COUNT
    this->mainWindow.ui->recordCountInput->connect(
        this->mainWindow.ui->recordCountInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeRecordCount(i); }
    );
    // PRETRIGGER
    this->mainWindow.ui->pretriggerInput->connect(
        this->mainWindow.ui->pretriggerInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changePretrigger(i); }
    );
    // RECORD LENGTH
    this->mainWindow.ui->recordLengthInput->connect(
        this->mainWindow.ui->recordLengthInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeRecordLength(i); }
    );
    // TRIGER DELAY
    this->mainWindow.ui->triggerDelayInput->connect(
        this->mainWindow.ui->triggerDelayInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeTriggerDelay(i); }
    );
    // TRIGGER EDGE
    this->mainWindow.ui->levelTriggerEdgeSelector->connect(
        this->mainWindow.ui->levelTriggerEdgeSelector,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ this->changeLevelTriggerEdge(index); }
    );
    // LEVEL CODES
    this->mainWindow.ui->levelTriggerCodesInput->connect(
        this->mainWindow.ui->levelTriggerCodesInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeLevelTriggerCode(i); }
    );
    // LEVEL MV
    this->mainWindow.ui->levelTriggerVoltageInput->connect(
        this->mainWindow.ui->levelTriggerVoltageInput,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [=](double d){ this->changeLevelTriggerMV(d); }
    );
    // LEVEL MV
    this->mainWindow.ui->levelTriggerResetOffsetInput->connect(
        this->mainWindow.ui->levelTriggerResetOffsetInput,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        [=](int i){ this->changeLevelTriggerReset(i); }
    );
    // SCOPE UPDATE
    this->mainWindow.ui->updateScopeCB->connect(
        this->mainWindow.ui->updateScopeCB,
        &QCheckBox::stateChanged,
        this,
        &Application::changeUpdateScope
    );
    // ANALYSE
    this->mainWindow.ui->analyseCB->connect(
        this->mainWindow.ui->analyseCB,
        &QCheckBox::stateChanged,
        this,
        &Application::changeAnalyse
    );
    // SAVE TO FILE
    this->mainWindow.ui->saveToFileCB->connect(
        this->mainWindow.ui->saveToFileCB,
        &QCheckBox::stateChanged,
        this,
        &Application::changeSaveToFile
    );
    // FILE SAVE MODE
    this->mainWindow.ui->fileTypeSelector->connect(
        this->mainWindow.ui->fileTypeSelector,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ this->changeFiletype(index); }
    );
    // ACQUSITION STATE CHANGED
    this->acquisition->connect(
        this->acquisition.get(),
        &Acquisition::onStateChanged,
        this,
        &Application::acquisitionStateChanged
    );
    // PRIMARY BUTTON
    this->mainWindow.ui->streamStartStopButton->connect(
        this->mainWindow.ui->streamStartStopButton,
        &QAbstractButton::pressed,
        this,
        &Application::primaryButtonPressed
    );
    // SCOPE UPDATE
    this->scopeUpdater->connect(
        this->scopeUpdater.get(),
        &ScopeUpdater::onScopeUpdate,
        this,
        &Application::updateScope,
        Qt::ConnectionType::BlockingQueuedConnection
    );
    // ACTIONS
    this->mainWindow.ui->actionConfigureDMA->connect(
        this->mainWindow.ui->actionConfigureDMA,
        &QAction::triggered,
        this,
        &Application::configureDMABuffers
    );
    this->mainWindow.ui->actionUL_Registers->connect(
        this->mainWindow.ui->actionUL_Registers,
        &QAction::triggered,
        this,
        &Application::configureULRegisters
    );
    this->mainWindow.ui->actionLoad->connect(
        this->mainWindow.ui->actionLoad,
        &QAction::triggered,
        this,
        &Application::loadConfig
    );
    this->mainWindow.ui->actionSave->connect(
        this->mainWindow.ui->actionSave,
        &QAction::triggered,
        this,
        &Application::saveConfig
    );
    // DMA DIALOG
    this->buffersConfigurationDialog->ui->buttonBox->connect(
        this->buffersConfigurationDialog->ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &Application::onDMADialogClosed
    );
    // REGISTERS DIALOG
    this->registerDialog->ui->buttonBox->connect(
        this->registerDialog->ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &Application::onRegisterDialogClosed
    );
    // SWTRIG
    this->mainWindow.ui->softwareTriggerButton->connect(
        this->mainWindow.ui->softwareTriggerButton,
        &QAbstractButton::pressed,
        this,
        &Application::triggerSoftwareTrig
    );
    // TIMED RUN
    this->mainWindow.ui->timedRunCheckbox->connect(
        this->mainWindow.ui->timedRunCheckbox,
        &QCheckBox::stateChanged,
        this,
        &Application::changeTimedRunEnabled
    );
    this->mainWindow.ui->timedRunValue->connect(
        this->mainWindow.ui->timedRunValue,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeTimedRunValue(i); }
    );
    // BASE DC BIAS
    this->mainWindow.ui->baseOffsetCalibrationValue->connect(
        this->mainWindow.ui->baseOffsetCalibrationValue,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeBaseDCBias(i); }
    );
    // AUTOCALIBRATE
    this->mainWindow.ui->baseOffsetAutoCalibrateButton->connect(
        this->mainWindow.ui->baseOffsetAutoCalibrateButton,
        &QAbstractButton::pressed,
        this,
        &Application::openCalibrateDialog
    );
    // AUTOCALIBRATE
    this->mainWindow.ui->flushDMAButton->connect(
        this->mainWindow.ui->flushDMAButton,
        &QAbstractButton::pressed,
        this,
        &Application::flushDMA
    );
    // TRIGGER OFFSET MODE
    this->mainWindow.ui->offsetFromAnalog->connect(
        this->mainWindow.ui->offsetFromAnalog,
        &QRadioButton::clicked,
        this,
        &Application::updateTriggerLevelDisplays
    );
    this->mainWindow.ui->offsetFromZero->connect(
        this->mainWindow.ui->offsetFromZero,
        &QRadioButton::clicked,
        this,
        &Application::updateTriggerLevelDisplays
    );
    // FILE TAG
    this->mainWindow.ui->acquisitionTag->connect(
        this->mainWindow.ui->acquisitionTag,
        &QLineEdit::textChanged,
        this,
        &Application::changeFileTag
    );
}

///// BEGIN UI SLOTS //////

void Application::changeChannel(int channel) {
    this->config->setCurrentChannel(channel);
    this->config->getCurrentChannelConfig().log();
    this->mainWindow.ui->sampleSkipInput->setValue(this->config->getCurrentChannelConfig().sampleSkip);
    this->mainWindow.ui->inputRangeSelector->setCurrentIndex(this->config->getCurrentChannelConfig().inputRangeEnum);
    this->mainWindow.ui->ulBypass1->setCheckState(
        this->config->getCurrentChannelConfig().userLogicBypass&0b01?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
    this->mainWindow.ui->ulBypass2->setCheckState(
        this->config->getCurrentChannelConfig().userLogicBypass&0b10?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
    this->mainWindow.ui->analogOffsetCodeInput->setValue(this->config->getCurrentChannelConfig().dcBiasCode);
    this->mainWindow.ui->digitalGainInput->setValue(this->config->getCurrentChannelConfig().digitalGain);
    switch(this->config->getCurrentChannelConfig().triggerMode)
    {
        case TRIGGER_MODES::SOFTWARE:
        {
            if(this->config->getCurrentChannelConfig().isContinuousStreaming)
            {
                this->mainWindow.ui->triggerModeSelector->setCurrentIndex(TRIGGER_MODE_SELECTOR::S_FREE_RUNNING);
                this->mainWindow.ui->limitRecordsCB->setEnabled(false);
            }
            else
            {
                this->mainWindow.ui->limitRecordsCB->setEnabled(true);
                this->mainWindow.ui->triggerModeSelector->setCurrentIndex(TRIGGER_MODE_SELECTOR::S_SOFTWARE);
            }

        }
        break;
        case TRIGGER_MODES::LEVEL:
        {
            this->mainWindow.ui->limitRecordsCB->setEnabled(true);
            this->mainWindow.ui->triggerModeSelector->setCurrentIndex(TRIGGER_MODE_SELECTOR::S_LEVEL);
        }
        break;
        case TRIGGER_MODES::EXTERNAL:
        {
            this->mainWindow.ui->limitRecordsCB->setEnabled(true);
            this->mainWindow.ui->triggerModeSelector->setCurrentIndex(TRIGGER_MODE_SELECTOR::S_EXTERNAL);
        }
        break;
        default:
        {
            spdlog::error("Unsupported trigger mode {}.", this->config->getCurrentChannelConfig().triggerMode);
        }
        break;
    }
    if(this->config->getCurrentChannelConfig().recordCount>MAX_FINITE_RECORD_COUNT)
    {
        this->mainWindow.ui->limitRecordsCB->setCheckState(Qt::CheckState::Unchecked);
    }
    else
    {
        this->mainWindow.ui->limitRecordsCB->setCheckState(Qt::CheckState::Checked);
        this->mainWindow.ui->recordCountInput->setValue(this->config->getCurrentChannelConfig().recordCount);
    }
    this->mainWindow.ui->pretriggerInput->setValue(this->config->getCurrentChannelConfig().pretrigger);
    this->mainWindow.ui->recordLengthInput->setValue(this->config->getCurrentChannelConfig().recordLength);
    this->mainWindow.ui->triggerDelayInput->setValue(this->config->getCurrentChannelConfig().triggerDelay);
    this->mainWindow.ui->levelTriggerEdgeSelector->setCurrentIndex(this->config->getCurrentChannelConfig().triggerEdge);
    this->mainWindow.ui->levelTriggerCodesInput->setValue(this->config->getCurrentChannelConfig().triggerLevelCode);
    this->mainWindow.ui->levelTriggerResetOffsetInput->setValue(this->config->getCurrentChannelConfig().triggerLevelReset);
    this->scopeUpdater->changePlotTriggerLine(this->config->getCurrentChannelConfig());
    this->mainWindow.ui->baseOffsetCalibrationValue->setValue(this->config->getCurrentChannelConfig().getCurrentBaseDCOffset());
    this->mainWindow.ui->digitalOffsetInput->setValue(this->config->getCurrentChannelConfig().getCurrentDigitalOffset());
    this->updateTriggerLevelDisplays();
}
void Application::changeSecondChannel(int channel) {
    channel -= 1; // -1 is disabled
    this->config->secondChannel = channel;
}

void Application::changeSampleSkip(int val) {
    if(val==3)
    {
        spdlog::info("Sample skip cannot be 3.");
        if(this->config->getCurrentChannelConfig().sampleSkip<3)
        {
            this->config->getCurrentChannelConfig().sampleSkip = 4;
            this->mainWindow.ui->sampleSkipInput->blockSignals(true);
            this->mainWindow.ui->sampleSkipInput->setValue(4);
            this->mainWindow.ui->sampleSkipInput->blockSignals(false);
        }
        else
        {
            this->config->getCurrentChannelConfig().sampleSkip = 2;
            this->mainWindow.ui->sampleSkipInput->blockSignals(true);
            this->mainWindow.ui->sampleSkipInput->setValue(2);
            this->mainWindow.ui->sampleSkipInput->blockSignals(false);
        }
    }
    else
    {
        this->config->getCurrentChannelConfig().sampleSkip = val;
    }
    double samplingRate = MAX_SAMPLING_RATE/(double)this->config->getCurrentChannelConfig().sampleSkip;
    int unitIndex = 0;

    while(samplingRate>1000)
    {
        unitIndex++;
        samplingRate /= 1000.0;
    }

    this->mainWindow.ui->frequencyValueLabel->setText(
        QString::fromStdString(
            fmt::format("{:.2f} {}Hz", samplingRate, UNIT_PREFIXES[unitIndex])
        )
    );
    spdlog::debug("Setting adq sample skip {}", this->config->getCurrentChannelConfig().sampleSkip);
    spdlog::debug("Set adq sample skip");
}
void Application::changeUL1Bypass(int state) {
    if(state)
        this->config->getCurrentChannelConfig().userLogicBypass |= 0b01;
    else
        this->config->getCurrentChannelConfig().userLogicBypass &= 0b10;
}
void Application::changeUL2Bypass(int state) {
    if(state)
        this->config->getCurrentChannelConfig().userLogicBypass |= 0b10;
    else
        this->config->getCurrentChannelConfig().userLogicBypass &= 0b01;
}
void Application::changeAnalogOffset(double val) {
    spdlog::debug("AnalogoffsetChange");
    int code = mvToADCCode(this->config->getCurrentChannelConfig().inputRangeFloat, val);
    this->mainWindow.ui->analogOffsetCodeInput->blockSignals(true);
    this->mainWindow.ui->analogOffsetCodeInput->setValue(code);
    this->mainWindow.ui->analogOffsetCodeInput->blockSignals(false);
    this->config->getCurrentChannelConfig().dcBias = val;
    this->config->getCurrentChannelConfig().dcBiasCode = code;
    this->adqDevice->SetAdjustableBias(
        this->config->getCurrentChannel()+1,
        this->config->getCurrentChannelConfig().dcBiasCode
    );
    this->scopeUpdater->changePlotTriggerLine(this->config->getCurrentChannelConfig());
    this->updateTriggerLevelDisplays();
}
void Application::changeAnalogOffsetCode(int val) {
    spdlog::debug("AnalogoffsetCodeChange");
    float flt = ADCCodeToMV(this->config->getCurrentChannelConfig().inputRangeFloat, val);
    this->mainWindow.ui->analogOffsetInput->blockSignals(true);
    this->mainWindow.ui->analogOffsetInput->setValue(flt);
    this->mainWindow.ui->analogOffsetInput->blockSignals(false);
    this->config->getCurrentChannelConfig().dcBias = flt;
    this->config->getCurrentChannelConfig().dcBiasCode = val;
    this->adqDevice->SetAdjustableBias(
        this->config->getCurrentChannel()+1,
        this->config->getCurrentChannelConfig().dcBiasCode
    );
    this->scopeUpdater->changePlotTriggerLine(this->config->getCurrentChannelConfig());
    this->updateTriggerLevelDisplays();
}
void Application::changeInputRange(int index) {
    this->config->getCurrentChannelConfig().inputRangeEnum = static_cast<INPUT_RANGES>(index);
    float target = (float)INPUT_RANGE_VALUES[index];
    float result;

    this->adqDevice->SetInputRange(this->config->getCurrentChannel()+1,target,&result);
    this->config->getCurrentChannelConfig().inputRangeFloat = result;
    this->mainWindow.ui->actualInputRangeLabel->setText(
        QString::fromStdString(
            fmt::format("{:.2f} mV", result)
        )
    );
    this->changeAnalogOffsetCode(this->config->getCurrentChannelConfig().dcBiasCode);
    this->changeLevelTriggerCode(this->config->getCurrentChannelConfig().triggerLevelCode);
    this->mainWindow.ui->baseOffsetCalibrationValue->setValue(this->config->getCurrentChannelConfig().getCurrentBaseDCOffset());
    this->mainWindow.ui->digitalOffsetInput->setValue(this->config->getCurrentChannelConfig().getCurrentDigitalOffset());
}
void Application::changeDigitalOffset(int val) {
    this->config->getCurrentChannelConfig().setCurrentDigitalOffset(val);
}
void Application::changeDigitalGain(int val) {
    this->config->getCurrentChannelConfig().digitalGain = val;
}
void Application::changeTriggerMode(int index) {
    QString txt = this->mainWindow.ui->triggerModeSelector->currentText();
    if(txt == "FREE RUNNING")
    {
        spdlog::debug("Trigger mode set to FREE RUNNING");
        this->config->getCurrentChannelConfig().triggerMode = TRIGGER_MODES::SOFTWARE;
        this->mainWindow.ui->limitRecordsCB->setCheckState(Qt::CheckState::Unchecked);
        this->mainWindow.ui->limitRecordsCB->setEnabled(false);
        this->mainWindow.ui->recordCountInput->setEnabled(false);
        this->config->getCurrentChannelConfig().isContinuousStreaming = true;
        this->config->getCurrentChannelConfig().recordCount = -1;
    }
    else
    {
        if(txt == "SOFTWARE")
        {
            spdlog::debug("Trigger mode set to SOFTWARE");
            this->config->getCurrentChannelConfig().triggerMode = TRIGGER_MODES::SOFTWARE;

        }
        else if (txt == "LEVEL")
        {
            spdlog::debug("Trigger mode set to LEVEL");
            this->config->getCurrentChannelConfig().triggerMode = TRIGGER_MODES::LEVEL;
        }
        else if (txt == "EXTERNAL")
        {
            spdlog::warn("Trigger mode set to EXTERNAL. This feature is experimental.");
            this->config->getCurrentChannelConfig().triggerMode = TRIGGER_MODES::EXTERNAL;
        }
        else
        {
            spdlog::error("{} trigger mode is currently unsupported.", txt.toStdString());
        }
        this->mainWindow.ui->limitRecordsCB->setEnabled(true);
        this->mainWindow.ui->recordCountInput->setEnabled(true);
        this->config->getCurrentChannelConfig().isContinuousStreaming = false;
        if(this->mainWindow.ui->limitRecordsCB->checkState())
        {
            this->config->getCurrentChannelConfig().recordCount =
                this->mainWindow.ui->recordCountInput->value();
        }
    }
}
void Application::changeLimitRecords(int state)
{
    if(state) // Checked or Partially checked
    {
        this->config->getCurrentChannelConfig().recordCount = this->mainWindow.ui->recordCountInput->value();
        this->mainWindow.ui->recordCountInput->setEnabled(true);
    }
    else
    {
        this->config->getCurrentChannelConfig().recordCount = ChannelConfiguration::INFINITE_RECORDS;
        this->mainWindow.ui->recordCountInput->setEnabled(false);
    }
}
void Application::changeRecordCount(int val) {
    if(this->mainWindow.ui->limitRecordsCB->checkState())
    {
        this->config->getCurrentChannelConfig().recordCount = val;
    }
    else
    {
        this->config->getCurrentChannelConfig().recordCount = -1;
        this->config->getCurrentChannelConfig().recordLength = 0;
    }
}
void Application::changePretrigger(int val) {
    this->config->getCurrentChannelConfig().pretrigger = val;
    if(val != 0 && this->config->getCurrentChannelConfig().triggerDelay != 0)
    {
        this->changeTriggerDelay(0);
        spdlog::warn("Pretrigger and trigger delay cannot be active at the same time. Disabled delay.");
    }
}
void Application::changeRecordLength(int val) {
    this->config->getCurrentChannelConfig().recordLength = val;

}
void Application::changeTriggerDelay(int val) {
    this->config->getCurrentChannelConfig().triggerDelay = val;
    if(val != 0 && this->config->getCurrentChannelConfig().pretrigger != 0)
    {
        this->changePretrigger(0);
        spdlog::warn("Pretrigger and trigger delay cannot be active at the same time. Disabled pretrigger.");
    }
}
void Application::changeLevelTriggerEdge(int index) {
    this->config->getCurrentChannelConfig().triggerEdge = static_cast<TRIGGER_EDGES>(index);
    this->adqDevice->SetLvlTrigEdge(index);
}
void Application::updateTriggerLevelDisplays()
{
    int zeroBasedTriggerValue = this->config->getCurrentChannelConfig().triggerLevelCode;
    float zeroBasedTriggerValueMv = ADCCodeToMV(this->config->getCurrentChannelConfig().inputRangeFloat, zeroBasedTriggerValue);
    int offsetCodeLevel = zeroBasedTriggerValue - this->config->getCurrentChannelConfig().dcBiasCode;
    float offsetMvLevel = ADCCodeToMV(this->config->getCurrentChannelConfig().inputRangeFloat, offsetCodeLevel);
    this->mainWindow.ui->levelTriggerVoltageInput->blockSignals(true);
    this->mainWindow.ui->levelTriggerCodesInput->blockSignals(true);
    if(this->mainWindow.ui->offsetFromZero->isChecked()) {
        this->mainWindow.ui->levelTriggerCodesInput->setValue(zeroBasedTriggerValue);
        this->mainWindow.ui->levelTriggerVoltageInput->setValue(zeroBasedTriggerValueMv);
    }
    else {
        this->mainWindow.ui->levelTriggerCodesInput->setValue(offsetCodeLevel);
        this->mainWindow.ui->levelTriggerVoltageInput->setValue(offsetMvLevel);
    }
    this->mainWindow.ui->levelTriggerCodesFromZero->setText(QString::asprintf("%d", zeroBasedTriggerValue));
    this->mainWindow.ui->levelTriggerVoltageFromZero->setText(QString::asprintf("%.2f",zeroBasedTriggerValueMv));
    this->mainWindow.ui->levelTriggerVoltageInput->blockSignals(false);
    this->mainWindow.ui->levelTriggerCodesInput->blockSignals(false);
    this->scopeUpdater->changePlotTriggerLine(this->config->getCurrentChannelConfig());
}
void Application::changeLevelTriggerCode(int codeLevel) {
    int zeroBasedTriggerValue;
    if(this->mainWindow.ui->offsetFromZero->isChecked())
        zeroBasedTriggerValue = codeLevel;
    else
        zeroBasedTriggerValue = this->config->getCurrentChannelConfig().dcBiasCode + codeLevel;
    this->config->getCurrentChannelConfig().triggerLevelCode = zeroBasedTriggerValue;
    this->adqDevice->SetLvlTrigLevel(this->config->getCurrentChannelConfig().getDCOffsetTriggerValue());
    this->updateTriggerLevelDisplays();
}
void Application::changeLevelTriggerMV(double mvLevel) {
    int zeroBasedTriggerValue;
    int codeLevel = mvToADCCode(this->config->getCurrentChannelConfig().inputRangeFloat, mvLevel);
    if(this->mainWindow.ui->offsetFromZero->isChecked())
        zeroBasedTriggerValue = codeLevel;
    else
        zeroBasedTriggerValue = this->config->getCurrentChannelConfig().dcBiasCode + codeLevel;
    this->config->getCurrentChannelConfig().triggerLevelCode = zeroBasedTriggerValue;
    this->adqDevice->SetLvlTrigLevel(this->config->getCurrentChannelConfig().getDCOffsetTriggerValue());
    this->updateTriggerLevelDisplays();
}

void Application::changeLevelTriggerReset(int val) {
    this->config->getCurrentChannelConfig().triggerLevelReset = val;
    this->adqDevice->SetTrigLevelResetValue(val);
}
void Application::changeUpdateScope(int state) {
    if(state)
    {
        this->appendRecordProcessor(this->scopeUpdater);
    }
    else
    {
        this->removeRecordProcessor(this->scopeUpdater);
    }

}
void Application::changeAnalyse(int state) {

}
void Application::setFileWriterType(FILE_TYPE_SELECTOR fts)
{
    if(this->fileWriter != nullptr)
    {
        this->removeRecordProcessor(this->fileWriter);
        this->fileWriter.reset();
    }
    switch(fts)
    {
        case FILE_TYPE_SELECTOR::S_BINARY:{
            this->fileWriter = std::make_shared<BinaryFileWriter>(this->config->fileSizeLimit);
        }break;
        case FILE_TYPE_SELECTOR::S_BINARY_VERBOSE:{
            this->fileWriter = std::make_shared<VerboseBinaryWriter>(this->config->fileSizeLimit);
        }break;
        case FILE_TYPE_SELECTOR::S_BINARY_BUFFERED:{
            this->fileWriter = std::make_shared<BufferedBinaryFileWriter>(this->config->fileSizeLimit);
        }break;
        case FILE_TYPE_SELECTOR::S_BINARY_BUFFERED_VERBOSE:{
            this->fileWriter = std::make_shared<VerboseBufferedBinaryWriter>(this->config->fileSizeLimit);
        }break;
        default:{
            spdlog::critical("Unsupported file mode (default)");
            return;
        }break;
    }
    this->appendRecordProcessor(this->fileWriter);
}
void Application::changeSaveToFile(int state) {
    if(state)
    {
        this->setFileWriterType((FILE_TYPE_SELECTOR)this->mainWindow.ui->fileTypeSelector->currentIndex());
    }
    else if(this->fileWriter != nullptr)
    {
        this->removeRecordProcessor(this->fileWriter);
        this->fileWriter.reset();
    }


}
void Application::changeFiletype(int state)
{
    if(this->mainWindow.ui->saveToFileCB->checkState())
    {
        this->setFileWriterType((FILE_TYPE_SELECTOR)state);
    }

}
void Application::primaryButtonPressed() {
    //spdlog::debug("Primary button pressed");
    if(this->acquisition == nullptr)
    {
        spdlog::error("Acquisition not created.");
        return;
    }
    switch(this->acquisition->getState())
    {
        case ACQUISITION_STATES::STOPPED:
        {
            bool success = true;
            success &= this->acquisition->configure(this->config, this->recordProcessors);
            if(this->config->timedRunValue > 0)
            {
                success &= this->acquisition->startTimed(this->config->timedRunValue, this->config->getCurrentChannelConfig().isContinuousStreaming);
            }
            else {
                success &= this->acquisition->start(this->config->getCurrentChannelConfig().isContinuousStreaming);
            }
            if(success) {
                this->mainWindow.ui->streamStartStopButton->setText("Stop");
                this->mainWindow.ui->analysisSettingsContainer->setEnabled(false);
                if(this->mainWindow.ui->resetPlot->checkState())
                {
                    if(this->config->getCurrentChannelConfig().isContinuousStreaming)
                        this->mainWindow.ui->plotArea->xAxis->setRange(0, this->config->transferBufferSize / sizeof(short));
                    else
                        this->mainWindow.ui->plotArea->xAxis->setRange(0, this->config->getCurrentChannelConfig().recordLength);
                    this->mainWindow.ui->plotArea->yAxis->setRange(-(2<<15), 2<<15);
                }
            }
            else
            {
                this->acquisition->stop();
            }
        }
        break;
        case ACQUISITION_STATES::STOPPING:
        {
            spdlog::error("Cannot start nor stop a stopping acquisition. This should not happen.");
        }
        break;
        case ACQUISITION_STATES::RUNNING:
        {
            this->acquisition->stop();
            this->mainWindow.ui->streamStartStopButton->setEnabled(false);
            this->mainWindow.ui->streamStartStopButton->setText("Stopping");
        }
        break;
        default:
        break;
    }
}
void Application::acquisitionStateChanged(ACQUISITION_STATES newState)
{
    switch(newState)
    {
        case ACQUISITION_STATES::STOPPED:
        {
            this->mainWindow.ui->streamStartStopButton->setEnabled(true);
            this->mainWindow.ui->streamStartStopButton->setText("Start");
            this->mainWindow.ui->analysisSettingsContainer->setEnabled(true);
        }
        break;
        case ACQUISITION_STATES::STOPPING:
        {
            this->mainWindow.ui->streamStartStopButton->setEnabled(false);
            this->mainWindow.ui->streamStartStopButton->setText("Stopping");
        }
        break;
        case ACQUISITION_STATES::RUNNING:
        {
        }
        break;
        default:
        break;
    }
}
void Application::updateScope(QVector<double> &x, QVector<double> y)
{
    this->mainWindow.ui->plotArea->graph(0)->setData(x,y);
    //this->mainWindow.ui->plotArea->rescaleAxes();
    this->mainWindow.ui->plotArea->replot();
}
void Application::changeDMABufferCount(unsigned long count)
{
    this->mainWindow.ui->DMAFillStatus->setMaximum(count);
}
void Application::changeBufferQueueCount(unsigned long count)
{
    this->mainWindow.ui->RAMFillStatus->setMaximum(count);
}
void Application::updatePeriodicUIElements()
{
    unsigned long buffersFill = this->acquisition->getBuffersFill();
    int queueFill = this->acquisition->getWriteQueueFill();
    unsigned long long fileFill = 0;
    if(this->fileWriter != nullptr) {
        fileFill = this->fileWriter->getProcessedBytes();
    }

    this->mainWindow.ui->DMAFillStatus->setValue(100ULL*buffersFill / (this->config->transferBufferCount - 1));

    // for some reason the api refuses to fill all buffers, GetDataStreaming will always return bufferCount-1 even when nearly overflowing
    this->mainWindow.ui->RAMFillStatus->setValue(100ULL*queueFill / this->config->writeBufferCount);
    this->mainWindow.ui->FileFillStatus->setValue(100ULL*fileFill / this->config->fileSizeLimit);

}
void Application::createPeriodicUpdateTimer(unsigned long period)
{
    this->updateTimer = std::unique_ptr<QTimer>(new QTimer(this));
    this->updateTimer->connect(
        this->updateTimer.get(),
        &QTimer::timeout,
        this,
        &Application::updatePeriodicUIElements
    );
    this->updateTimer->start();
}
void Application::configureDMABuffers()
{
    this->buffersConfigurationDialog->ui->dmaBufferCount->setValue(this->config->transferBufferCount);
    this->buffersConfigurationDialog->ui->dmaBufferSize->setValue(this->config->transferBufferSize);
    this->buffersConfigurationDialog->ui->writeBufferCount->setValue(this->config->writeBufferCount);
    this->buffersConfigurationDialog->ui->maximumFileSize->setValue((double)this->config->fileSizeLimit/BuffersDialog::FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER);
    this->buffersConfigurationDialog->show();
}
void Application::onDMADialogClosed()
{
    unsigned long newBufferCount = this->buffersConfigurationDialog->ui->dmaBufferCount->value();
    unsigned long newBufferSize = this->buffersConfigurationDialog->ui->dmaBufferSize->value();
    unsigned long newQueueCount = this->buffersConfigurationDialog->ui->writeBufferCount->value();
    unsigned long long newFileLimit = static_cast<unsigned long long>(this->buffersConfigurationDialog->ui->maximumFileSize->value()*BuffersDialog::FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER);
    this->config->transferBufferCount = newBufferCount;
    this->config->transferBufferSize = newBufferSize;
    this->config->writeBufferCount = newQueueCount;
    this->config->fileSizeLimit = newFileLimit;
    spdlog::debug("Modified buffers config to {} buffers of {}B with a {} queue and {}B file.", newBufferCount, newBufferSize, newQueueCount, newFileLimit);
}
void Application::configureULRegisters()
{
    this->registerDialog->show();
}

void Application::onRegisterDialogClosed()
{
    unsigned int algorithmMode = this->registerDialog->ui->triggerAlgoithmSelector->currentIndex();
    unsigned int activeChannels =
        (this->registerDialog->ui->activeChannel1->checkState()>0 ? 1<<0 : 0 ) |
        (this->registerDialog->ui->activeChannel2->checkState()>0 ? 1<<1 : 0 ) |
        (this->registerDialog->ui->activeChannel3->checkState()>0 ? 1<<2 : 0 ) |
        (this->registerDialog->ui->activeChannel4->checkState()>0 ? 1<<3 : 0 );
    unsigned int passthrough =
        (this->registerDialog->ui->passthroughChannel1->checkState()>0 ? 1<<0 : 0 ) |
        (this->registerDialog->ui->passthroughChannel2->checkState()>0 ? 1<<1 : 0 ) |
        (this->registerDialog->ui->passthroughChannel3->checkState()>0 ? 1<<2 : 0 ) |
        (this->registerDialog->ui->passthroughChannel4->checkState()>0 ? 1<<3 : 0 ) |
        (this->registerDialog->ui->moduloPassthrough->checkState()>0 ? 1<<4 : 0 )    ;

    unsigned int retval;
    unsigned int algorithmConfig = ( algorithmMode | (activeChannels << 4) | ( passthrough << 8 ));
    this->adqDevice->WriteUserRegister(1, 0x10, 0, algorithmConfig, &retval);
    if(retval != algorithmConfig) spdlog::debug("Failed to set algorithm configuration");

    short dcOffsetValue = this->registerDialog->ui->algorithmParamInput0->value();
    spdlog::debug("Setting DC offset register to {}", dcOffsetValue);
    this->adqDevice->WriteUserRegister(1, 0x11, 0, dcOffsetValue, &retval);
    if(dcOffsetValue != (short)retval) spdlog::debug("Failed to set DC offset register");


    short algParam1 = this->registerDialog->ui->algorithmParamInput1->value();
    spdlog::debug("Setting alg_param1 register to {}", algParam1);
    this->adqDevice->WriteUserRegister(1, 0x12, 0, algParam1, &retval);
    if(algParam1 != (short)retval) spdlog::debug("Failed to set algParam1 register");

}
void Application::triggerSoftwareTrig()
{
    this->adqDevice->SWTrig();
}

void Application::loadConfig()
{
    QString fileName = QFileDialog::getOpenFileName(
        &this->mainWindow,
        "Open config",
        "",
        "JSON config (*.json);;All Files (*)"
    );
    if(!fileName.length()) return;
    QByteArray ba = fileName.toLocal8Bit();
    bool success = this->config->fromFile(ba.data());
    if(!success)
    {
        spdlog::warn("Failed to load configuration from file {}", ba.data());
    }

    this->updateTriggerLevelDisplays();
}

void Application::saveConfig()
{
    QString fileName = QFileDialog::getSaveFileName(
        &this->mainWindow,
        "Save config",
        "",
        "JSON config (*.json);;All Files (*)"
    );
    if(!fileName.length()) return;
    QByteArray ba = fileName.toLocal8Bit();
    this->config->toFile(ba.data());
}

void Application::changeTimedRunEnabled(int state)
{
    if(state)
    {
        this->config->timedRunValue = this->mainWindow.ui->timedRunValue->value();
    }
    else
    {
        this->config->timedRunValue = 0;
    }
}
void Application::changeTimedRunValue(int val)
{
    if(this->mainWindow.ui->timedRunCheckbox->checkState())
    {
        this->config->timedRunValue = this->mainWindow.ui->timedRunValue->value();
    }
}
void Application::changeBaseDCBias(int val)
{
    this->config->getCurrentChannelConfig().setCurrentBaseDCOffset(val);
}
void Application::openCalibrateDialog()
{
    this->autoCalibrateDialog->show();
}
void Application::useCalculatedOffset(CALIBRATION_MODES mode, int offset)
{
    switch(mode)
    {
        case CALIBRATION_MODES::ANALOG:
            this->config->getCurrentChannelConfig().setCurrentBaseDCOffset(offset);
        break;
        case CALIBRATION_MODES::FINE_DIGITAL: case CALIBRATION_MODES::DIGITAL:
            this->config->getCurrentChannelConfig().setCurrentDigitalOffset(-offset);
        break;
    }
    spdlog::debug("Should update offsets to {} {}", this->config->getCurrentChannelConfig().getCurrentBaseDCOffset(), this->config->getCurrentChannelConfig().getCurrentDigitalOffset());
    this->mainWindow.ui->baseOffsetCalibrationValue->setValue(this->config->getCurrentChannelConfig().getCurrentBaseDCOffset());
    this->mainWindow.ui->digitalOffsetInput->setValue(this->config->getCurrentChannelConfig().getCurrentDigitalOffset());
}
void Application::flushDMA()
{
    this->adqDevice->FlushDMA();
}
