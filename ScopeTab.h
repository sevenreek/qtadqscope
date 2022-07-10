#ifndef SCOPETAB_H
#define SCOPETAB_H

#include <QWidget>
#include "DigitizerGUIComponent.h"
#include "AcquisitionSettingsSidePanel.h"
#include "PrimaryControls.h"
#include "DigitizerConstants.h"

namespace Ui {
class ScopeTab;
}

class ScopeTab : public QWidget, public DigitizerGUIComponent
{
    /**
     * @brief ScopeTab is the UI widget corresponding to the primary tab
     * housing the scope (plot) window. It contains the plot window,
     * trigger configuration and AFE controls.
     */
    Q_OBJECT

public:
    explicit ScopeTab(QWidget *parent = nullptr);
    ~ScopeTab();
    void reloadUI() override;
    void onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns) override;
    void enableAcquisitionSettings(bool en) override;

private:
    QCPItemLine * triggerLine = nullptr;
    Ui::ScopeTab *ui;
    AcquisitionSettingsSidePanel *acqSettingsPanel = nullptr;
    void autoSetTriggerLine();
private slots:
    void replot();
    void updateScope(QVector<double> &x, QVector<double> y);
public slots:
    void resetPlot();
};

#endif // SCOPETAB_H
