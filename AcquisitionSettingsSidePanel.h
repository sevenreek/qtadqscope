#ifndef ACQUISITIONSETTINGS_H
#define ACQUISITIONSETTINGS_H

#include <QWidget>
#include "AcquisitionChannelSettingsTab.h"
#include <array>
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
#include "util.h"
namespace Ui {
class AcquisitionSettingsSidePanel;
}

class AcquisitionSettingsSidePanel : public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit AcquisitionSettingsSidePanel(QWidget *parent = nullptr);
    ~AcquisitionSettingsSidePanel();
    void reloadUI() override;
    void initialize(ApplicationContext * context) override;
    static std::string calculateFrequency(unsigned long long samplingRate, unsigned long long sampleSkip);
    void enableVolatileSettings(bool enabled) override;
private:
    enum class TriggerModeOptions {
        CONTINUOUS = 0,
        SOFTWARE = 1,
        LEVEL = 2,
        INTERNAL = 3,
        EXTERNAL = 4
    };
    int lastActiveChannel = 0;
    Ui::AcquisitionSettingsSidePanel *ui;
    std::array<std::unique_ptr<AcquisitionChannelSettingsTab>, MAX_NOF_CHANNELS> tabs;
    int triggerModeToSelectIndex(TRIGGER_MODES mode);
    void createChannelSettingsTabs();
    void changeTabNames();

public slots:
    void handleTabChanged(int tab);
    // void handleSetChannelActive(int channel, bool active);
    // void handleSetChannelTriggerActive(int channel, bool active);
private slots:
    void setUL1Bypass(bool bypass);
    void setUL2Bypass(bool bypass);
    void setHorizontalShift(int val);
    void setRecordLength(int val);
    void setRecordCount(int val);
    void setSampleSkip(int val);
    void setLimitRecordsEnabled(int state);
    void setTriggerMode(int tmode);
    void setAcquisitionTag(const QString &str);

signals:
    void onChannelTabChanged(int tab);
};

#endif // ACQUISITIONSETTINGS_H
