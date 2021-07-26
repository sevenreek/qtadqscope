#ifndef BUFFERSDIALOG_H
#define BUFFERSDIALOG_H

#include <QDialog>

namespace Ui {
class BuffersDialog;
}

class BuffersDialog : public QDialog
{

    Q_OBJECT
public:
    enum DEFAULT_CONFIGS {
        NONE,
        BALANCED,
        SHORT_PULSE_DISPLAY,
        LONG_PULSE_DISPLAY,
        FILE_SAVE,
        LARGE_FILE_SAVE
    };
    struct DefaultConfig {
        unsigned int bufferCount;
        unsigned long long bufferSize;
        unsigned int queueSize;
    };
    static const int DEFAULT_CONFIG_COUNT = 6;
    static const DefaultConfig DEFAULT_CONFIG_VALUES[DEFAULT_CONFIG_COUNT];
    static double FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER;
    explicit BuffersDialog(QWidget *parent = nullptr);
    ~BuffersDialog();
    Ui::BuffersDialog *ui;

private:
};

#endif // BUFFERSDIALOG_H
