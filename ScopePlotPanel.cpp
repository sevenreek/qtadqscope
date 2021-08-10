#include "ScopePlotPanel.h"
#include "ui_ScopePlotPanel.h"

ScopePlotPanel::ScopePlotPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScopePlotPanel)
{
    ui->setupUi(this);
}

ScopePlotPanel::~ScopePlotPanel()
{
    delete ui;
}
