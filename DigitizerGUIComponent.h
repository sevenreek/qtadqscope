#ifndef DIGITIZERGUICOMPONENT_H
#define DIGITIZERGUICOMPONENT_H
#include <QObject>
#include <QWidget>
#include "Digitizer.h"
#include "ApplicationContext.h"

class DigitizerGUIComponent
{
protected:
    Digitizer *digitizer = nullptr;
    ApplicationConfiguration *config;
public:
    virtual void initialize(ApplicationContext * context)
    {
        this->digitizer = context->digitizer;
        this->config = context->config;
    }
    virtual void reloadUI() = 0;
    virtual void enableVolatileSettings(bool enabled) = 0;
};



#endif // DIGITIZERGUICOMPONENT_H
