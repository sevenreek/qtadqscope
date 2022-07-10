#ifndef ACQUISITIONCHANNELSETTINGSTAB_H
#define ACQUISITIONCHANNELSETTINGSTAB_H

#include <QWidget>
#include "DigitizerConstants.h"
#include "DigitizerGUIComponent.h"
namespace Ui {
class AcquisitionChannelSettingsTab;
}

class AcquisitionChannelSettingsTab : public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit AcquisitionChannelSettingsTab(int index, QWidget *parent = nullptr);
    ~AcquisitionChannelSettingsTab();


// DigitizerGUIComponent interface
    void reloadUI() override;
    void onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns) override;
    void enableAcquisitionSettings(bool enabled) override;
private:
    int channel;
    static int lastActivatedChannel;
    Ui::AcquisitionChannelSettingsTab *ui;
    void setOffsetSource(bool fromZero);
    void invalidateTriggerLevels();
    void invalidateDCOffset();
    bool allowAcqChange = true;
    bool allowTriggerChange = true;
    int inputRangeToSelectBoxPosition(int inputRange);

private slots:
    void setChannelActive(bool act);
    void setTriggerActive(bool act);
    void setTriggerChangeAllowed(bool allowed);
    void setActiveChangeAllowed(bool allowed);
    void setObtainedRange(double val);
    void setTriggerLevel(int val);
    void setTriggerLevelMv(double val);
    void setDCOffset(int val);
    void setDCOffsetMv(double mv);

    void setTriggerEdge(int edge);
    void setTriggerReset(int reset);
    void setInputRange(int inputRangeEnum);

signals:
    void channelActiveChanged(bool act);
    void triggerActiveChanged(bool act);

};

#endif // ACQUISITIONCHANNELSETTINGSTAB_H

