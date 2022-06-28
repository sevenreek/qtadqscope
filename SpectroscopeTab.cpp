#include "SpectroscopeTab.h"
#include "DigitizerGUIComponent.h"
#include "ui_SpectroscopeTab.h"
#include "DigitizerConstants.h"
using namespace Spectroscopy;
SpectroscopeTab::SpectroscopeTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpectroscopeTab)
{
    ui->setupUi(this);
    this->plotter = std::unique_ptr<SpectrumPlotter>(new SpectrumPlotter(1024));
}

SpectroscopeTab::~SpectroscopeTab()
{
    delete ui;
}

void SpectroscopeTab::reloadUI()
{
    auto &spec = this->digitizer->cfg().acq().spectroscope;
    unsigned int binCount = spec.binCount();
    unsigned int reductionShift = spec.binCountReductionShift();
    unsigned int sliderPos = this->ui->binCount->maximum() - reductionShift;
    this->ui->binCount->setValue(sliderPos);
    this->ui->binCountLabel->setText(QString::number(binCount));
    this->ui->spectroscopeEnable->setChecked(spec.enabled());
}

void SpectroscopeTab::initialize(ApplicationContext *context)
{
    DigitizerGUIComponent::initialize(context);
    auto &spec = this->digitizer->cfg().acq().spectroscope;
    this->reallocatePlotSize(spec.binCount());
    this->ui->plotArea->addGraph();
    this->ui->plotArea->setInteraction(QCP::iRangeDrag, true);
    this->ui->plotArea->setInteraction(QCP::iRangeZoom, true);
    this->connect(this->ui->downloadButton, &QAbstractButton::pressed, this, &SpectroscopeTab::downloadSpectrum);
    this->connect(this->ui->loadButton, &QAbstractButton::pressed, this, &SpectroscopeTab::loadSpectrum);
    this->connect(this->ui->saveButton, &QAbstractButton::pressed, this, &SpectroscopeTab::saveSpectrum);
    this->connect(this->ui->resetButton, &QAbstractButton::pressed, this, &SpectroscopeTab::resetSpectrum);
    this->connect(this->ui->enableSpectrumDMA, &QCheckBox::stateChanged, this, &SpectroscopeTab::changeSpectrumDMAEnabled);
    this->connect(this->ui->channelPlotSource, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SpectroscopeTab::changePlotChannel);
    this->connect(this->plotter.get(), &SpectrumPlotter::onScopeUpdate, this, &SpectroscopeTab::updateScope, Qt::ConnectionType::BlockingQueuedConnection);
    this->connect(this->ui->binCount, &QAbstractSlider::valueChanged, this, &SpectroscopeTab::changeSpectrumBinCount);
    this->connect(this->plotter.get(), &SpectrumPlotter::updateSpectrumCalculatedParams, this, &SpectroscopeTab::updateSpectrumCalculatedParams, Qt::ConnectionType::BlockingQueuedConnection);
    this->connect(this->ui->setWindowDuration, &QAbstractButton::released, this, &SpectroscopeTab::setSpectrumWindow);
    this->connect(this->ui->spectroscopeEnable, &QCheckBox::stateChanged, this, &SpectroscopeTab::setSpectroscopeEnabled);
    // Disable spectroscope on application start.
    this->context->digitizer->spectroscope().disable();
}

void SpectroscopeTab::changeSpectrumDMAEnabled(int checked)
{
    this->digitizer->cfg().acq().spectroscope.setTransferOverDMA(checked);
}

void SpectroscopeTab::debugSpectrum()
{

}
void SpectroscopeTab::downloadSpectrum()
{
    unsigned int binCount = this->digitizer->cfg().acq().spectroscope.binCount();
    auto data = this->digitizer->spectroscope().downloadSpectrum(binCount);
    for(unsigned int i = 0; i < binCount; i++)
    {
        //if(i%10==1) spdlog::debug("{} = {}",i, data[i]);
        this->y[i] = data[i];
    }
    this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
    this->ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}

void SpectroscopeTab::loadSpectrum()
{
    unsigned int binCount = this->digitizer->cfg().acq().spectroscope.binCount();
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
    for(unsigned int i = 0; i < binCount; i++)
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

void SpectroscopeTab::saveSpectrum()
{
    unsigned int spectrumBinCount = this->digitizer->cfg().acq().spectroscope.binCount();
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save spectrum",
        "",
        "CSV spectrum (*.csv);;All Files (*)"
    );
    if(!fileName.length()) return;
    QFile file(fileName);
    file.open(QFile::OpenModeFlag::WriteOnly);
    for(unsigned int i = 0; i < spectrumBinCount; i++)
    {
        file.write( fmt::format("{:f},{:f}\n", this->x.at(i), this->y.at(i)).c_str() );
    }
    file.close();
}

void SpectroscopeTab::resetSpectrum()
{
    unsigned int spectrumBinCount = this->digitizer->cfg().acq().spectroscope.binCount();
    this->digitizer->spectroscope().resetSpectrum();
    for(unsigned int i = 0; i < spectrumBinCount; i++)
    {
        y[i] = 0;
    }
    this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
    this->ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}

void SpectroscopeTab::changePlotChannel(int ch)
{
    this->context->digitizer->removeRecordProcessor(this->plotter.get());
    this->plotter->changeChannel(ch-1);
    if(ch)
    {
        this->context->digitizer->appendRecordProcessor(this->plotter.get());
    }
}


void SpectroscopeTab::updateScope(QVector<double> &x, QVector<double> y)
{
    this->ui->plotArea->graph(0)->setData(x,y, true);
    this->ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}
void SpectroscopeTab::reallocatePlotSize(int binCount)
{
    unsigned int spectrumBinCount = this->digitizer->cfg().acq().spectroscope.binCount();
    this->x.clear();
    this->y.clear();
    this->x.reserve(spectrumBinCount);
    this->y.reserve(spectrumBinCount);
    for(size_t i = 0; i < spectrumBinCount; i++)
    {
        this->x.append( i/**(double)USHRT_MAX/REGISTER_COUNT-SHRT_MIN*/);
        this->y.append( 0 );
    }
}

void SpectroscopeTab::changeSpectrumBinCount(int sliderPos)
{
    unsigned int reductionShift = this->ui->binCount->maximum() - sliderPos;
    this->digitizer->cfg().acq().spectroscope.setBinCountReductionShift(reductionShift);
    unsigned int spectrumBinCount = this->digitizer->cfg().acq().spectroscope.binCount();
    this->plotter->reallocate(spectrumBinCount);
    this->ui->binCountLabel->setText(QString::number(spectrumBinCount));
    this->reallocatePlotSize(spectrumBinCount);

}

void SpectroscopeTab::setSpectrumWindow()
{
    unsigned int toSet = static_cast<uint32_t>(this->ui->windowDuration->value());
    this->digitizer->cfg().acq().spectroscope.setWindowLength(toSet);
}

void SpectroscopeTab::setSpectroscopeEnabled(int checked)
{
    this->digitizer->cfg().acq().spectroscope.setEnabled(checked);
}

void SpectroscopeTab::updateSpectrumCalculatedParams(unsigned long long totalCount)
{
    this->ui->totalCount->setText(QString::fromStdString(fmt::format("{}",totalCount)));
    this->ui->pulseRate->setText(QString::fromStdString(fmt::format("{:.2f}",(double)totalCount / this->windowDuration * 1000)));
}
