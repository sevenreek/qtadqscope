#ifndef ACQUISITIONSTATECHANGELISTENER_H
#define ACQUISITIONSTATECHANGELISTENER_H
#include "DigitizerConstants.h"
class AcquisitionStateChangeListener {
public:
    virtual void onAcquisitionStateChanged(AcquisitionStates olds, AcquisitionStates news) = 0;
};
#endif