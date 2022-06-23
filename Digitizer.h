#ifndef DIGITIZER_H
#define DIGITIZER_H
#include <string>
#include <chrono>
#include <QObject>
#include "AcquisitionConfiguration.h"
#include "AcquisitionHandler.h"
#include "ConfigurationController.h"
#include "SpectroscopeController.h"
#include "RecordProcessor.h"

class Digitizer : public QObject
{
    Q_OBJECT

private:
    ADQInterface *adq;
    std::unique_ptr<AcquisitionHandler> acquisitionHandler;
    std::vector<IRecordProcessor *> recordProcessors;
    QConfigurationController cfg_;
    SpectroscopeController spectroscope_;
public:
    Digitizer(ADQInterface *adq);
    ~Digitizer();
    QConfigurationController &cfg() {return cfg_;};
    SpectroscopeController &spectroscope() {return spectroscope_;};
    void appendRecordProcessor(IRecordProcessor *rp);
    void removeRecordProcessor(IRecordProcessor *rp);
    bool startCustomAcquisition(AcquisitionConfiguration * config, std::vector<IRecordProcessor*> recordProcessors);
    bool startAcquisition();
    bool stopAcquisition();
    void SWTrig();

};

#endif // DIGITIZER_H
