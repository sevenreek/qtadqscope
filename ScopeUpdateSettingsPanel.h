#ifndef SCOPEUPDATESETTINGSPANEL_H
#define SCOPEUPDATESETTINGSPANEL_H

#include <QWidget>
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
#include "ApplicationContext.h"
#include "ScopeUpdater.h"


namespace Ui {
class ScopeUpdateSettingsPanel;
}

class ScopeUpdateSettingsPanel : public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit ScopeUpdateSettingsPanel(QWidget *parent = nullptr);
    ~ScopeUpdateSettingsPanel();
    void enableAcquisitionSettings(bool enabled) override;
    void onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns) override;
    void reloadUI() override;

private:
    Ui::ScopeUpdateSettingsPanel *ui;
    std::unique_ptr<ScopeUpdater> scopeUpdater;
    bool scopeUpdaterAdded = false;

private slots:
    void changePlotChannel(int ch);
    void setUpdateScope(int enable);



};

#endif // SCOPEUPDATESETTINGSPANEL_H
