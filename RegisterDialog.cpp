#include "RegisterDialog.h"
#include "ui_RegisterDialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}


void RegisterDialog::initialize(ApplicationContext *context)
{
    this->digitizer = context->digitizer;
    this->connect(this, &QDialog::accepted, this, &RegisterDialog::apply);
}

void RegisterDialog::reloadUI()
{
}

void RegisterDialog::apply()
{
    unsigned int algorithmMode = this->ui->triggerAlgoithmSelector->currentIndex();
    unsigned int activeChannels =
        (this->ui->activeChannel1->checkState()>0 ? 1<<0 : 0 ) |
        (this->ui->activeChannel2->checkState()>0 ? 1<<1 : 0 ) |
        (this->ui->activeChannel3->checkState()>0 ? 1<<2 : 0 ) |
        (this->ui->activeChannel4->checkState()>0 ? 1<<3 : 0 );
    unsigned int passthrough =
        (this->ui->passthroughChannel1->checkState()>0 ? 1<<0 : 0 ) |
        (this->ui->passthroughChannel2->checkState()>0 ? 1<<1 : 0 ) |
        (this->ui->passthroughChannel3->checkState()>0 ? 1<<2 : 0 ) |
        (this->ui->passthroughChannel4->checkState()>0 ? 1<<3 : 0 ) |
        (this->ui->moduloPassthrough->checkState()>0 ? 1<<4 : 0 )    ;

    unsigned int retval;
    unsigned int algorithmConfig = ( algorithmMode | (activeChannels << 4) | ( passthrough << 8 ));
    this->digitizer->writeUserRegister(1, 0x10, 0, algorithmConfig, &retval);
    if(retval != algorithmConfig) spdlog::error("Failed to set algorithm configuration");

    short dcOffsetValue = this->ui->algorithmParamInput0->value();
    spdlog::debug("Setting DC offset register to {}", dcOffsetValue);
    this->digitizer->writeUserRegister(1, 0x11, 0, dcOffsetValue, &retval);
    if(dcOffsetValue != (short)retval) spdlog::error("Failed to set DC offset register");


    short algParam1 = this->ui->algorithmParamInput1->value();
    spdlog::debug("Setting alg_param1 register to {}", algParam1);
    this->digitizer->writeUserRegister(1, 0x12, 0, algParam1, &retval);
    if(algParam1 != (short)retval) spdlog::error("Failed to set algParam1 register");
}


void RegisterDialog::enableVolatileSettings(bool enabled)
{

}
