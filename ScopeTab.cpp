#include "ScopeTab.h"
#include "ui_ScopeTab.h"

ScopeTab::ScopeTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScopeTab)
{
    ui->setupUi(this);
    this->acqSettingsPanel = ui->sideSettings;
}

void ScopeTab::initialize(ApplicationContext* context)
{
    DigitizerGUIComponent::initialize(context); // call super
    this->acqSettingsPanel->initialize(this->context);
    this->ui->plotArea->addGraph(); // create the graph for the scope
    this->ui->plotArea->setInteraction(QCP::iRangeDrag, true); // enables mouse dragging on the plot
    this->ui->plotArea->setInteraction(QCP::iRangeZoom, true); // enables zooming with the scroll wheel
    this->triggerLine = new QCPItemLine(this->ui->plotArea); // adds a horizontal line for the trigger level to the plot
    this->triggerLine->setPen(QPen(Qt::red)); // set the color of the line to red
    this->context->scopeUpdater->connect(
        this->context->scopeUpdater,
        &ScopeUpdater::onScopeUpdate,
        this,
        &ScopeTab::updateScope,
        Qt::ConnectionType::BlockingQueuedConnection 
    );
    this->connect(this->context->digitizer, &Digitizer::triggerLevelChanged, this, [this]{this->autoSetTriggerLine();});
    this->connect(this->context->digitizer, &Digitizer::recordLengthChanged, this, [this]{this->autoSetTriggerLine();});
}

ScopeTab::~ScopeTab()
{
    delete ui;
}

void ScopeTab::reloadUI()
{
    this->autoSetTriggerLine();
}

void ScopeTab::replot()
{
    this->ui->plotArea->replot();
}

void ScopeTab::updateScope(QVector<double> &x, QVector<double> y)
{
    this->ui->plotArea->graph(0)->setData(x,y, true);
    //this->mainWindow.ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}

void ScopeTab::autoSetTriggerLine()
{
    int sampleCount = this->context->digitizer->getSamplesPerRecordComplete();
    int pos = this->context->digitizer->getTriggerLevel();
    this->triggerLine->start->setCoords(0, pos);
    this->triggerLine->end->setCoords(sampleCount-1, pos);
    this->replot();
}

void ScopeTab::resetPlot()
{
    this->ui->plotArea->xAxis->setRange(0, this->context->digitizer->getSamplesPerRecordComplete());
    this->ui->plotArea->yAxis->setRange(CODE_MIN, CODE_MAX);
}
