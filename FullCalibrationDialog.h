#ifndef FULLCALIBRATIONDIALOG_H
#define FULLCALIBRATIONDIALOG_H

#include <QDialog>
#include "CalibrationTable.h"
#include "SignalParameterComputer.h"
#include "spdlog/spdlog.h"
#include <memory>
#include "DigitizerGUIComponent.h"
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

class FullCalibrationDialog : public QDialog, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit FullCalibrationDialog(QWidget *parent = nullptr);
    ~FullCalibrationDialog();
    Ui::FullCalibrationDialog *ui;
    void reloadUI();

private:
    std::unique_ptr<SignalParameterComputer> parameterComputer;
    std::vector<FullCalibrationSetup> setups;
    QSpinBox * analogValues[MAX_NOF_CHANNELS];
    QSpinBox * digitalValues[MAX_NOF_CHANNELS];
    unsigned long currentSetupIndex = 0;
    CalibrationTable calibrationTable;
    bool acquisitionActive = false;
    bool runStage();
    Acquisition acquisitionFromStage(const FullCalibrationSetup &stage);
    void blockUI();
    void unblockUI();
    bool moveToNextStage();
    void appendStage(int ch, int inrange, int mode);
    CALIBRATION_MODES mode;
    std::list<IRecordProcessor*> calibrationProcessorsList;
    Acquisition currentCalibrationAcquisition;
    QLabel *labels[MAX_NOF_CHANNELS];
    void setAnalogValue(int ch, int ir, int val);
    void setDigitalValue(int ch, int ir, int val);
public slots:
    void onStateChanged(Digitizer::DIGITIZER_STATE newState);
    void apply();
    void startCalibration();
    void load();
    void save();
    void stopAcquisitions();
    void changeInputRange(int v);
    void initialize(ApplicationContext *context);
signals:
    void calibrationApply(CalibrationTable& table);

    // DigitizerGUIComponent interface
public:
    void enableVolatileSettings(bool enabled) override;
};

#endif // FULLCALIBRATIONDIALOG_H
