#ifndef PRIMARYWINDOW_H
#define PRIMARYWINDOW_H

#include <QMainWindow>
#include "Digitizer.h"
#include "AcquisitionSettingsSidePanel.h"
#include "PrimaryControls.h"
#include "ApplicationContext.h"
#include "FullCalibrationDialog.h"
#include "BuffersDialog.h"
#include "RegisterDialog.h"
#include "SpectroscopeTab.h"
#include "ScopeTab.h"
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
    ApplicationContext *context;
    ScopeTab *scopeTab;
    SpectroscopeTab *spectroscopeTab;
    PrimaryControls *primaryControls;
    std::unique_ptr<FullCalibrationDialog> calibrationDialog;
    std::unique_ptr<BuffersDialog> buffersDialog;
    std::unique_ptr<RegisterDialog> registerDialog;
private slots:
    void openConfigSaveDialog();
    void openConfigLoadDialog();
    void onDigitizerStateChanged(Digitizer::DIGITIZER_STATE state);
};

#endif // PRIMARYWINDOW_H
