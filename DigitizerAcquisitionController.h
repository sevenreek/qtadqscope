#ifndef DIGITIZERACQUISITIONCONTROLLER_H
#define DIGITIZERACQUISITIONCONTROLLER_H

#include <QObject>
#include "DigitizerConfiguration.h"
#include "QADQWrapper.h"
#include <QThread>
class DigitizerAcquisitionController : public QObject
{
    Q_OBJECT
public:
    DigitizerAcquisitionController(ADQInterfaceWrapper &digitizerWrapper, Acquisition &defaultAcquisition);
private:
    Acquisition &defaultAcquisition;
    ADQInterfaceWrapper &adq;

signals:
    void onStarted();
    void onStopped();
};

#endif // DIGITIZERACQUISITIONCONTROLLER_H
