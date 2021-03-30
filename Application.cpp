#include "Application.h"
#include "MainWindow.h"
#include "Acquisition.h"
#include "./ui_MainWindow.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "BinaryFileWriter.h"
#include <limits>
Application::Application( MainWindow& mainWindow) : mainWindow(mainWindow)
{

}
int Application::start(int argc, char *argv[]) {
    this->adqControlUnit = CreateADQControlUnit();
    if(this->adqControlUnit == NULL)
    {
        spdlog::error("Failed to create ADQControlUnit. Exiting...");
        return -1;
    }

    // parse args here
    ADQControlUnit_EnableErrorTrace(this->adqControlUnit, std::max((int)this->config.deviceConfig.adqLoggingLevel, 3), "."); // log to root dir, LOGGING_LEVEL::DEBUG is 4 but API only supports INFO=3
    ADQControlUnit_FindDevices(this->adqControlUnit);
    int numberOfDevices = ADQControlUnit_NofADQ(adqControlUnit);
    if(numberOfDevices == 0)
    {
        spdlog::error("No ADQ devices found. Exiting.");
        return -1;
    }
    else if(numberOfDevices != 1)
    {
        spdlog::warn("Found {} devices. Using {}.", numberOfDevices, this->config.deviceNumber);
    }
    this->adqDevice =
            ADQControlUnit_GetADQ(adqControlUnit, this->config.deviceNumber);
    this->adqDevice->StopStreaming();
    this->scopeUpdater = std::unique_ptr<ScopeUpdater>(
        new ScopeUpdater(
            this->config.getCurrentChannelConfig().recordLength,
            *this->mainWindow.ui->plotArea
        )
    );
    this->acquisition = std::unique_ptr<Acquisition>(
        new Acquisition(
            this->config,
            *this->adqDevice
        )
    );
    this->linkSignals();
    this->setUI();
    //this->config.toFile("test_json.scfg");
}


void Application::setUI()
{
    // This is a temporary(hopefully) hack.
    int actualChannel = this->config.getCurrentChannel();
    this->mainWindow.ui->channelComboBox->setCurrentIndex((this->config.getCurrentChannel()+1)%MAX_NOF_CHANNELS);
    this->mainWindow.ui->channelComboBox->setCurrentIndex(actualChannel);
    this->changeDMABufferCount(this->config.deviceConfig.transferBufferCount);
}

float ADCCodeToMV(float inputRange, int code)
{
    return (float)code * (inputRange/2) / std::pow(2,15);
}


int mvToADCCode(float inputRange, float value)
{
    return std::round ( value / ( inputRange / 2 ) * std::pow(2,15) );
}

void Application::linkSignals()
{
    // CHANNEL
    this->mainWindow.ui->channelComboBox->connect(
        this->mainWindow.ui->channelComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ this->changeChannel(index); }
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
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int i){ this->changeAnalogOffset(i); }
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
}

///// BEGIN UI SLOTS //////

void Application::changeChannel(int channel) {
    this->config.setCurrentChannel(channel);
    this->mainWindow.ui->sampleSkipInput->setValue(this->config.getCurrentChannelConfig().sampleSkip);
    this->mainWindow.ui->inputRangeSelector->setCurrentIndex(this->config.getCurrentChannelConfig().inputRangeEnum);
    this->mainWindow.ui->ulBypass1->setCheckState(
        this->config.getCurrentChannelConfig().userLogicBypass&0b01?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
    this->mainWindow.ui->ulBypass2->setCheckState(
        this->config.getCurrentChannelConfig().userLogicBypass&0b10?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
    this->mainWindow.ui->analogOffsetInput->setValue(this->config.getCurrentChannelConfig().dcBias);
    this->mainWindow.ui->digitalOffsetInput->setValue(this->config.getCurrentChannelConfig().digitalOffset);
    this->mainWindow.ui->digitalGainInput->setValue(this->config.getCurrentChannelConfig().digitalGain);
    switch(this->config.getCurrentChannelConfig().triggerMode)
    {
        case TRIGGER_MODES::SOFTWARE:
        {
            if(this->config.getCurrentChannelConfig().isContinuousStreaming)
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
            if(this->config.getCurrentChannelConfig().recordCount <= 0)
            {
                this->mainWindow.ui->triggerModeSelector->setCurrentIndex(TRIGGER_MODE_SELECTOR::S_FREE_RUNNING);
            }
            else
            {
                this->mainWindow.ui->triggerModeSelector->setCurrentIndex(TRIGGER_MODE_SELECTOR::S_SOFTWARE);
            }

        }
        break;
        default:
        {
            spdlog::error("Unsupported trigger mode {}.", this->config.getCurrentChannelConfig().triggerMode);
        }
        break;
    }
    if(this->config.getCurrentChannelConfig().recordCount>MAX_FINITE_RECORD_COUNT)
    {
        this->mainWindow.ui->limitRecordsCB->setCheckState(Qt::CheckState::Unchecked);
    }
    else
    {
        this->mainWindow.ui->limitRecordsCB->setCheckState(Qt::CheckState::Checked);
        this->mainWindow.ui->recordCountInput->setValue(this->config.getCurrentChannelConfig().recordCount);
    }
    this->mainWindow.ui->pretriggerInput->setValue(this->config.getCurrentChannelConfig().pretrigger);
    this->mainWindow.ui->recordLengthInput->setValue(this->config.getCurrentChannelConfig().recordLength);
    this->mainWindow.ui->recordLengthInput->setValue(this->config.getCurrentChannelConfig().triggerDelay);
    this->mainWindow.ui->levelTriggerEdgeSelector->setCurrentIndex(this->config.getCurrentChannelConfig().triggerEdge);
}
void Application::changeSampleSkip(int val) {
    if(val==3)
    {
        spdlog::info("Sample skip cannot be 3.");
        if(this->config.getCurrentChannelConfig().sampleSkip<3)
        {
            this->config.getCurrentChannelConfig().sampleSkip = 4;
            this->mainWindow.ui->sampleSkipInput->blockSignals(true);
            this->mainWindow.ui->sampleSkipInput->setValue(4);
            this->mainWindow.ui->sampleSkipInput->blockSignals(false);
        }
        else
        {
            this->config.getCurrentChannelConfig().sampleSkip = 2;
            this->mainWindow.ui->sampleSkipInput->blockSignals(true);
            this->mainWindow.ui->sampleSkipInput->setValue(2);
            this->mainWindow.ui->sampleSkipInput->blockSignals(false);
        }
    }
    else
    {
        this->config.getCurrentChannelConfig().sampleSkip = val;
    }
    double samplingRate = MAX_SAMPLING_RATE/(double)this->config.getCurrentChannelConfig().sampleSkip;
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
    spdlog::debug("Setting adq sample skip {}", this->config.getCurrentChannelConfig().sampleSkip);
    this->adqDevice->SetSampleSkip(this->config.getCurrentChannelConfig().sampleSkip);
    spdlog::debug("Set adq sample skip");
}
void Application::changeUL1Bypass(int state) {
    if(state)
        this->config.getCurrentChannelConfig().userLogicBypass |= 0b01;
    else
        this->config.getCurrentChannelConfig().userLogicBypass &= 0b10;
    this->adqDevice->BypassUserLogic(1, state?1:0);
}
void Application::changeUL2Bypass(int state) {
    if(state)
        this->config.getCurrentChannelConfig().userLogicBypass |= 0b10;
    else
        this->config.getCurrentChannelConfig().userLogicBypass &= 0b01;
    this->adqDevice->BypassUserLogic(2, state?1:0);
}
void Application::changeAnalogOffset(int val) {
    int code = mvToADCCode(this->config.getCurrentChannelConfig().inputRangeFloat, val);
    this->mainWindow.ui->analogOffsetCodeInput->blockSignals(true);
    this->mainWindow.ui->analogOffsetCodeInput->setValue(code);
    this->mainWindow.ui->analogOffsetCodeInput->blockSignals(false);
    this->config.getCurrentChannelConfig().dcBias = val;
    this->config.getCurrentChannelConfig().dcBiasCode = code;
    this->adqDevice->SetAdjustableBias(
        this->config.getCurrentChannel()+1,
        this->config.getCurrentChannelConfig().dcBiasCode
    );
}
void Application::changeAnalogOffsetCode(int val) {
    int flt = ADCCodeToMV(this->config.getCurrentChannelConfig().inputRangeFloat, val);
    this->mainWindow.ui->analogOffsetInput->blockSignals(true);
    this->mainWindow.ui->analogOffsetInput->setValue(flt);
    this->mainWindow.ui->analogOffsetInput->blockSignals(false);
    this->config.getCurrentChannelConfig().dcBias = flt;
    this->config.getCurrentChannelConfig().dcBiasCode = val;
    this->adqDevice->SetAdjustableBias(
        this->config.getCurrentChannel()+1,
        this->config.getCurrentChannelConfig().dcBiasCode
    );
}
void Application::changeInputRange(int index) {
    this->config.getCurrentChannelConfig().inputRangeEnum = static_cast<INPUT_RANGES>(index);
    float target = (float)INPUT_RANGE_VALUES[index];
    float result;
    this->adqDevice->SetInputRange(this->config.getCurrentChannel()+1,target,&result);
    this->config.getCurrentChannelConfig().inputRangeFloat = result;
    this->mainWindow.ui->actualInputRangeLabel->setText(
        QString::fromStdString(
            fmt::format("{:.2f} mV", result)
        )
    );
}
void Application::changeDigitalOffset(int val) {
    this->config.getCurrentChannelConfig().digitalOffset = val;
    this->adqDevice->SetGainAndOffset(
        this->config.getCurrentChannel()+1,
        this->config.getCurrentChannelConfig().digitalGain,
        this->config.getCurrentChannelConfig().digitalOffset
    );
}
void Application::changeDigitalGain(int val) {
    this->config.getCurrentChannelConfig().digitalGain = val;
    this->adqDevice->SetGainAndOffset(
        this->config.getCurrentChannel()+1,
        this->config.getCurrentChannelConfig().digitalGain,
        this->config.getCurrentChannelConfig().digitalOffset
    );
}
void Application::changeTriggerMode(int index) {
    QString txt = this->mainWindow.ui->triggerModeSelector->currentText();
    if(txt == "FREE RUNNING")
    {
        spdlog::debug("Trigger mode set to FREE RUNNING");
        this->config.getCurrentChannelConfig().triggerMode = TRIGGER_MODES::SOFTWARE;
        this->mainWindow.ui->limitRecordsCB->setCheckState(Qt::CheckState::Unchecked);
        this->mainWindow.ui->limitRecordsCB->setEnabled(false);
        this->adqDevice->SetTriggerMode(this->config.getCurrentChannelConfig().triggerMode);
        this->config.getCurrentChannelConfig().isContinuousStreaming = true;
    }
    else if(txt == "SOFTWARE")
    {
        spdlog::debug("Trigger mode set to SOFTWARE");
        this->config.getCurrentChannelConfig().triggerMode = TRIGGER_MODES::SOFTWARE;
        this->mainWindow.ui->limitRecordsCB->setEnabled(true);
        this->adqDevice->SetTriggerMode(this->config.getCurrentChannelConfig().triggerMode);
        this->config.getCurrentChannelConfig().isContinuousStreaming = false;
    }
    else if (txt == "LEVEL")
    {
        spdlog::debug("Trigger mode set to LEVEL");
        this->mainWindow.ui->limitRecordsCB->setEnabled(true);
        this->config.getCurrentChannelConfig().triggerMode = TRIGGER_MODES::LEVEL;
        this->adqDevice->SetTriggerMode(this->config.getCurrentChannelConfig().triggerMode);
        this->config.getCurrentChannelConfig().isContinuousStreaming = false;
    }
    else if (txt == "EXTERNAL")
    {
        spdlog::warn("Trigger mode set to EXTERNAL. This feature is experimental.");
        this->mainWindow.ui->limitRecordsCB->setEnabled(true);
        this->config.getCurrentChannelConfig().triggerMode = TRIGGER_MODES::EXTERNAL;
        this->adqDevice->SetTriggerMode(this->config.getCurrentChannelConfig().triggerMode);
        this->config.getCurrentChannelConfig().isContinuousStreaming = false;
    }
    else
    {
        spdlog::error("{} trigger mode is currently unsupported.", txt.toStdString());
    }
}
void Application::changeLimitRecords(int state)
{
    if(state) // Checked or Partially checked
    {

        this->config.getCurrentChannelConfig().recordCount = this->mainWindow.ui->recordCountInput->value();
        this->mainWindow.ui->recordCountInput->setEnabled(true);
    }
    else
    {
        this->config.getCurrentChannelConfig().recordCount = -1;
        this->mainWindow.ui->recordCountInput->setEnabled(false);
    }
}
void Application::changeRecordCount(int val) {
    if(this->mainWindow.ui->limitRecordsCB->checkState())
    {
        this->config.getCurrentChannelConfig().recordCount = val;
    }
    else
    {
        this->config.getCurrentChannelConfig().recordCount = -1;
        this->config.getCurrentChannelConfig().recordLength = 0;
    }
}
void Application::changePretrigger(int val) {
    this->config.getCurrentChannelConfig().pretrigger = val;
    if(val != 0 && this->config.getCurrentChannelConfig().triggerDelay != 0)
    {
        this->changeTriggerDelay(0);
        spdlog::warn("Pretrigger and trigger delay cannot be active at the same time. Disabled delay.");
    }
}
void Application::changeRecordLength(int val) {
    this->config.getCurrentChannelConfig().recordLength = val;

}
void Application::changeTriggerDelay(int val) {
    this->config.getCurrentChannelConfig().triggerDelay = val;
    if(val != 0 && this->config.getCurrentChannelConfig().pretrigger != 0)
    {
        this->changePretrigger(0);
        spdlog::warn("Pretrigger and trigger delay cannot be active at the same time. Disabled pretrigger.");
    }
}
void Application::changeLevelTriggerEdge(int index) {
    this->config.getCurrentChannelConfig().triggerEdge = static_cast<TRIGGER_EDGES>(index);
    this->adqDevice->SetLvlTrigEdge(index);
}
void Application::changeLevelTriggerCode(int val) {
    this->config.getCurrentChannelConfig().triggerLevelCode = val;
    this->adqDevice->SetLvlTrigLevel(val);
    double mvVal = ADCCodeToMV(this->config.getCurrentChannelConfig().inputRangeFloat, val);
    this->mainWindow.ui->levelTriggerVoltageInput->blockSignals(true);
    this->mainWindow.ui->levelTriggerVoltageInput->setValue(mvVal);
    this->mainWindow.ui->levelTriggerVoltageInput->blockSignals(false);
}
void Application::changeLevelTriggerMV(double val) {
    int code = mvToADCCode(this->config.getCurrentChannelConfig().inputRangeFloat, val);
    this->config.getCurrentChannelConfig().triggerLevelCode = code;
    this->adqDevice->SetLvlTrigLevel(this->config.getCurrentChannelConfig().triggerLevelCode);
    this->mainWindow.ui->levelTriggerCodesInput->blockSignals(true);
    this->mainWindow.ui->levelTriggerCodesInput->setValue(this->config.getCurrentChannelConfig().triggerLevelCode);
    this->mainWindow.ui->levelTriggerCodesInput->blockSignals(false);
}

void Application::changeUpdateScope(int state) {
    if(state)
    {
        this->acquisition->appendRecordProcessor(this->scopeUpdater);
    }
    else
    {
        this->acquisition->removeRecordProcessor(this->scopeUpdater);
    }

}
void Application::changeAnalyse(int state) {

}
void Application::changeSaveToFile(int state) {
    if(state)
    {
        switch(this->mainWindow.ui->fileTypeSelector->currentIndex())
        {
            case FILE_TYPE_SELECTOR::S_BINARY:{
                this->fileWriter = std::make_shared<BinaryFileWriter>();
            }break;
            case FILE_TYPE_SELECTOR::S_ASCII:{
                spdlog::error("ASCII is an unsupported file mode");
                return;
            }break;
            case FILE_TYPE_SELECTOR::S_HDF5:{
                spdlog::error("HDF5 is an unsupported file mode");
                return;
            }break;
            default:{
                spdlog::critical("Unsupported file mode (default)");
                return;
            }break;
        }
        this->acquisition->appendRecordProcessor(this->fileWriter);
    }
    else if(this->fileWriter != nullptr)
    {
        this->acquisition->removeRecordProcessor(this->fileWriter);
        this->fileWriter.reset();
    }

}
void Application::changeFiletype(int state) {

}
void Application::primaryButtonPressed() {
    spdlog::debug("Primary button pressed");
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
            success &= this->acquisition->configure();
            success &= this->acquisition->start();
            if(success) {
                this->mainWindow.ui->streamStartStopButton->setText("Stop");
                this->mainWindow.ui->analysisSettingsContainer->setEnabled(false);
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
    this->mainWindow.ui->plotArea->rescaleAxes();
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