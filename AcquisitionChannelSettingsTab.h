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
    void reloadUI();
    void setChannelActive(bool act, bool exclusive=false);
    void setTriggerActive(bool act, bool exclusive=false);
    void setExclusive(bool exc);
    void setObtainedRange(double val);
    void initialize(ApplicationContext * context, int index);
private:
    int channel;
    static int lastActivatedChannel;
    Ui::AcquisitionChannelSettingsTab *ui;
    void setOffsetSource(bool fromZero);
    void setTriggerLevel(int val);
    void setDCOffset(int val);

signals:
    void channelActiveChanged(bool act);
    void triggerActiveChanged(bool act);

};

#endif // ACQUISITIONCHANNELSETTINGSTAB_H

