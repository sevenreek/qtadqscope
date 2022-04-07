#ifndef ACQUISITIONCHANNELSETTINGSTAB_H
#define ACQUISITIONCHANNELSETTINGSTAB_H

#include <QWidget>
#include "DigitizerGUIComponent.h"
namespace Ui {
class AcquisitionChannelSettingsTab;
}

class AcquisitionChannelSettingsTab : public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit AcquisitionChannelSettingsTab(QWidget *parent = nullptr);
    ~AcquisitionChannelSettingsTab();
    void reloadUI() override;
    void setChannelActive(bool act);
    void setTriggerActive(bool act);
    void setTriggerChangeAllowed(bool allowed);
    void setActiveChangeAllowed(bool allowed);
    void setObtainedRange(double val);
    void initialize(ApplicationContext * context, int index);
    void enableVolatileSettings(bool enabled) override;
private:
    int channel;
    static int lastActivatedChannel;
    Ui::AcquisitionChannelSettingsTab *ui;
    void setOffsetSource(bool fromZero);
    void setTriggerLevel(int val);
    void setTriggerLevelMv(double val);
    void setDCOffset(int val);
    void invalidateTriggerLevels();
    void invalidateDCOffset();
    void setDCOffsetMv(double mv);
    bool allowAcqChange = true;
    bool allowTriggerChange = true;

signals:
    void channelActiveChanged(bool act);
    void triggerActiveChanged(bool act);

};

#endif // ACQUISITIONCHANNELSETTINGSTAB_H

