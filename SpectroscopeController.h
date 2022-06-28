#ifndef SPECTROSCOPECONTROLLER_H
#define SPECTROSCOPECONTROLLER_H
#include "ADQAPIIncluder.h"
#include "AcquisitionConfiguration.h"
#include "RegisterConstants.h"
class SpectroscopeController {
private:
    ADQInterface *adq;
public:
    void loadRegisters();
    void setConfig(AcquisitionConfiguration * config);
    void enable();
    void disable();
    void resetSpectrum();
    std::vector<unsigned int> downloadSpectrum(unsigned int binCount);

};


#endif