#include "SpectroscopeTab.h"
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
#include "ui_SpectroscopeTab.h"

using namespace Spectroscopy;
SpectroscopeTab::SpectroscopeTab(QWidget *parent)
    : QWidget(parent), ui(new Ui::SpectroscopeTab) {
  auto &spec = this->context.config()->acq().spectroscope;
  ui->setupUi(this);
  this->plotter = std::unique_ptr<SpectrumPlotter>(new SpectrumPlotter(1024));
  this->reallocatePlotSize(spec.binCount());
  this->ui->plotArea->addGraph();
  this->ui->plotArea->setInteraction(QCP::iRangeDrag, true);
  this->ui->plotArea->setInteraction(QCP::iRangeZoom, true);
  this->connect(this->ui->downloadButton, &QAbstractButton::pressed, this,
                &SpectroscopeTab::downloadSpectrum);
  this->connect(this->ui->loadButton, &QAbstractButton::pressed, this,
                &SpectroscopeTab::loadSpectrum);
  this->connect(this->ui->saveButton, &QAbstractButton::pressed, this,
                &SpectroscopeTab::saveSpectrum);
  this->connect(this->ui->resetButton, &QAbstractButton::pressed, this,
                &SpectroscopeTab::resetSpectrum);
  this->connect(this->ui->enableSpectrumDMA, &QCheckBox::stateChanged, this,
                &SpectroscopeTab::changeSpectrumDMAEnabled);
  this->connect(
      this->ui->channelPlotSource,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &SpectroscopeTab::changePlotChannel);
  this->connect(this->plotter.get(), &SpectrumPlotter::onScopeUpdate, this,
                &SpectroscopeTab::updateScope,
                Qt::ConnectionType::BlockingQueuedConnection);
  this->connect(this->ui->binCount, &QAbstractSlider::valueChanged, this,
                &SpectroscopeTab::changeSpectrumBinCount);
  this->connect(this->plotter.get(),
                &SpectrumPlotter::updateSpectrumCalculatedParams, this,
                &SpectroscopeTab::updateSpectrumCalculatedParams,
                Qt::ConnectionType::BlockingQueuedConnection);
  this->connect(this->ui->setWindowDuration, &QAbstractButton::released, this,
                &SpectroscopeTab::setSpectrumWindow);
  this->connect(this->ui->spectroscopeEnable, &QCheckBox::stateChanged, this,
                &SpectroscopeTab::setSpectroscopeEnabled);
  this->connect(this->context.digitizer(), &Digitizer::acquisitionStateChanged,
                this, &SpectroscopeTab::onAcquisitionStateChanged);
  // Disable spectroscope on application start.
  this->digitizer.spectroscope().disable();
}

SpectroscopeTab::~SpectroscopeTab() { delete ui; }

void SpectroscopeTab::reloadUI() {
  auto &spec = this->context.config()->acq().spectroscope;
  unsigned int binCount = spec.binCount();
  unsigned int reductionShift = spec.binCountReductionShift();
  unsigned int sliderPos = this->ui->binCount->maximum() - reductionShift;
  this->ui->binCount->setValue(sliderPos);
  this->ui->binCountLabel->setText(QString::number(binCount));
  this->ui->spectroscopeEnable->setChecked(spec.enabled());
}

void SpectroscopeTab::changeSpectrumDMAEnabled(int checked) {
  auto &acq = this->context.config()->acq();
  acq.spectroscope.setTransferOverDMA(checked);
}

void SpectroscopeTab::debugSpectrum() {}
void SpectroscopeTab::downloadSpectrum() {
  auto &acq = this->context.config()->acq();
  unsigned int binCount = acq.spectroscope.binCount();
  auto data = this->digitizer.spectroscope().downloadSpectrum(binCount);
  for (unsigned int i = 0; i < binCount; i++) {
    // if(i%10==1) spdlog::debug("{} = {}",i, data[i]);
    this->y[i] = data[i];
  }
  this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
  this->ui->plotArea->rescaleAxes();
  this->ui->plotArea->replot();
}

void SpectroscopeTab::loadSpectrum() {
  unsigned int binCount = this->context.config()->acq().spectroscope.binCount();
  QString fileName = QFileDialog::getOpenFileName(
      this, "Load spectrum", "", "CSV spectrum (*.csv);;All Files (*)");
  if (!fileName.length())
    return;
  QFile file(fileName);
  if (!file.exists()) {
    spdlog::error("File {} does not exist.", fileName.toStdString());
    return;
  }
  file.open(QFile::OpenModeFlag::ReadOnly);
  for (unsigned int i = 0; i < binCount; i++) {
    QByteArray line = file.readLine();
    QList<QByteArray> vals = line.split(',');
    double newY = vals[1].replace("\n", "").toDouble();
    this->x[i] = vals[0].toDouble();
    this->y[i] = newY;
  }
  file.close();
  this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
  this->ui->plotArea->rescaleAxes();
  this->ui->plotArea->replot();
}

void SpectroscopeTab::saveSpectrum() {
  unsigned int binCount = this->context.config()->acq().spectroscope.binCount();
  QString fileName = QFileDialog::getSaveFileName(
      this, "Save spectrum", "", "CSV spectrum (*.csv);;All Files (*)");
  if (!fileName.length())
    return;
  QFile file(fileName);
  file.open(QFile::OpenModeFlag::WriteOnly);
  for (unsigned int i = 0; i < binCount; i++) {
    file.write(
        fmt::format("{:f},{:f}\n", this->x.at(i), this->y.at(i)).c_str());
  }
  file.close();
}

void SpectroscopeTab::resetSpectrum() {
  unsigned int binCount = this->context.config()->acq().spectroscope.binCount();
  this->digitizer.spectroscope().resetSpectrum();
  for (unsigned int i = 0; i < binCount; i++) {
    y[i] = 0;
  }
  this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
  this->ui->plotArea->rescaleAxes();
  this->ui->plotArea->replot();
}

void SpectroscopeTab::changePlotChannel(int ch) {
  this->digitizer.removeRecordProcessor(this->plotter.get());
  this->plotter->changeChannel(ch - 1);
  if (ch) {
    this->digitizer.appendRecordProcessor(this->plotter.get());
  }
}

void SpectroscopeTab::updateScope(QVector<double> &x, QVector<double> y) {
  this->ui->plotArea->graph(0)->setData(x, y, true);
  this->ui->plotArea->rescaleAxes();
  this->ui->plotArea->replot();
}
void SpectroscopeTab::reallocatePlotSize(int binCount) {
  this->x.clear();
  this->y.clear();
  this->x.reserve(binCount);
  this->y.reserve(binCount);
  for (size_t i = 0; i < binCount; i++) {
    this->x.append(i /**(double)USHRT_MAX/REGISTER_COUNT-SHRT_MIN*/);
    this->y.append(0);
  }
}

void SpectroscopeTab::changeSpectrumBinCount(int sliderPos) {
  auto &acq = this->context.config()->acq();
  unsigned int reductionShift = this->ui->binCount->maximum() - sliderPos;
  acq.spectroscope.setBinCountReductionShift(reductionShift);
  unsigned int spectrumBinCount = acq.spectroscope.binCount();
  this->plotter->reallocate(spectrumBinCount);
  this->ui->binCountLabel->setText(QString::number(spectrumBinCount));
  this->reallocatePlotSize(spectrumBinCount);
}

void SpectroscopeTab::setSpectrumWindow() {
  auto &acq = this->context.config()->acq();
  unsigned int toSet = static_cast<uint32_t>(this->ui->windowDuration->value());
  acq.spectroscope.setWindowLength(toSet);
}

void SpectroscopeTab::setSpectroscopeEnabled(int checked) {
  auto &acq = this->context.config()->acq();
  acq.spectroscope.setEnabled(checked);
}

void SpectroscopeTab::updateSpectrumCalculatedParams(
    unsigned long long totalCount) {
  this->ui->totalCount->setText(
      QString::fromStdString(fmt::format("{}", totalCount)));
  this->ui->pulseRate->setText(QString::fromStdString(
      fmt::format("{:.2f}", (double)totalCount / this->windowDuration * 1000)));
}
void SpectroscopeTab::onAcquisitionStateChanged(AcquisitionStates os,
                                                AcquisitionStates ns) {
  switch (ns) {
  case AcquisitionStates::INACTIVE:
    this->enableAcquisitionSettings(true);
    break;
  case AcquisitionStates::STARTING:
    this->enableAcquisitionSettings(false);
    break;
  default:
    break;
  }
}

void SpectroscopeTab::enableAcquisitionSettings(bool en)
{
    this->ui->spectroscopeEnable->setEnabled(en);
    this->ui->enableSpectrumDMA->setEnabled(en);
    this->ui->binCount->setEnabled(en);
    this->ui->boxcarTrigger->setEnabled(en);
    this->ui->setBoxcarTrigger->setEnabled(en);
    this->ui->setWindowDuration->setEnabled(en);
    this->ui->windowDuration->setEnabled(en);
    this->ui->useZCDTrigger->setEnabled(en);
    this->ui->resetButton->setEnabled(en);
    this->ui->downloadButton->setEnabled(en);
}