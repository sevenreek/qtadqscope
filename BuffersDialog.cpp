#include "BuffersDialog.h"
#include "ui_BuffersDialog.h"

BuffersDialog::BuffersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuffersDialog)
{
    ui->setupUi(this);
}
double BuffersDialog::FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER = 1000000000.0;
BuffersDialog::~BuffersDialog()
{
    delete ui;
}
