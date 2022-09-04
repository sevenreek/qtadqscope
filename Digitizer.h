#ifndef DIGITIZER_H
#define DIGITIZER_H
#include <ratio>
#include <string>
#include <chrono>
#include <QObject>
#include "AcquisitionConfiguration.h"
#include "AcquisitionHandler.h"
#include "ConfigurationController.h"
#include "DigitizerConstants.h"
#include "QAcquisitionHandler.h"
#include "SpectroscopeController.h"
#include "RecordProcessor.h"

class Digitizer : public QObject
{
    Q_OBJECT

private:
    ADQInterface *adq;
    std::unique_ptr<QAcquisitionHandlerGen3> acquisitionHandler;
    std::vector<IRecordProcessor *> recordProcessors;
    SpectroscopeController spectroscope_;
public:
    Digitizer(ADQInterface *adq);
    SpectroscopeController &spectroscope() {return spectroscope_;};
    void appendRecordProcessor(IRecordProcessor *rp);
    void removeRecordProcessor(IRecordProcessor *rp);
    bool startCustomAcquisition(AcquisitionConfiguration *config, std::vector<IRecordProcessor*> *recordProcessors);
    bool startAcquisition();
    bool stopAcquisition();
    float ramFill();
    float dmaUsage();
    AcquisitionStates state();
    std::chrono::milliseconds durationRemaining();
    void SWTrig();
signals:
    void acquisitionStateChanged(AcquisitionStates oldState, AcquisitionStates newState);
};

#endif // DIGITIZER_H
