#include "SpectrumDialog.h"
#include "ui_SpectrumDialog.h"
#include "DigitizerConstants.h"
#include "RegisterConstants.h"
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
        x.append( i/**(double)USHRT_MAX/REGISTER_COUNT-SHRT_MIN*/);
        y.append( 0 );
    }
    this->ui->plotArea->addGraph();
    this->ui->plotArea->setInteraction(QCP::iRangeDrag, true);
    this->ui->plotArea->setInteraction(QCP::iRangeZoom, true);
    this->connect(this->ui->downloadButton, &QAbstractButton::pressed, this, &SpectrumDialog::downloadSpectrum);
    this->connect(this->ui->loadButton, &QAbstractButton::pressed, this, &SpectrumDialog::loadSpectrum);
    this->connect(this->ui->saveButton, &QAbstractButton::pressed, this, &SpectrumDialog::saveSpectrum);
    this->connect(this->ui->resetButton, &QAbstractButton::pressed, this, &SpectrumDialog::resetSpectrum);
    this->connect(this->ui->setBoxcarTrigger, &QAbstractButton::pressed, this, &SpectrumDialog::setTriggerLevel);
    this->connect(this->ui->debugButton, &QAbstractButton::pressed, this, &SpectrumDialog::debugSpectrum);
    unsigned int retval;
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~ACTIVE_SPECTRUM_BIT, 0, &retval); // disable pha
}

void SpectrumDialog::enableVolatileSettings(bool enabled)
{

}

void SpectrumDialog::debugSpectrum()
{
    unsigned int retval;
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~DEBUG_SPECTRUM_BIT, DEBUG_SPECTRUM_BIT, &retval); // debug pha
    spdlog::debug("write debug 0x06 {:X}",retval);
    this->context->digitizer->readUserRegister(UL_TARGET, 0x07, &retval);
    spdlog::debug("write debug 0x07 {:X}",retval);
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~DEBUG_SPECTRUM_BIT, 0, &retval); // debug pha
    spdlog::debug("reset debug 0x06 {:X}",retval);
    std::unique_ptr<uint32_t[]> data(new uint32_t[REGISTER_COUNT]());
}

void SpectrumDialog::downloadSpectrum()
{
    std::unique_ptr<uint32_t[]> data(new uint32_t[REGISTER_COUNT]());

    this->context->digitizer->readBlockUserRegister(UL_TARGET, FIRST_REGISTER, data.get(), REGISTER_COUNT*sizeof(uint32_t), READ_USER_REGISTER_LIKE_RAM);
    for(size_t i = 0; i < REGISTER_COUNT; i++)
    {
        //if(i%10==1) spdlog::debug("{} = {}",i, data[i]);
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
    this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~RESET_SPECTRUM_BIT, RESET_SPECTRUM_BIT, &retval);
    spdlog::debug("reset set 0x06 {:X}",retval);
    int timeToReset = ceil(4.0*REGISTER_COUNT/1000000); // 4 ns clock period
    QTimer::singleShot(timeToReset, this, [=] {
        unsigned int retval;
        this->context->digitizer->writeUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~RESET_SPECTRUM_BIT, 0, &retval);
    });
    spdlog::debug("reset reset 0x06 {:X}",retval);
    this->ui->plotArea->graph(0)->setData(this->x, this->y, true);
    this->ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
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
