#ifndef ACQUISITIONSETTINGS_H
#define ACQUISITIONSETTINGS_H

#include <QWidget>
#include "AcquisitionChannelSettingsTab.h"
#include <array>
#include "DigitizerGUIComponent.h"
#include "util.h"
namespace Ui {
class AcquisitionSettings;
}

class AcquisitionSettings : public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit AcquisitionSettings(QWidget *parent = nullptr);
    ~AcquisitionSettings();
    void reloadUI();
    void initialize(ApplicationContext * context);
    static std::string calculateFrequency(unsigned long long samplingRate, unsigned long long sampleSkip);
    void enableVolatileSettings(bool enabled) override;
private:
    int lastActiveChannel = 0;
    Ui::AcquisitionSettings *ui;
    std::array<std::unique_ptr<AcquisitionChannelSettingsTab>, MAX_NOF_CHANNELS> tabs;
public slots:
    void handleTabChanged(int tab);
    void handleSetChannelActive(int channel, bool active);
    void handleSetChannelTriggerActive(int channel, bool active);
    void handleTabNameChange(int channel, bool recreateAll=false);
    void handleApproachChanged(int approach);

    // DigitizerGUIComponent interface
signals:
    void onChannelTabChanged(int tab);
};

#endif // ACQUISITIONSETTINGS_H
