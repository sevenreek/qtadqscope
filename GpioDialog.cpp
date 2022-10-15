#include "GpioDialog.h"
#include "ApplicationContext.h"
#include "DigitizerGUIComponent.h"
#include "qabstractbutton.h"
#include "qnamespace.h"
#include "ui_GpioDialog.h"


GPIODialog::GPIODialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GPIODialog)
{
    ui->setupUi(this);
    this->createGPIOGrid();
}

GPIODialog::~GPIODialog()
{
    delete ui;
}

void GPIODialog::createGPIOGrid()
{
    headerLeftOutput = new QLabel(this);
    headerLeftOutput->setText("Output");
    ui->gpioContainer->addWidget(headerLeftOutput, 0, 0, 1, 2, Qt::AlignHCenter);
    headerLeftNames = new QLabel(this);
    headerLeftNames->setText("Port");
    ui->gpioContainer->addWidget(headerLeftNames, 0, 2, 1, 1, Qt::AlignRight);

    headerInput = new QLabel(this);
    headerInput->setText("Input");
    ui->gpioContainer->addWidget(headerInput, 0, 3, 1, 2, Qt::AlignHCenter);

    headerRightNames = new QLabel(this);
    headerRightNames->setText("Port");
    ui->gpioContainer->addWidget(headerRightNames, 0, 5, 1, 1);
    headerRightOutput = new QLabel(this);
    headerRightOutput->setText("Output");
    ui->gpioContainer->addWidget(headerRightOutput, 0, 6, 1, 2, Qt::AlignHCenter);
    for(int gpio=0; gpio < GPIODialog::GPIO_COUNT; gpio+=2)
    {
        int row = gpio/2+1;
        portOutputValues[gpio] = new QCheckBox(this);
        portOutputValues[gpio]->setText("High");
        ui->gpioContainer->addWidget(portOutputValues[gpio], row, 0);
        portOutputEnables[gpio] = new QCheckBox(this);
        portOutputEnables[gpio]->setText("Enable");
        ui->gpioContainer->addWidget(portOutputEnables[gpio], row, 1);
        portLabels[gpio] = new QLabel(this);
        portLabels[gpio]->setText(QString::fromStdString(fmt::format("GPIO{}", gpio)));
        ui->gpioContainer->addWidget(portLabels[gpio], row, 2, Qt::AlignRight);
        portInputValues[gpio] = new QLabel(this);
        portInputValues[gpio]->setText("#");
        ui->gpioContainer->addWidget(portInputValues[gpio], row, 3, Qt::AlignRight);

        portInputValues[gpio+1] = new QLabel(this);
        portInputValues[gpio+1]->setText("#");
        ui->gpioContainer->addWidget(portInputValues[gpio+1], row, 4);
        portLabels[gpio+1] = new QLabel(this);
        portLabels[gpio+1 ]->setText(QString::fromStdString(fmt::format("GPIO{}", gpio+1)));
        ui->gpioContainer->addWidget(portLabels[gpio+1], row, 5);
        portOutputEnables[gpio+1] = new QCheckBox(this);
        portOutputEnables[gpio+1]->setText("Enable");
        ui->gpioContainer->addWidget(portOutputEnables[gpio+1], row, 6);
        portOutputValues[gpio+1] = new QCheckBox(this);
        portOutputValues[gpio+1]->setText("High");
        ui->gpioContainer->addWidget(portOutputValues[gpio+1], row, 7);

    }
}

void GPIODialog::reloadUI()
{

}

void GPIODialog::initialize(ApplicationContext *context)
{
    DigitizerGUIComponent::initialize(context);
    connect(this->ui->readGPIO, &QAbstractButton::clicked, this, &GPIODialog::readGPIO);
    connect(this->ui->supplyOutputEnable, &QCheckBox::stateChanged, this, &GPIODialog::enableSupplyOutput);
    for(int gpio=0; gpio < GPIODialog::GPIO_COUNT; gpio+=2)
    {
        connect(portOutputEnables[gpio], &QCheckBox::stateChanged, this, [=](int state){ this->setOutputEnable(gpio, bool(state));});
        connect(portOutputEnables[gpio+1], &QCheckBox::stateChanged, this, [=](int state){ this->setOutputEnable(gpio+1, bool(state));});
        connect(portOutputValues[gpio], &QCheckBox::stateChanged, this, [=](int state){ this->setValue(gpio, bool(state));});
        connect(portOutputValues[gpio+1], &QCheckBox::stateChanged, this, [=](int state){ this->setValue(gpio+1, bool(state));});
    }
}

void GPIODialog::onStateChanged(Digitizer::DIGITIZER_STATE newState)
{


}

void GPIODialog::enableVolatileSettings(bool enabled) 
{
    this->setEnabled(enabled);
}

void GPIODialog::setOutputEnable(int port, bool en)
{
    this->digitizer->setDirectionGPIOPort(0, (1UL&en)<<port, ~(1UL<<port));
}

void GPIODialog::setValue(int port, bool val)
{
    this->digitizer->writeGPIOPort(0, (1UL&val)<<port, ~(1UL<<port));
}

void GPIODialog::readGPIO()
{
    unsigned int values = this->digitizer->readGPIOPort(0);
    for(int gpio=0; gpio < GPIODialog::GPIO_COUNT; gpio++)
    {
        int status = (values >> gpio) & 0b1;
        portInputValues[gpio]->setText(QString::number(status));
    }
}

void GPIODialog::enableSupplyOutput(bool enable)
{
    this->digitizer->enableGPIOSupplyOutput(enable);
}
