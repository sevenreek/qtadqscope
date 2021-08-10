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
    void initialize(ApplicationContext *context);
    void reloadUI();
private:
};

#endif // BUFFERSDIALOG_H
