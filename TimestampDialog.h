#ifndef TIMESTAMPDIALOG_H
#define TIMESTAMPDIALOG_H

#include <QDialog>
#include "DigitizerGUIComponent.h"
#include "TimestampSyncConfig.h"
#include "TimestampReader.h"

namespace Ui {
class TimestampDialog;
}

class TimestampDialog : public QDialog, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit TimestampDialog(QWidget *parent = nullptr);
    ~TimestampDialog();
    void reloadUI() override;
    void initialize(ApplicationContext *context) override;
    void enableVolatileSettings(bool enabled) override;

private:
    Ui::TimestampDialog *ui;
    TimestampSyncConfig timestampConfig;
    TimestampReader tsReader;
    QTimer timer;
    void updateDigitizerConfig();
private slots:
    void readTimestamp();
    void rearmTimestamp();
    void disarmTimestamp();
    void changeArmOnStart(int state);
    void changeUpdateTimestamp(int state);
    void changeSyncOnce(int state);
    void changeSyncSource(int src);
    void changePXITriggerMask();
    void changeClockSource(int index);
    void loadClockSource();
    void onUpdateTimestamp();
    void onClockOutputEnableChanged(int state);
};

#endif // TIMESTAMPDIALOG_H
