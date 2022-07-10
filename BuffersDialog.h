#ifndef BUFFERSDIALOG_H
#define BUFFERSDIALOG_H

#include <QDialog>
#include "DigitizerGUIComponent.h"

namespace Ui {
class BuffersDialog;
}

class BuffersDialog : public QDialog, public DigitizerGUIComponent
{

    Q_OBJECT
public:
    enum CONFIG_PRESETS {
        NONE,
        BALANCED,
        SHORT_PULSE_DISPLAY,
        LONG_PULSE_DISPLAY,
        FILE_SAVE,
        LARGE_FILE_SAVE
    };
    struct ConfigPreset {
        unsigned int bufferCount;
        unsigned long long bufferSize;
        unsigned int queueSize;
    };
    static const int DEFAULT_CONFIG_COUNT = 6;
    static const ConfigPreset DEFAULT_CONFIG_VALUES[DEFAULT_CONFIG_COUNT];
    static double FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER;
    explicit BuffersDialog(QWidget *parent = nullptr);
    ~BuffersDialog();
    Ui::BuffersDialog *ui;
    void onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns) override;
    void reloadUI() override;
private slots:
    void saveValuesToActiveAcquisition();
    void loadPreconfiguredBuffer(int index);
public:
    void enableAcquisitionSettings(bool enabled) override;
};

#endif // BUFFERSDIALOG_H
