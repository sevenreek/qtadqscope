#ifndef FULLCALIBRATIONDIALOG_H
#define FULLCALIBRATIONDIALOG_H

#include <QDialog>
#include "CalibrationTable.h"
#include "Acquisition.h"
#include "SignalParameterComputer.h"
#include "spdlog/spdlog.h"
#include <memory>
enum CALIBRATION_MODES {
    ANALOG,
    DIGITAL,
    FINE_DIGITAL
};
struct FullCalibrationSetup {
    CALIBRATION_MODES mode;
    int channel = 0;
    int inputRange = 0;
};

namespace Ui {
class FullCalibrationDialog;
}

class FullCalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FullCalibrationDialog(std::shared_ptr<ApplicationConfiguration> appConfig, std::shared_ptr<Acquisition> acq, QWidget *parent = nullptr);
    ~FullCalibrationDialog();
    Ui::FullCalibrationDialog *ui;

private:
    std::shared_ptr<ApplicationConfiguration> appConfig;
    std::shared_ptr<ApplicationConfiguration> calibrationConfiguration;
    std::shared_ptr<SignalParameterComputer> parameterComputer;
    std::shared_ptr<Acquisition> acquisition;
    std::vector<FullCalibrationSetup> setups;
    QSpinBox * analogValues[MAX_NOF_CHANNELS];
    QSpinBox * digitalValues[MAX_NOF_CHANNELS];
    unsigned long currentSetupIndex = 0;
    std::unique_ptr<CalibrationTable> calibrationTable;
    bool acquisitionActive = false;
    void configureNextStage();
    bool runStage();

    void blockUI();
    void unblockUI();
    bool moveToNextStage();
    void appendStage(int ch, int inrange, int mode);
    CALIBRATION_MODES mode;
public slots:
    void onStateChanged(ACQUISITION_STATES newState);
    void apply();
    void start();
    void load();
    void save();
    void stopAcquisitions();
    void changeInputRange(int v);
signals:
    void calibrationApply(CalibrationTable& table);
};

#endif // FULLCALIBRATIONDIALOG_H
