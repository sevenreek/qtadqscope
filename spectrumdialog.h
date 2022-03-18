#ifndef SPECTRUMDIALOG_H
#define SPECTRUMDIALOG_H

#include <QDialog>
#include "spdlog/spdlog.h"
#include <memory>
#include "DigitizerGUIComponent.h"
#include "SpectrumPlotter.h"
#include "RegisterConstants.h"

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
    std::unique_ptr<SpectrumPlotter> plotter;
    bool isPlotterActive = false;
    Ui::SpectrumDialog *ui;
    ApplicationContext * context;
    int windowDuration = 0;
    unsigned int spectrumBinCount = SPECTRUM_BIN_COUNT;
    void loadConfigFromDevice();
private slots:
    void downloadSpectrum();
    void loadSpectrum();
    void saveSpectrum();
    void resetSpectrum();
    void setTriggerLevel();
    void debugSpectrum();
    void changeSpectrumDMAEnabled(int checked);
    void changeUseZCDTrigger(int checked);
    void changePlotChannel(int selection);
    void changeSpectrumBinCount(int count);
    void setSpectrumWindow();
public slots:
    void updateScope(QVector<double> &x, QVector<double> y);
    void updateSpectrumCalculatedParams(unsigned long long totalCount);
};

#endif // SPECTRUMDIALOG_H
