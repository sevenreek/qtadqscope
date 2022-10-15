#include "AuxTriggerDialog.h"
#include "ui_AuxTriggerDialog.h"

AuxTriggerDialog::AuxTriggerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuxTriggerDialog)
{
    ui->setupUi(this);
}

AuxTriggerDialog::~AuxTriggerDialog()
{
    delete ui;
}
