#include "AutoCalibrateDialog.h"
#include "ui_AutoCalibrateDialog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/spdlog.h"
AutoCalibrateDialog::AutoCalibrateDialog(std::shared_ptr<ApplicationConfiguration> appConfig,
                                         std::shared_ptr<Acquisition> acq, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoCalibrateDialog)
{
    this->appConfig = appConfig;
    this->acquisition = acq;
    this->calibrationConfiguration = std::make_shared<ApplicationConfiguration>(*(appConfig.get()));
    //this->calibrationConfiguration
    ui->setupUi(this);
    this->parameterComputer = std::make_shared<SignalParameterComputer>(this->calibrationConfiguration->fileSizeLimit);

    // AUTOCALIBRATE
    this->ui->autoCalibrateButton->connect(
        this->ui->autoCalibrateButton,
        &QAbstractButton::pressed,
        this,
        &AutoCalibrateDialog::start
    );
    this->ui->autoCalibrateApplyOffset->connect(
        this->ui->autoCalibrateApplyOffset,
        &QAbstractButton::pressed,
        this,
        &AutoCalibrateDialog::apply
    );

}
void AutoCalibrateDialog::showDialog()
{
    this->calibrationConfiguration = std::make_shared<ApplicationConfiguration>(*(this->appConfig.get()));
    this->configure();
    this->ui->autoCalibrateChannel->setText(QString::fromStdString(fmt::format("CH{}", this->calibrationConfiguration->getCurrentChannel()+1)));
    this->ui->autoCalibrateSampleSkip->setText(QString::fromStdString(fmt::format("{}", this->calibrationConfiguration->getCurrentChannelConfig().sampleSkip)));
    this->ui->autoCalibrationStatus->setText("READY");
    this->show();
}

AutoCalibrateDialog::~AutoCalibrateDialog()
{
    delete ui;
}
void AutoCalibrateDialog::start()
{
    this->calibrationConfiguration->getCurrentChannelConfig().log();
    this->parameterComputer->startNewStream(*(this->calibrationConfiguration.get()));

    std::list<std::shared_ptr<RecordProcessor>> rps;
    rps.push_back(this->parameterComputer);
    unsigned long durationMs = this->ui->autoCalibrateDuration->value();
    CALIBRATION_MODES mode;
    if(this->ui->autoCalibrateAnalog->isChecked()) {
        mode = CALIBRATION_MODES::ANALOG;
    } else if(this->ui->autoCalibrateDigital->isChecked()) {
        mode = CALIBRATION_MODES::DIGITAL;
    } else if(this->ui->autoCalibrateFine->isChecked()) {
        mode = CALIBRATION_MODES::FINE_DIGITAL;
    }
    bool success = true;
    success &= this->acquisition->configure(this->calibrationConfiguration, rps);
    if(!success) {
        this->ui->autoCalibrationStatus->setText("FAILED TO CONFIGURE");
        return;
    }
    if(durationMs > 0)
    {
        success &= this->acquisition->startTimed(durationMs, true);
    }
    else {
        this->ui->autoCalibrationStatus->setText("FAILED TO START");
        return;
    }
    if(success) {
        connect(this->acquisition.get(), &Acquisition::onStateChanged, this, &AutoCalibrateDialog::onStateChanged);
        this->ui->autoCalibrationStatus->setText("RUNNING");
        this->ui->autoCalibrateProgress->setValue(0);
        this->ui->autoCalibrateButton->setEnabled(false);
    }
    else
    {
        this->ui->autoCalibrationStatus->setText("FAILED TO START");
        this->acquisition->stop();
    }
}
void AutoCalibrateDialog::onStateChanged(ACQUISITION_STATES newState)
{
    if(newState == ACQUISITION_STATES::STOPPED)
    {
        this->ui->autoCalibrationStatus->setText("CALCULATING...");
        std::unique_ptr<SignalParameters> sp = this->parameterComputer->getResults();
        this->ui->autoCalibrationStatus->setText("FINISHED");
        this->ui->autoCalibrateButton->setEnabled(true);
        CALIBRATION_MODES mode;
        if(this->ui->autoCalibrateAnalog->isChecked()) {
            mode = CALIBRATION_MODES::ANALOG;
        } else if(this->ui->autoCalibrateDigital->isChecked()) {
            mode = CALIBRATION_MODES::DIGITAL;
        } else if(this->ui->autoCalibrateFine->isChecked()) {
            mode = CALIBRATION_MODES::FINE_DIGITAL;
        }
        spdlog::debug("Obtained average {} rms {}.", sp->average, sp->rms);
        this->computedAverage = sp->average;
        this->ui->autoCalibrateApplyOffset->setEnabled(true);
        this->ui->autoCalibrateAverage->setText(QString::fromStdString(
            fmt::format("{} ({:.2f} mV)", (int)sp->average, ADCCodeToMV(this->appConfig->getCurrentChannelConfig().inputRangeFloat, sp->average))));
        this->ui->autoCalibrateRMS->setText(QString::fromStdString(
            fmt::format("{} ({:.2f} mV)", (int)sp->rms, ADCCodeToMV(this->appConfig->getCurrentChannelConfig().inputRangeFloat, sp->rms))));
        this->acquisition->disconnect(this);
    }

}
void AutoCalibrateDialog::configure()
{
    CALIBRATION_MODES mode;
    if(this->ui->autoCalibrateAnalog->isChecked()) {
        mode = CALIBRATION_MODES::ANALOG;
        this->calibrationConfiguration->getCurrentChannelConfig().baseDcBiasOffset = 0;
    } else if(this->ui->autoCalibrateDigital->isChecked()) {
        mode = CALIBRATION_MODES::DIGITAL;
        this->calibrationConfiguration->getCurrentChannelConfig().baseDcBiasOffset = 0;
    } else if(this->ui->autoCalibrateFine->isChecked()) {
        mode = CALIBRATION_MODES::FINE_DIGITAL;
    }
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
}
void AutoCalibrateDialog::apply()
{
    CALIBRATION_MODES mode;
    if(this->ui->autoCalibrateAnalog->isChecked()) {
        mode = CALIBRATION_MODES::ANALOG;
    } else if(this->ui->autoCalibrateDigital->isChecked()) {
        mode = CALIBRATION_MODES::DIGITAL;
    } else if(this->ui->autoCalibrateFine->isChecked()) {
        mode = CALIBRATION_MODES::FINE_DIGITAL;
    }
    emit this->offsetCalculated(mode, -this->computedAverage);
}
