#include "SpectrumDialog.h"
#include "ui_SpectrumDialog.h"
#include "DigitizerConstants.h"
const unsigned int RESET_REGISTER = 0x05;
const unsigned int FIRST_REGISTER = 0x08;
const unsigned int REGISTER_COUNT = (1<<10);
const unsigned int UL_TARGET = 1;
const unsigned int CHANNEL_SOURCE = 1;
SpectrumDialog::SpectrumDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpectrumDialog)
{
    ui->setupUi(this);
}

SpectrumDialog::~SpectrumDialog()
{
    delete ui;
}

void SpectrumDialog::reloadUI()
{

}

void SpectrumDialog::initialize(ApplicationContext *context)
{
    this->context = context;
    x.reserve(REGISTER_COUNT);
    y.reserve(REGISTER_COUNT);
    for(size_t i = 0; i < REGISTER_COUNT; i++)
    {
        x.append( i*(double)USHRT_MAX/REGISTER_COUNT/*-SHRT_MIN*/);
        y.append( 0 );
    }
    this->ui->plotArea->addGraph();
    this->ui->plotArea->setInteraction(QCP::iRangeDrag, true);
    this->ui->plotArea->setInteraction(QCP::iRangeZoom, true);
    this->connect(this->ui->downloadButton, &QAbstractButton::pressed, this, &SpectrumDialog::downloadSpectrum);
    this->connect(this->ui->loadButton, &QAbstractButton::pressed, this, &SpectrumDialog::loadSpectrum);
    this->connect(this->ui->saveButton, &QAbstractButton::pressed, this, &SpectrumDialog::saveSpectrum);
    this->connect(this->ui->resetButton, &QAbstractButton::pressed, this, &SpectrumDialog::resetSpectrum);
}

void SpectrumDialog::enableVolatileSettings(bool enabled)
{

}

void SpectrumDialog::downloadSpectrum()
{
    std::unique_ptr<uint32_t[]> data(new uint32_t[REGISTER_COUNT]());

    this->context->digitizer->readBlockUserRegister(UL_TARGET, FIRST_REGISTER, data.get(), REGISTER_COUNT*sizeof(uint32_t), READ_USER_REGISTER_LIKE_RAM);
    for(size_t i = 0; i < REGISTER_COUNT; i++)
    {
        y[i] = data[i];
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
    for(int i = 0; i < REGISTER_COUNT; i++)
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
    for(size_t i = 0; i < REGISTER_COUNT; i++)
    {
        file.write( fmt::format("{:f},{:f}\n", this->x.at(i), this->y.at(i)).c_str() );
    }
    file.close();
}

void SpectrumDialog::resetSpectrum()
{
    for(size_t i = 0; i < REGISTER_COUNT; i++)
    {
        y[i] = 0;
    }
    unsigned int retval;
    this->context->digitizer->writeUserRegister(UL_TARGET, RESET_REGISTER, 1, 1, &retval);
    this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
    this->ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}

