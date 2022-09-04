#ifndef PRIMARYCONTROLS_H
#define PRIMARYCONTROLS_H

#include <QWidget>
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
#include "GUILogger.h"
namespace Ui {
class PrimaryControls;
}

class PrimaryControls : public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit PrimaryControls(QWidget *parent = nullptr);
    ~PrimaryControls();
    void dumpAppConfig();
    void reloadUI() override;
    void onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns) override;
    void enableAcquisitionSettings(bool enable) override;
public slots:
    void resetFillIndicators();
private:
    std::shared_ptr<QGUILogSink_mt> logSink;
    Ui::PrimaryControls *ui;
    QTimer periodicUpdateTimer;
private slots:
    void primaryButtonClicked();
    void periodicUIUpdate();
    void enableTimedRun(int enabled);
    void changeRunDuration(int duration);
    void SWTrig();

};

#endif // PRIMARYCONTROLS_H
