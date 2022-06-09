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
    explicit FileSaveSettingsPanel(QWidget *parent = nullptr);
    ~FileSaveSettingsPanel();
    void initialize(ApplicationContext * context) override;
    void reloadUI() override;
    void enableVolatileSettings(bool enabled) override;

private:
    Ui::FileSaveSettingsPanel *ui;
    std::unique_ptr<IRecordProcessor> fileSaver;
    void autosetFileSaver();
};

#endif // FILESAVESETTINGSPANELL_H
