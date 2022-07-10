#ifndef PRIMARYWINDOW_H
#define PRIMARYWINDOW_H

#include <QMainWindow>
#include "Digitizer.h"
#include "AcquisitionSettingsSidePanel.h"
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
#include "PrimaryControls.h"
#include "ApplicationContext.h"
// #include "FullCalibrationDialog.h"
#include "BuffersDialog.h"
#include "SpectroscopeTab.h"
#include "ScopeTab.h"
namespace Ui {
class PrimaryWindow;
}

class PrimaryWindow : public QMainWindow, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit PrimaryWindow(QWidget *parent = nullptr);
    ~PrimaryWindow();
    void reloadUI() override;
    void onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns) override;
    void enableAcquisitionSettings(bool enable) override;

private:
    Ui::PrimaryWindow *ui;
    ScopeTab *scopeTab;
    SpectroscopeTab *spectroscopeTab;
    PrimaryControls *primaryControls;
    // std::unique_ptr<FullCalibrationDialog> calibrationDialog;
    std::unique_ptr<BuffersDialog> buffersDialog;
private slots:
    void openConfigSaveDialog();
    void openConfigLoadDialog();
};

#endif // PRIMARYWINDOW_H
