#ifndef AUTOCALIBRATEDIALOG_H
#define AUTOCALIBRATEDIALOG_H
#include "Acquisition.h"
#include <QDialog>
#include "SignalParameterComputer.h"

enum CALIBRATION_MODES {
    ANALOG,
    DIGITAL,
    FINE_DIGITAL
};
enum CALIBRATION_RESULT {
    SUCCESS,
    OVERFLOW_DEVICE,
    OVERFLOW_RAM,
    UNKNOWN
};
namespace Ui {
class AutoCalibrateDialog;
}

class AutoCalibrateDialog : public QDialog
{
    Q_OBJECT
protected:
    std::shared_ptr<ApplicationConfiguration> appConfig;
    std::shared_ptr<ApplicationConfiguration> calibrationConfiguration;
    std::shared_ptr<SignalParameterComputer> parameterComputer;
    std::shared_ptr<Acquisition> acquisition;
    int computedAverage;
    void configure();
    void apply();
public slots:
    void onStateChanged(ACQUISITION_STATES newState);
public:
    void start();
    void showDialog();
    explicit AutoCalibrateDialog(std::shared_ptr<ApplicationConfiguration> appConfig,
                                 std::shared_ptr<Acquisition> acq, QWidget *parent = nullptr);
    ~AutoCalibrateDialog();
    Ui::AutoCalibrateDialog *ui;
signals:
    void offsetCalculated(CALIBRATION_MODES mode, int offset);
};

#endif // AUTOCALIBRATEDIALOG_H
