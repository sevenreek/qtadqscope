#include "SpectrumDialog.h"
#include "ui_SpectrumDialog.h"
#include "DigitizerConstants.h"
SpectrumDialog::SpectrumDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpectrumDialog)
{
    ui->setupUi(this);
    this->plotter = std::unique_ptr<SpectrumPlotter>(new SpectrumPlotter(1024));
}

SpectrumDialog::~SpectrumDialog()
{
    delete ui;
}

void SpectrumDialog::reloadUI()
{
    this->loadConfigFromDevice();
    unsigned int reductionShift = std::round(std::log2(MAX_SPECTRUM_BIN_COUNT/this->spectrumBinCount));
    unsigned int sliderPos = this->ui->binCount->maximum() - reductionShift;
    this->ui->binCount->setValue(sliderPos);
    this->ui->binCountLabel->setText(QString::number(this->spectrumBinCount));
    this->ui->spectroscopeEnable->setChecked(this->context->digitizer->getSpectroscopeEnabled());
}

void SpectrumDialog::initialize(ApplicationContext *context)
{
    this->context = context;
    this->reallocatePlotSize(this->spectrumBinCount);
    this->ui->plotArea->addGraph();
    this->ui->plotArea->setInteraction(QCP::iRangeDrag, true);
    this->ui->plotArea->setInteraction(QCP::iRangeZoom, true);
    this->connect(this->ui->downloadButton, &QAbstractButton::pressed, this, &SpectrumDialog::downloadSpectrum);
    this->connect(this->ui->loadButton, &QAbstractButton::pressed, this, &SpectrumDialog::loadSpectrum);
    this->connect(this->ui->saveButton, &QAbstractButton::pressed, this, &SpectrumDialog::saveSpectrum);
    this->connect(this->ui->resetButton, &QAbstractButton::pressed, this, &SpectrumDialog::resetSpectrum);
    this->connect(this->ui->setBoxcarTrigger, &QAbstractButton::pressed, this, &SpectrumDialog::setTriggerLevel);
    this->connect(this->ui->enableSpectrumDMA, &QCheckBox::stateChanged, this, &SpectrumDialog::changeSpectrumDMAEnabled);
    this->connect(this->ui->useZCDTrigger, &QCheckBox::stateChanged, this, &SpectrumDialog::changeUseZCDTrigger);
    this->connect(this->ui->channelPlotSource, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SpectrumDialog::changePlotChannel);
    this->connect(this->plotter.get(), &SpectrumPlotter::onScopeUpdate, this, &SpectrumDialog::updateScope, Qt::ConnectionType::BlockingQueuedConnection);
    this->connect(this->ui->binCount, &QAbstractSlider::valueChanged, this, &SpectrumDialog::changeSpectrumBinCount);
    this->connect(this->plotter.get(), &SpectrumPlotter::updateSpectrumCalculatedParams, this, &SpectrumDialog::updateSpectrumCalculatedParams, Qt::ConnectionType::BlockingQueuedConnection);
    this->connect(this->ui->setWindowDuration, &QAbstractButton::released, this, &SpectrumDialog::setSpectrumWindow);
    this->connect(this->ui->spectroscopeEnable, &QCheckBox::stateChanged, this, &SpectrumDialog::setSpectroscopeEnabled);
    unsigned int retval;
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~ACTIVE_SPECTRUM_BIT, 0, &retval); // disable pha
}

void SpectrumDialog::changeSpectrumDMAEnabled(int checked)
{
    unsigned int retval;
    unsigned int value;
    if(checked) value = TRANSFER_SPECTRA_BIT;
    else value = 0;
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~TRANSFER_SPECTRA_BIT, value, &retval); // debug pha
    if((retval & TRANSFER_SPECTRA_BIT) != value) {
        spdlog::error("Failed to configure DMA spectrum transmission. {} != {}", retval, value);
    }

}

void SpectrumDialog::enableVolatileSettings(bool enabled)
{

}
void SpectrumDialog::changeUseZCDTrigger(int checked)
{
    checked = static_cast<bool>(checked);
    unsigned int retval;
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~USE_ZCD_BIT, checked?USE_ZCD_BIT:0, &retval);

}
void SpectrumDialog::debugSpectrum()
{

}
void SpectrumDialog::downloadSpectrum()
{
    std::unique_ptr<uint32_t[]> data(new uint32_t[this->spectrumBinCount]());

    this->context->digitizer->readBlockUserRegister(UL_TARGET, FIRST_REGISTER, data.get(), this->spectrumBinCount*sizeof(uint32_t), READ_USER_REGISTER_LIKE_RAM);
    for(unsigned int i = 0; i < this->spectrumBinCount; i++)
    {
        //if(i%10==1) spdlog::debug("{} = {}",i, data[i]);
        this->y[i] = data[i];
    }
    this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
    this->ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}

void SpectrumDialog::loadSpectrum()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Load spectrum",
        "",
        "CSV spectrum (*.csv);;All Files (*)"
    );
    if(!fileName.length()) return;
    QFile file(fileName);
    if(!file.exists())
    {
        spdlog::error("File {} does not exist.", fileName.toStdString());
        return;
    }
    file.open(QFile::OpenModeFlag::ReadOnly);
    for(unsigned int i = 0; i < this->spectrumBinCount; i++)
    {
        QByteArray line = file.readLine();
        QList<QByteArray> vals = line.split(',');
        double newY = vals[1].replace("\n","").toDouble();
        this->x[i] = vals[0].toDouble();
        this->y[i] = newY;
    }
    file.close();
    this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
    this->ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}

void SpectrumDialog::saveSpectrum()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save spectrum",
        "",
        "CSV spectrum (*.csv);;All Files (*)"
    );
    if(!fileName.length()) return;
    QFile file(fileName);
    file.open(QFile::OpenModeFlag::WriteOnly);
    for(unsigned int i = 0; i < this->spectrumBinCount; i++)
    {
        file.write( fmt::format("{:f},{:f}\n", this->x.at(i), this->y.at(i)).c_str() );
    }
    file.close();
}

void SpectrumDialog::resetSpectrum()
{
    for(unsigned int i = 0; i < this->spectrumBinCount; i++)
    {
        y[i] = 0;
    }
    unsigned int retval;
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~RESET_SPECTRUM_BIT, RESET_SPECTRUM_BIT, &retval);
    int timeToReset = ceil(4.0*this->spectrumBinCount/1000000); // 4 ns clock period
    QTimer::singleShot(timeToReset, this, [=] {
        unsigned int retval;
        this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~RESET_SPECTRUM_BIT, 0, &retval);
    });
    this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
    this->ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}

void SpectrumDialog::loadConfigFromDevice()
{
    uint32_t retval;
    this->context->digitizer->readUserRegister(UL_TARGET, TRIGGER_REGISTER, &retval);
    this->ui->boxcarTrigger->setValue(static_cast<int32_t>(retval));

    this->context->digitizer->readUserRegister(UL_TARGET, PHA_WINDOW_LENGTH_AND_BINCOUNT_REGISTER, &retval);
    unsigned int windowLength = retval & WINDOWLENGTH_MASK;
    this->ui->windowDuration->setValue(static_cast<uint32_t>(windowLength));
}

void SpectrumDialog::setTriggerLevel()
{
    int trigger = this->ui->boxcarTrigger->value();
    unsigned int retval;
    this->context->digitizer->writeUserRegister(UL_TARGET, TRIGGER_REGISTER, 0, trigger, &retval);
    if(int(retval)!=trigger) {
        spdlog::warn("Trigger not set properly. Should be {}. Returned {}.", trigger, int(retval));
    }
}

void SpectrumDialog::changePlotChannel(int ch)
{
    this->context->digitizer->removeRecordProcessor(this->plotter.get());
    this->plotter->changeChannel(ch-1);
    if(ch)
    {
        this->context->digitizer->appendRecordProcessor(this->plotter.get());
    }
}


void SpectrumDialog::updateScope(QVector<double> &x, QVector<double> y)
{
    this->ui->plotArea->graph(0)->setData(x,y, true);
    this->ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}
void SpectrumDialog::reallocatePlotSize(int binCount)
{
    this->x.clear();
    this->y.clear();
    this->x.reserve(this->spectrumBinCount);
    this->y.reserve(this->spectrumBinCount);
    for(size_t i = 0; i < this->spectrumBinCount; i++)
    {
        this->x.append( i/**(double)USHRT_MAX/REGISTER_COUNT-SHRT_MIN*/);
        this->y.append( 0 );
    }
}

void SpectrumDialog::changeSpectrumBinCount(int sliderPos)
{
    unsigned int reductionShift = this->ui->binCount->maximum() - sliderPos;
    int binCount = MAX_SPECTRUM_BIN_COUNT >> reductionShift;
    this->spectrumBinCount = binCount;
    this->plotter->reallocate(binCount);
    unsigned int shiftedReductionShift = (reductionShift << BINCOUNT_SHIFT);
    this->ui->binCountLabel->setText(QString::number(binCount));
    unsigned int retval;
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_WINDOW_LENGTH_AND_BINCOUNT_REGISTER, ~BINCOUNT_MASK, shiftedReductionShift, &retval);
    this->context->digitizer->readUserRegister(UL_TARGET, PHA_WINDOW_LENGTH_AND_BINCOUNT_REGISTER, &retval);
    this->reallocatePlotSize(this->spectrumBinCount);

}

void SpectrumDialog::setSpectrumWindow()
{
    unsigned int toSet = static_cast<uint32_t>(this->ui->windowDuration->value());
    unsigned int returnValue;
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_WINDOW_LENGTH_AND_BINCOUNT_REGISTER, ~WINDOWLENGTH_MASK, toSet, &returnValue);
    this->windowDuration = toSet;
}

void SpectrumDialog::setSpectroscopeEnabled(int checked)
{
    this->context->digitizer->setSpectroscopeEnabled(bool(checked));
}

void SpectrumDialog::updateSpectrumCalculatedParams(unsigned long long totalCount)
{
    this->ui->totalCount->setText(QString::fromStdString(fmt::format("{}",totalCount)));
    this->ui->pulseRate->setText(QString::fromStdString(fmt::format("{:.2f}",(double)totalCount / this->windowDuration * 1000)));
}
