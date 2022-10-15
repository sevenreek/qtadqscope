#ifndef PRIMARYWINDOW_H
#define PRIMARYWINDOW_H

#include <QMainWindow>
#include "Digitizer.h"
#include "AcquisitionSettings.h"
#include "PrimaryControls.h"
#include "ApplicationContext.h"
#include "FullCalibrationDialog.h"
#include "BuffersDialog.h"
#include "RegisterDialog.h"
#include "SpectrumDialog.h"
#include "GpioDialog.h"
namespace Ui {
class PrimaryWindow;
}

class PrimaryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PrimaryWindow(ApplicationContext * context, QWidget *parent = nullptr);
    ~PrimaryWindow();
    void reloadUI();

private:
    Ui::PrimaryWindow *ui;
    ApplicationContext * context;
    AcquisitionSettings *acqSettings = nullptr;
    PrimaryControls *primaryControls = nullptr;
    QCPItemLine * triggerLine;
    std::unique_ptr<FullCalibrationDialog> calibrationDialog;
    std::unique_ptr<BuffersDialog> buffersDialog;
    std::unique_ptr<RegisterDialog> registerDialog;
    std::unique_ptr<SpectrumDialog> spectrumDialog;
    std::unique_ptr<GPIODialog> gpioDialog;

private slots:
    void openConfigSaveDialog();
    void openConfigLoadDialog();
    void replot();
    void updateScope(QVector<double> &x, QVector<double> y);
    void autoSetTriggerLine();
    void onDigitizerStateChanged(Digitizer::DIGITIZER_STATE state);
    void openSpectrumAnalyzer();
};

#endif // PRIMARYWINDOW_H
