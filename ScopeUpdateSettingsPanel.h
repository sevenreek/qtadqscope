#ifndef SCOPEUPDATESETTINGSPANEL_H
#define SCOPEUPDATESETTINGSPANEL_H

#include <QWidget>
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
    void initialize(ApplicationContext * context) override;
    void enableVolatileSettings(bool enabled) override;
    void reloadUI() override;

private:
    Ui::ScopeUpdateSettingsPanel *ui;
    ScopeUpdater *scopeUpdater = nullptr;
    void autosetUpdateScope();
    bool scopeUpdaterAdded = false;

private slots:
    void onPlotChannelCheckboxChanged(int ch);



};

#endif // SCOPEUPDATESETTINGSPANEL_H
