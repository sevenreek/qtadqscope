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
    ApplicationContext *context = nullptr;
    Digitizer *digitizer = nullptr;
public:
    virtual void initialize(ApplicationContext * context)
    {
        this->context = context;
        this->digitizer = context->digitizer;
    }
    virtual void reloadUI() {};
    virtual void onAcquisitionStateChanged(AcquisitionStates os, AcquisitionStates ns) override {};
};



#endif // DIGITIZERGUICOMPONENT_H
