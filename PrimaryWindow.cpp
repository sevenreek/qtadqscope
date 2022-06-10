#include "PrimaryWindow.h"
#include "ui_PrimaryWindow.h"

PrimaryWindow::PrimaryWindow(ApplicationContext * context, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PrimaryWindow),
    context(context)
{
    ui->setupUi(this);
    std::string windowTitle = "ADQScope";
#ifdef MOCK_ADQAPI
    windowTitle += " - MOCKADQAPI";
    spdlog::warn("Using debug/mock version of the ADQAPI.");
#endif
#ifdef DEBUG_DMA_DELAY
#if DEBUG_DMA_DELAY > 0
    windowTitle += " - DEBUG SLOW DMA ENABLED";
    spdlog::warn("DMA transfers are purposefully slowed down in this version for debugging purposes! If this is not intentional rebuild the application with DEBUG_DMA_DELAY=0");
#endif
#endif
    this->setWindowTitle(QString::fromStdString(windowTitle));
    connect(this->ui->actionSave, &QAction::triggered, this, &PrimaryWindow::openConfigSaveDialog);
    connect(this->ui->actionLoad, &QAction::triggered, this, &PrimaryWindow::openConfigLoadDialog);

    this->calibrationDialog = std::unique_ptr<FullCalibrationDialog>(new FullCalibrationDialog(this));
    this->buffersDialog = std::unique_ptr<BuffersDialog>(new BuffersDialog(this));
    this->registerDialog = std::unique_ptr<RegisterDialog>(new RegisterDialog(this));
    this->scopeTab = this->ui->scopeTabWidget;
    this->spectroscopeTab = this->ui->spectroscopeTabWidget;
    this->primaryControls = this->ui->primaryControls;


    this->calibrationDialog->initialize(this->context);
    this->buffersDialog->initialize(this->context);
    this->registerDialog->initialize(this->context);
    this->scopeTab->initialize(this->context);
    this->spectroscopeTab->initialize(this->context);
    this->primaryControls->initialize(this->context);

    connect(this->ui->actionCalibration, &QAction::triggered, this, [=]{this->calibrationDialog->reloadUI(); this->calibrationDialog->show();});
    connect(this->ui->actionDMA_Buffers, &QAction::triggered, this, [=]{this->buffersDialog->reloadUI(); this->buffersDialog->show();});
    connect(this->context->digitizer, &Digitizer::digitizerStateChanged, this, &PrimaryWindow::onDigitizerStateChanged);
}

PrimaryWindow::~PrimaryWindow()
{
    delete ui;
}

void PrimaryWindow::reloadUI()
{
    this->scopeTab->reloadUI();

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
    file.open(QFile::OpenModeFlag::ReadOnly);
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &err);
    if(err.error != QJsonParseError::NoError)
    {
        spdlog::error("Failed to parse JSON file {}.", fileName.toStdString());
        return;
    }
    *this->context->config = ApplicationConfiguration::fromJson(json.object());
    this->context->digitizer->setAcquisition(Acquisition::fromJson(json.object()["acquisition"].toObject()));
    this->reloadUI();
}



void PrimaryWindow::onDigitizerStateChanged(Digitizer::DIGITIZER_STATE state)
{
    if(state == Digitizer::DIGITIZER_STATE::ACTIVE)
    {
        this->calibrationDialog->enableVolatileSettings(false);
        this->buffersDialog->enableVolatileSettings(false);
        this->registerDialog->enableVolatileSettings(false);
        this->ui->centralwidget->setStyleSheet("#centralwidget {border: 4px solid red;}");
    }
    else if (state == Digitizer::DIGITIZER_STATE::READY)
    {
        this->calibrationDialog->enableVolatileSettings(true);
        this->buffersDialog->enableVolatileSettings(true);
        this->registerDialog->enableVolatileSettings(true);
        this->ui->centralwidget->setStyleSheet("#centralwidget {border: 0px solid red;}");
    }
}