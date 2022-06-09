#ifndef SPECTRUMDIALOG_H
#define SPECTRUMDIALOG_H

#include <QDialog>
#include "spdlog/spdlog.h"
#include <memory>
#include "DigitizerGUIComponent.h"
#include "SpectrumPlotter.h"
#include "RegisterConstants.h"

namespace Ui {
class SpectroscopeTab;
}

class SpectroscopeTab: public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit SpectroscopeTab(QWidget *parent = nullptr);
    ~SpectroscopeTab();
    void reloadUI() override;
    void initialize(ApplicationContext * context) override;
    void enableVolatileSettings(bool enabled) override;

private:
    QVector<double> x;
    QVector<double> y;
    std::unique_ptr<SpectrumPlotter> plotter;
    bool isPlotterActive = false;
    Ui::SpectroscopeTab *ui;
    ApplicationContext * context;
    int windowDuration = 0;
    unsigned int spectrumBinCount = MAX_SPECTRUM_BIN_COUNT;
    void loadConfigFromDevice();
    void reallocatePlotSize(int binCount);
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
    void setSpectroscopeEnabled(int checked);
public slots:
    void updateScope(QVector<double> &x, QVector<double> y);
    void updateSpectrumCalculatedParams(unsigned long long totalCount);
};

#endif // SPECTRUMDIALOG_H
