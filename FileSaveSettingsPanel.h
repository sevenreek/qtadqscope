#ifndef FILESAVESETTINGSPANEL_H
#define FILESAVESETTINGSPANEL_H

#include <QWidget>
#include "DigitizerGUIComponent.h"
#include "ApplicationContext.h"
#include "RecordProcessor.h"
#include "BinaryFileWriter.h"

namespace Ui {
class FileSaveSettingsPanel;
}

class FileSaveSettingsPanel : public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    enum class FileSaveSelectorOptions 
    {
        DISABLED = 0,
        BINARY = 1,
        BINARY_HEADERS = 2,
    };
    explicit FileSaveSettingsPanel(QWidget *parent = nullptr);
    ~FileSaveSettingsPanel();
    void reloadUI() override;
    void onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns) override;
    void enableAcquisitionSettings(bool enabled) override;

private:
    Ui::FileSaveSettingsPanel *ui;
private slots:
    void changeFileSaveMode(int mode);
};

#endif // FILESAVESETTINGSPANELL_H
