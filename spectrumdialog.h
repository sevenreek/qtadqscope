#ifndef SPECTRUMDIALOG_H
#define SPECTRUMDIALOG_H

#include <QDialog>
#include "spdlog/spdlog.h"
#include <memory>
#include "DigitizerGUIComponent.h"

namespace Ui {
class SpectrumDialog;
}

class SpectrumDialog : public QDialog, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit SpectrumDialog(QWidget *parent = nullptr);
    ~SpectrumDialog();
    void reloadUI() override;
    void initialize(ApplicationContext * context) override;
    void enableVolatileSettings(bool enabled) override;

private:
    QVector<double> x;
    QVector<double> y;
    Ui::SpectrumDialog *ui;
    ApplicationContext * context;
private slots:
    void downloadSpectrum();
    void loadSpectrum();
    void saveSpectrum();
    void resetSpectrum();
    void setTriggerLevel();
    void debugSpectrum();
};

#endif // SPECTRUMDIALOG_H
