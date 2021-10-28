#include "FullCalibrationDialog.h"
#include "ui_FullCalibrationDialog.h"

FullCalibrationDialog::FullCalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FullCalibrationDialog)
{
    ui->setupUi(this);

    this->digitalValues[0] = this->ui->do0;
    this->digitalValues[1] = this->ui->do1;
    this->digitalValues[2] = this->ui->do2;
    this->digitalValues[3] = this->ui->do3;

    this->analogValues[0] = this->ui->ao0;
    this->analogValues[1] = this->ui->ao1;
    this->analogValues[2] = this->ui->ao2;
    this->analogValues[3] = this->ui->ao3;

    this->labels[0] = this->ui->ch1label;
    this->labels[1] = this->ui->ch2label;
    this->labels[2] = this->ui->ch3label;
    this->labels[3] = this->ui->ch4label;
}

FullCalibrationDialog::~FullCalibrationDialog()
{
    delete ui;
}

void FullCalibrationDialog::reloadUI()
{
    this->changeInputRange(this->ui->inputRangeView->currentIndex());
}
void FullCalibrationDialog::changeInputRange(int v)
{
    for(int ch = 0; ch < MAX_NOF_CHANNELS; ch++)
    {
        this->digitalValues[ch]->setValue(this->calibrationTable.digitalOffset[ch][v]);
        this->analogValues[ch]->setValue(this->calibrationTable.analogOffset[ch][v]);
        this->labels[ch]->setText(
            QString::fromStdString(
                fmt::format("CH{} ({}|{})", ch+1, this->digitizer->getAnalogOffset(ch, v), this->digitizer->getDigitalOffset(ch,v))
            )
        );
    }
}

void FullCalibrationDialog::initialize(ApplicationContext *context)
{
    this->DigitizerGUIComponent::initialize(context);
    this->parameterComputer = std::unique_ptr<SignalParameterComputer>(
        new SignalParameterComputer(this->digitizer->getFileSizeLimit())
    );
    this->calibrationProcessorsList.push_back(this->parameterComputer.get());
    this->ui->startCalibration->connect(
        this->ui->startCalibration,
        &QAbstractButton::pressed,
        this,
        &FullCalibrationDialog::startCalibration
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
    for(int ch=0; ch < MAX_NOF_CHANNELS; ch++)
    {
        this->digitalValues[ch]->connect(
            this->digitalValues[ch],
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,
            [=](int val) {
                this->setDigitalValue(ch, this->ui->inputRangeView->currentIndex(), val);
            }
        );
        this->analogValues[ch]->connect(
            this->analogValues[ch],
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,
            [=](int val) {
                this->setAnalogValue(ch, this->ui->inputRangeView->currentIndex(), val);
            }
        );
    }

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
    this->calibrationTable.analogOffset[ch][inrange] = 0;
    this->calibrationTable.digitalOffset[ch][inrange] = 0;
}

void FullCalibrationDialog::setAnalogValue(int ch, int ir, int val)
{
    this->calibrationTable.analogOffset[ch][ir] = val;
}

void FullCalibrationDialog::setDigitalValue(int ch, int ir, int val)
{
    this->calibrationTable.digitalOffset[ch][ir] = val;
}
void FullCalibrationDialog::startCalibration()
{
    this->currentSetupIndex = 0;
    unsigned char channelMask =
        (this->ui->channelSelection0->checkState()?(1<<0):0) |
        (this->ui->channelSelection1->checkState()?(1<<1):0) |
        (this->ui->channelSelection2->checkState()?(1<<2):0) |
        (this->ui->channelSelection3->checkState()?(1<<3):0);
    if(!channelMask) return;
    spdlog::debug("Starting full calibration");
    if(this->ui->modeAnalog->isChecked()) {
        this->mode = CALIBRATION_MODES::ANALOG;
    } else if(this->ui->modeDigital->isChecked()) {
        this->mode = CALIBRATION_MODES::DIGITAL;
    } else if(this->ui->modeCombined->isChecked()) {
        this->mode = CALIBRATION_MODES::FINE_DIGITAL;
    }
    this->blockUI();

    connect(this->digitizer, &Digitizer::digitizerStateChanged, this, &FullCalibrationDialog::onStateChanged);
    this->setups.clear();

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
            fmt::format("CH{}@{}mV", this->setups[this->currentSetupIndex].channel+1, INPUT_RANGE_VALUES[this->setups[this->currentSetupIndex].inputRange])
        )
    );
    this->currentCalibrationAcquisition = this->acquisitionFromStage(this->setups[this->currentSetupIndex]);
    bool success = true;
    success &= this->digitizer->runOverridenAcquisition(this->currentCalibrationAcquisition, this->calibrationProcessorsList, this->calibrationTable);
    if(!success) {
        spdlog::debug("Calibration-acquisition failed to start.");
        this->ui->hintLabel->setText("Calibration failed.");
        this->digitizer->stopAcquisition();
        return false;
    }
    spdlog::debug("Running stage {}: m:{} ch:{} ir:{}", this->currentSetupIndex, this->setups[this->currentSetupIndex].mode, this->setups[this->currentSetupIndex].channel, this->setups[this->currentSetupIndex].inputRange);
    return true;
}

Acquisition FullCalibrationDialog::acquisitionFromStage(const FullCalibrationSetup &stage)
{
    int ch = stage.channel;
    int ir = stage.inputRange;
    Acquisition acq;
    acq.setChannelMask(1<<ch);
    acq.setTriggerMask(1<<ch);
    acq.setInputRange(ch, static_cast<INPUT_RANGES>(ir));
    acq.setAnalogOffset(ch, this->calibrationTable.analogOffset[ch][ir]);
    acq.setDigitalOffset(ch, this->calibrationTable.digitalOffset[ch][ir]);
    acq.setSampleSkip(this->ui->sampleSkip->value());
    acq.setTransferBufferCount(32);
    acq.setTransferBufferSize(2UL*1024UL*1024UL);
    acq.setTransferBufferQueueSize(64);
    acq.setDcBias(ch, 0);
    acq.setIsContinuous(true);
    acq.setRecordCount(Acquisition::INFINITE_RECORDS);
    acq.setTriggerMode(TRIGGER_MODES::SOFTWARE);
    acq.setUserLogicBypassMask(0b11);
    acq.setDuration(this->ui->calibrationDuration->value());
    acq.setFileSizeLimit(1000000000UL);
    return acq;
}
void FullCalibrationDialog::onStateChanged(Digitizer::DIGITIZER_STATE newState)
{
    if(!this->acquisitionActive)
    {
        spdlog::debug("Acquisition was canceled.");
        this->ui->hintLabel->setText("Calibration cancelled.");
        this->stopAcquisitions();
        return;
    }
    if(newState == Digitizer::DIGITIZER_STATE::READY)
    {
        spdlog::debug("Calibration stage finished. Computing average.");
        std::unique_ptr<SignalParameters> sp = this->parameterComputer->getResults();
        if(!sp)
        {
            spdlog::error("Signal anaylzer results were accessed before finishing acquisition.");
            return;
        }
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
            this->calibrationTable.analogOffset[this->setups[currentSetupIndex].channel][this->setups[currentSetupIndex].inputRange]
                = -sp->average;
        }
        else
        {
            this->calibrationTable.digitalOffset[this->setups[currentSetupIndex].channel][this->setups[currentSetupIndex].inputRange]
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
            this->ui->progressBar->setValue(100*this->currentSetupIndex/(int)this->setups.size());

            if(!this->runStage()) {
                this->stopAcquisitions();
            }

        }

    }
}
void FullCalibrationDialog::stopAcquisitions()
{
    this->acquisitionActive = false;
    this->digitizer->disconnect(this);
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
            this->digitizer->setAnalogOffset(ch, inputRange, this->calibrationTable.analogOffset[ch][inputRange]);
            this->digitizer->setDigitalOffset(ch, inputRange, this->calibrationTable.digitalOffset[ch][inputRange]);
            continue;
        }
        for(int inrange = 0; inrange < INPUT_RANGE_COUNT; inrange++)
        {
            this->digitizer->setAnalogOffset(ch, inrange, this->calibrationTable.analogOffset[ch][inrange]);
            this->digitizer->setDigitalOffset(ch, inrange, this->calibrationTable.digitalOffset[ch][inrange]);
        }
    }
    this->reloadUI();

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
    QFile file(fileName);
    if(!file.open(QFile::OpenModeFlag::ReadOnly))
    {
        spdlog::error("Failed to open file {}", fileName.toStdString());
        return;
    }
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();
    if(err.error != QJsonParseError::NoError)
    {
        spdlog::error("Failed to parse JSON in file {}", fileName.toStdString());
        return;
    }
    this->calibrationTable = CalibrationTable::fromJson(doc.object());
    this->changeInputRange(this->ui->inputRangeView->currentIndex());
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
    QFile file(fileName);
    if(!file.open(QFile::OpenModeFlag::WriteOnly))
    {
        spdlog::error("Failed to open file {}", fileName.toStdString());
        return;
    }
    QJsonDocument doc;
    doc.setObject(this->calibrationTable.toJson());
    file.write(doc.toJson());
    file.close();
}


void FullCalibrationDialog::enableVolatileSettings(bool enabled)
{
    this->setEnabled(enabled);
}
