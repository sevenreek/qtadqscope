#ifndef RECORDPROCESSORSPANEL_H
#define RECORDPROCESSORSPANEL_H

#include <QWidget>
#include "DigitizerGUIComponent.h"
namespace Ui {
class RecordProcessorsPanel;
}

class RecordProcessorsPanel : public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit RecordProcessorsPanel(QWidget *parent = nullptr);
    ~RecordProcessorsPanel();
    void initialize(ApplicationContext * context) override;
    void reloadUI() override;
private:
    Ui::RecordProcessorsPanel *ui;
    ScopeUpdater *scopeUpdater;
    ApplicationContext *context;
    std::unique_ptr<IRecordProcessor> fileSaver = std::unique_ptr<IRecordProcessor>(nullptr);
    void autosetFileSaver();
    bool scopeUpdaterAdded = false;
    void autosetUpdateScope();

    // DigitizerGUIComponent interface
public:
    void enableVolatileSettings(bool enabled) override;
};

#endif // RECORDPROCESSORSPANEL_H
