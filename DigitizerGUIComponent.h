#ifndef DIGITIZERGUICOMPONENT_H
#define DIGITIZERGUICOMPONENT_H
#include <QObject>
#include <QWidget>
#include "AcquisitionStateChangeListener.h"
#include "Digitizer.h"
#include "ApplicationContext.h"
#include "ConfigurationController.h"
#include "DigitizerConstants.h"
class DigitizerGUIComponent : public AcquisitionStateChangeListener
{
protected:
    ApplicationContext &context;
    Digitizer &digitizer;
public:
    DigitizerGUIComponent(): context(ApplicationContext::get()), digitizer(*context.digitizer()) {};
    virtual ~DigitizerGUIComponent() {};
    virtual void reloadUI() = 0;
    virtual void onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns) override {};
    virtual void enableAcquisitionSettings(bool enabled) = 0;
};



#endif // DIGITIZERGUICOMPONENT_H
