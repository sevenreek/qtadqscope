#include "PrimaryWindow.h"
#include "ui_PrimaryWindow.h"

PrimaryWindow::PrimaryWindow(ApplicationContext * context, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PrimaryWindow),
    context(context)
{
    ui->setupUi(this);
    this->primaryControls = ui->primaryControls;
    this->primaryControls->initialize(this->context);
    this->acqSettings = ui->sideSettings;
    this->acqSettings->initialize(this->context);
    connect(this->ui->actionSave, &QAction::triggered, this, &PrimaryWindow::openConfigSaveDialog);
    connect(this->ui->actionLoad, &QAction::triggered, this, &PrimaryWindow::openConfigLoadDialog);
    this->ui->plotArea->addGraph();
    this->ui->plotArea->setInteraction(QCP::iRangeDrag, true);
    this->ui->plotArea->setInteraction(QCP::iRangeZoom, true);
    this->triggerLine = new QCPItemLine(this->ui->plotArea);
    this->triggerLine->setPen(QPen(Qt::red));
    this->context->scopeUpdater->connect(
        this->context->scopeUpdater,
        &ScopeUpdater::onScopeUpdate,
        this,
        &PrimaryWindow::updateScope,
        Qt::ConnectionType::BlockingQueuedConnection
    );
    this->primaryControls->connect(
        this->primaryControls,
        &PrimaryControls::resetPlot,
        this,
        [=]{
            this->ui->plotArea->xAxis->setRange(0, this->context->digitizer->getSamplesPerRecordComplete());
            this->ui->plotArea->yAxis->setRange(-(1<<15), 1<<15);
        }
    );
    this->calibrationDialog = std::unique_ptr<FullCalibrationDialog>(new FullCalibrationDialog(this));
    this->buffersDialog = std::unique_ptr<BuffersDialog>(new BuffersDialog(this));
    this->calibrationDialog->initialize(this->context);
    this->buffersDialog->initialize(this->context);
    connect(this->ui->actionCalibration, &QAction::triggered, this, [=]{this->calibrationDialog->reloadUI(); this->calibrationDialog->show();});
    connect(this->ui->actionDMA_Buffers, &QAction::triggered, this, [=]{this->buffersDialog->reloadUI(); this->buffersDialog->show();});
}

PrimaryWindow::~PrimaryWindow()
{
    delete ui;
}

void PrimaryWindow::reloadUI()
{
    this->primaryControls->reloadUI();
    this->acqSettings->reloadUI();
    this->autoSetTriggerLine();
}

void PrimaryWindow::openConfigSaveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save config",
        "",
        "JSON config (*.json);;All Files (*)"
    );
    if(!fileName.length()) return;
    QFile file(fileName);
    file.open(QFile::OpenModeFlag::WriteOnly);
    QJsonDocument doc;
    QJsonObject cfg = this->context->config->toJson();
    QJsonObject acq = this->context->digitizer->getAcquisition().toJson();
    cfg.insert("acquisition", acq);
    doc.setObject(cfg);
    file.write(doc.toJson());
    file.close();
}

void PrimaryWindow::openConfigLoadDialog()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Load config",
        "",
        "JSON config (*.json);;All Files (*)"
    );
    if(!fileName.length()) return;
    QFile file(fileName);
    if(!file.exists())
    {
        spdlog::error("File {} does not exist.", fileName.toStdString());
        return;
    }
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &err);
    if(err.error != QJsonParseError::NoError)
    {
        spdlog::error("Failed to parse JSON file {}.", fileName.toStdString());
        return;
    }
    *this->context->config = ApplicationConfiguration::fromJson(json.object());
    this->context->digitizer->setAcquisition(Acquisition::fromJson(json["acquisition"].toObject()));
    this->reloadUI();
}

void PrimaryWindow::replot()
{
    this->ui->plotArea->replot();
}

void PrimaryWindow::updateScope(QVector<double> &x, QVector<double> y)
{
    this->ui->plotArea->graph(0)->setData(x,y);
    //this->mainWindow.ui->plotArea->rescaleAxes();
    this->ui->plotArea->replot();
}

void PrimaryWindow::autoSetTriggerLine()
{
    int sampleCount = this->context->digitizer->getSamplesPerRecordComplete();
    int pos = this->context->digitizer->getTriggerLevel();
    this->triggerLine->start->setCoords(0, pos);
    this->triggerLine->end->setCoords(sampleCount-1, pos);
    this->replot();
}