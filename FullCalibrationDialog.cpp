#include "FullCalibrationDialog.h"
#include "ui_FullCalibrationDialog.h"

FullCalibrationDialog::FullCalibrationDialog(std::shared_ptr<ApplicationConfiguration> appConfig, std::shared_ptr<Acquisition> acq, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FullCalibrationDialog)
{
    ui->setupUi(this);
    this->acquisition = acq;
    this->calibrationConfiguration = std::make_shared<ApplicationConfiguration>(*(appConfig.get()));
    this->calibrationTable = std::unique_ptr<CalibrationTable>(new CalibrationTable());
    this->appConfig = appConfig;
    this->parameterComputer = std::make_shared<SignalParameterComputer>(this->calibrationConfiguration->fileSizeLimit);
    this->ui->startCalibration->connect(
        this->ui->startCalibration,
        &QAbstractButton::pressed,
        this,
        &FullCalibrationDialog::start
    );
    this->ui->apply->connect(
        this->ui->apply,
        &QAbstractButton::pressed,
        this,
        &FullCalibrationDialog::apply
    );
    this->ui->save->connect(
        this->ui->save,
        &QAbstractButton::pressed,
        this,
        &FullCalibrationDialog::save
    );
    this->ui->load->connect(
        this->ui->load,
        &QAbstractButton::pressed,
        this,
        &FullCalibrationDialog::load
    );
    this->ui->inputRangeView->connect(
        this->ui->inputRangeView,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ this->changeInputRange(index); }
    );
    this->digitalValues[0] = this->ui->do0;
    this->digitalValues[1] = this->ui->do1;
    this->digitalValues[2] = this->ui->do2;
    this->digitalValues[3] = this->ui->do3;

    this->analogValues[0] = this->ui->ao0;
    this->analogValues[1] = this->ui->ao1;
    this->analogValues[2] = this->ui->ao2;
    this->analogValues[3] = this->ui->ao3;
}

FullCalibrationDialog::~FullCalibrationDialog()
{
    delete ui;
}
void FullCalibrationDialog::changeInputRange(int v)
{
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        this->digitalValues[ch]->setValue(this->calibrationTable->digitalOffset[ch][v]);
        this->analogValues[ch]->setValue(this->calibrationTable->analogOffset[ch][v]);
    }
}

void FullCalibrationDialog::configureNextStage()
{
    this->calibrationConfiguration->setCurrentChannel(this->setups[currentSetupIndex].channel);
    this->calibrationConfiguration->getCurrentChannelConfig().setInputRange((INPUT_RANGES)this->setups[currentSetupIndex].inputRange);
    this->calibrationConfiguration->getCurrentChannelConfig().setCurrentBaseDCOffset(
        calibrationTable->analogOffset[this->setups[currentSetupIndex].channel][this->setups[currentSetupIndex].inputRange]
    );
    this->calibrationConfiguration->getCurrentChannelConfig().setCurrentDigitalOffset(
        calibrationTable->digitalOffset[this->setups[currentSetupIndex].channel][this->setups[currentSetupIndex].inputRange]
    );
    this->calibrationConfiguration->getCurrentChannelConfig().sampleSkip = this->ui->sampleSkip->value();

    this->calibrationConfiguration->transferBufferCount = 16;
    this->calibrationConfiguration->transferBufferSize = 4ULL*1024ULL*1024ULL;
    this->calibrationConfiguration->writeBufferCount = 64;

    this->calibrationConfiguration->getCurrentChannelConfig().dcBias = 0;
    this->calibrationConfiguration->getCurrentChannelConfig().dcBiasCode = 0;
    this->calibrationConfiguration->getCurrentChannelConfig().isContinuousStreaming = true;
    this->calibrationConfiguration->getCurrentChannelConfig().recordCount = -1;
    this->calibrationConfiguration->getCurrentChannelConfig().triggerMode = TRIGGER_MODES::SOFTWARE;
    this->calibrationConfiguration->getCurrentChannelConfig().updateScope = false;
    this->calibrationConfiguration->getCurrentChannelConfig().userLogicBypass = 0b1111;
    this->calibrationConfiguration->secondChannel = CHANNEL_DISABLED;
}
void FullCalibrationDialog::appendStage(int ch, int inrange, int mode)
{
    FullCalibrationSetup s1;
    s1.channel = ch;
    s1.inputRange = inrange;
    if(mode == FINE_DIGITAL)
    {
        s1.mode = CALIBRATION_MODES::ANALOG;
        FullCalibrationSetup s2;
        s2.channel = ch;
        s2.inputRange = inrange;
        s2.mode = CALIBRATION_MODES::FINE_DIGITAL;
        this->setups.push_back(s1);
        this->setups.push_back(s2);
    }
    else
    {
        s1.mode = (CALIBRATION_MODES)mode;
        this->setups.push_back(s1);
    }
    // reset affected cells:
    this->calibrationTable->analogOffset[ch][inrange] = 0;
    this->calibrationTable->digitalOffset[ch][inrange] = 0;
}
void FullCalibrationDialog::start()
{
    spdlog::debug("Starting full calibration");
    if(this->ui->modeAnalog->isChecked()) {
        this->mode = CALIBRATION_MODES::ANALOG;
    } else if(this->ui->modeDigital->isChecked()) {
        this->mode = CALIBRATION_MODES::DIGITAL;
    } else if(this->ui->modeCombined->isChecked()) {
        this->mode = CALIBRATION_MODES::FINE_DIGITAL;
    }
    this->blockUI();
    connect(this->acquisition.get(), &Acquisition::onStateChanged, this, &FullCalibrationDialog::onStateChanged);
    this->setups.clear();
    unsigned char channelMask =
        (this->ui->channelSelection0->checkState()?(1<<0):0) |
        (this->ui->channelSelection1->checkState()?(1<<1):0) |
        (this->ui->channelSelection2->checkState()?(1<<2):0) |
        (this->ui->channelSelection3->checkState()?(1<<3):0);
    if(this->ui->rangeSingle->isChecked())
    {
        for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
        {
            if(!((1<<ch) & channelMask)) continue;
            this->appendStage(ch, this->ui->inputRangeView->currentIndex(), this->mode);
        }
    }
    else
    {
        for(int inrange = 0; inrange < INPUT_RANGE_COUNT; inrange++)
        {
            for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
            {
                if(!((1<<ch) & channelMask)) continue;
                this->appendStage(ch, inrange, this->mode);
            }

        }
    }
    spdlog::debug("Running first stage");
    this->acquisitionActive = true;
    this->ui->progressBar->setValue(0);
    if(this->runStage())
    {
        this->ui->hintLabel->setText("Calibration started. This will take a while.");
    }
    else
    {
        this->ui->hintLabel->setText("Calibration failed.");
        this->stopAcquisitions();
    }
}
bool FullCalibrationDialog::moveToNextStage()
{
    this->currentSetupIndex += 1;
    if (this->currentSetupIndex >= this->setups.size())
    {
        return false;
    }
    return true;
}
bool FullCalibrationDialog::runStage()
{
    spdlog::debug("Running stage");
    this->ui->currentStage->setText(
        QString::fromStdString(
            fmt::format("CH{}: {} mV", this->setups[this->currentSetupIndex].channel+1, INPUT_RANGE_VALUES[this->setups[this->currentSetupIndex].inputRange])
        )
    );
    this->configureNextStage();
    this->parameterComputer->startNewStream(*(this->calibrationConfiguration));
    std::list<std::shared_ptr<RecordProcessor>> rps;
    rps.push_back(this->parameterComputer);
    unsigned long durationMs = this->ui->calibrationDuration->value();
    bool success = true;
    success &= this->acquisition->configure(this->calibrationConfiguration, rps);
    if(!success) {
        spdlog::debug("Acquisition configuration failed.");
        this->ui->hintLabel->setText("Calibration failed.");
        return false;
    }
    if(durationMs > 0)
    {
        success &= this->acquisition->startTimed(durationMs, true);
    }
    else {
        spdlog::debug("Acqusistion start failed.");
        this->ui->hintLabel->setText("Calibration failed.");
        return false;
    }
    if(success) {
    }
    else
    {
        spdlog::debug("Acqusistion start failed.");
        this->ui->hintLabel->setText("Calibration failed.");
        this->acquisition->stop();
        return false;
    }
    spdlog::debug("Running stage {}: m:{} ch:{} ir:{}", this->currentSetupIndex, this->setups[this->currentSetupIndex].mode, this->setups[this->currentSetupIndex].channel, this->setups[this->currentSetupIndex].inputRange);
    return true;
}
void FullCalibrationDialog::onStateChanged(ACQUISITION_STATES newState)
{
    if(!this->acquisitionActive)
    {
        spdlog::debug("Acquisition was canceled.");
        this->ui->hintLabel->setText("Calibration cancelled.");
        this->stopAcquisitions();
        return;
    }
    if(newState == ACQUISITION_STATES::STOPPED)
    {
        spdlog::debug("Calibration stage finished. Computing average.");
        std::unique_ptr<SignalParameters> sp = this->parameterComputer->getResults();
        spdlog::debug("Average obtained {}", sp->average);
        this->ui->inputRangeView->setCurrentIndex(this->setups[this->currentSetupIndex].inputRange);
        int channel = this->setups[this->currentSetupIndex].channel;

        if(this->setups[this->currentSetupIndex].mode == CALIBRATION_MODES::ANALOG){
            this->analogValues[channel]->setValue(-sp->average);
        }
        else {
            this->digitalValues[channel]->setValue(sp->average);
        }

        if(this->setups[this->currentSetupIndex].mode == CALIBRATION_MODES::ANALOG)
        {
            this->calibrationTable->analogOffset[this->setups[currentSetupIndex].channel][this->setups[currentSetupIndex].inputRange]
                = -sp->average;
        }
        else
        {
            this->calibrationTable->digitalOffset[this->setups[currentSetupIndex].channel][this->setups[currentSetupIndex].inputRange]
                = sp->average;
        }
        if(!this->moveToNextStage()) // if no more stages
        {
            spdlog::debug("All calibration acquisitions finished");
            this->ui->hintLabel->setText("Calibration finished. Terminate channels before starting again. You can edit the obtained values before applying them.");
            this->stopAcquisitions();
            this->ui->progressBar->setValue(100);

        }
        else
        {
            this->ui->progressBar->setValue(100*this->currentSetupIndex/this->setups.size());

            if(!this->runStage()) {
                this->stopAcquisitions();
            }

        }

    }
}
void FullCalibrationDialog::stopAcquisitions()
{
    this->acquisitionActive = false;
    this->acquisition->disconnect(this);
    this->unblockUI();
}
void FullCalibrationDialog::blockUI()
{
    this->setEnabled(false);
}
void FullCalibrationDialog::unblockUI()
{
    this->setEnabled(true);
}
void FullCalibrationDialog::apply()
{
    unsigned char channelMask =
        (this->ui->channelSelection0->checkState()?(1<<0):0) |
        (this->ui->channelSelection1->checkState()?(1<<1):0) |
        (this->ui->channelSelection2->checkState()?(1<<2):0) |
        (this->ui->channelSelection3->checkState()?(1<<3):0);
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        if(!((1<<ch) & channelMask)) continue;
        if(this->ui->rangeSingle->isChecked())
        {
            int inputRange = this->ui->inputRangeView->currentIndex();
            this->appConfig->channelConfig[ch].baseDcBiasOffset[inputRange] = this->calibrationTable->analogOffset[ch][inputRange];
            this->appConfig->channelConfig[ch].digitalOffset[inputRange] = this->calibrationTable->digitalOffset[ch][inputRange];
            continue;
        }
        for(int inrange = 0; inrange < INPUT_RANGE_COUNT; inrange++)
        {
            this->appConfig->channelConfig[ch].baseDcBiasOffset[inrange] = this->calibrationTable->analogOffset[ch][inrange];
            this->appConfig->channelConfig[ch].digitalOffset[inrange] = this->calibrationTable->digitalOffset[ch][inrange];
        }
    }

}
void FullCalibrationDialog::load()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Load calibration table",
        "",
        "JSON config (*.json);;All Files (*)"
    );
    if(!fileName.length()) return;
    QByteArray ba = fileName.toLocal8Bit();
    bool success = this->calibrationTable->fromJSON(ba.data());
    if(!success)
    {
        spdlog::warn("Failed to load configuration from file {}", ba.data());
        return;
    }
    changeInputRange(this->ui->inputRangeView->currentIndex());
}
void FullCalibrationDialog::save()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save calibration table",
        "",
        "JSON table (*.json);;All Files (*)"
    );
    if(!fileName.length()) return;
    QByteArray ba = fileName.toLocal8Bit();
    this->calibrationTable->toJSON(ba.data());
}
