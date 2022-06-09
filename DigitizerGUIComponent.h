#ifndef DIGITIZERGUICOMPONENT_H
#define DIGITIZERGUICOMPONENT_H
#include <QObject>
#include <QWidget>
#include "Digitizer.h"
#include "ApplicationContext.h"

class DigitizerGUIComponent
{
protected:
    ApplicationContext *context = nullptr;
    Digitizer *digitizer = nullptr;
    ApplicationConfiguration *config = nullptr;
public:
    virtual void initialize(ApplicationContext * context)
    {
        this->context = context;
        this->digitizer = context->digitizer;
        this->config = context->config;
    }
    virtual void reloadUI() {};
    virtual void enableVolatileSettings(bool enabled) {};
    virtual void onAcquisitionStateChanged(Digitizer::DIGITIZER_STATE state) {};
};



#endif // DIGITIZERGUICOMPONENT_H
