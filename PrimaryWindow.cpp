#include "PrimaryWindow.h"
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
#include "ui_PrimaryWindow.h"

PrimaryWindow::PrimaryWindow(QWidget *parent)
    : QMainWindow(parent), DigitizerGUIComponent(), ui(new Ui::PrimaryWindow) {
  ui->setupUi(this);
  std::string windowTitle = "ADQScope";
#ifdef MOCK_ADQAPI
  windowTitle += " - MOCKADQAPI";
  spdlog::warn("Using debug/mock version of the ADQAPI.");
#endif
#ifdef DEBUG_DMA_DELAY
#if DEBUG_DMA_DELAY > 0
  windowTitle += " - DEBUG SLOW DMA ENABLED";
  spdlog::warn("DMA transfers are purposefully slowed down in this version for "
               "debugging purposes! If this is not intentional rebuild the "
               "application with DEBUG_DMA_DELAY=0");
#endif
#endif
  this->setWindowTitle(QString::fromStdString(windowTitle));
  connect(this->ui->actionSave, &QAction::triggered, this,
          &PrimaryWindow::openConfigSaveDialog);
  connect(this->ui->actionLoad, &QAction::triggered, this,
          &PrimaryWindow::openConfigLoadDialog);

  // this->calibrationDialog = std::unique_ptr<FullCalibrationDialog>(new
  // FullCalibrationDialog(this));
  this->buffersDialog = std::unique_ptr<BuffersDialog>(new BuffersDialog(this));
  this->scopeTab = this->ui->scopeTabWidget;
  this->spectroscopeTab = this->ui->spectroscopeTabWidget;
  this->primaryControls = this->ui->primaryControls;

  // connect(this->ui->actionCalibration, &QAction::triggered, this,
  // [=]{this->calibrationDialog->reloadUI();
  // this->calibrationDialog->show();});
  connect(this->ui->actionDMA_Buffers, &QAction::triggered, this, [=] {
    this->buffersDialog->reloadUI();
    this->buffersDialog->show();
  });
  connect(&this->digitizer, &Digitizer::acquisitionStateChanged, this,
          [this](AcquisitionStates os, AcquisitionStates ns) {
            this->onAcquisitionStateChanged(os, ns);
          });
}

PrimaryWindow::~PrimaryWindow() { delete ui; }

void PrimaryWindow::reloadUI() {
  this->scopeTab->reloadUI();
  this->spectroscopeTab->reloadUI();
  this->primaryControls->reloadUI();
  this->buffersDialog->reloadUI();
  // this->calibrationDialog->reloadUI();
}

void PrimaryWindow::openConfigSaveDialog() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Save config", "", "JSON config (*.json);;All Files (*)");
  if (!fileName.length())
    return;
  this->context.config()->saveToFile(fileName.toStdString());
}

void PrimaryWindow::openConfigLoadDialog() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Load config", "", "JSON config (*.json);;All Files (*)");
  if (!fileName.length())
    return;
  this->context.config()->loadFromFile(fileName.toStdString());
  this->reloadUI();
}

void PrimaryWindow::onAcquisitionStateChanged(AcquisitionStates os,
                                              AcquisitionStates ns) {
  // Paint the border red to indicate that acquisition is running
  if (ns == AcquisitionStates::ACTIVE) {
    this->ui->centralwidget->setStyleSheet(
        "#centralwidget {border: 4px solid red;}");
  } else if (ns == AcquisitionStates::INACTIVE) {
    this->ui->centralwidget->setStyleSheet(
        "#centralwidget {border: 0px solid red;}");
  }
}
void PrimaryWindow::enableAcquisitionSettings(bool enable) {
  // do nothing
}