#ifndef APPLICATION_H
#define APPLICATION_H
#include "MainWindow.h"
#include "ADQAPI.h"
#include "Acquisition.h"
#include <memory>
#include "ApplicationConfiguration.h"
#include "ScopeUpdater.h"

class Application : public QObject
{
    Q_OBJECT
private:
    MainWindow& mainWindow;
    void* adqControlUnit;
    ApplicationConfiguration config;
    ADQInterface* adqDevice;
    std::shared_ptr<ScopeUpdater> scopeUpdater;
    std::shared_ptr<RecordProcessor> fileWriter;
    std::unique_ptr<Acquisition> acquisition;
    bool streamActive = false;
    void linkSignals();
    void setUI();
public:
    Application(MainWindow& mainWindow);
    int start(int argc, char *argv[]);

public slots:
    void changeChannel(int channel);
    void changeSampleSkip(int sampleSkip);
    void changeUL1Bypass(int state);
    void changeUL2Bypass(int state);
    void changeAnalogOffset(int val);
    void changeAnalogOffsetCode(int val);
    void changeInputRange(int index);
    void changeDigitalOffset(int val);
    void changeDigitalGain(int val);
    void changeTriggerMode(int index);
    void changeLimitRecords(int state);
    void changeRecordCount(int val);
    void changePretrigger(int val);
    void changeRecordLength(int val);
    void changeTriggerDelay(int val);
    void changeLevelTriggerEdge(int index);
    void changeLevelTriggerCode(int val);
    void changeLevelTriggerMV(double val);

    void changeUpdateScope(int state);
    void changeAnalyse(int state);
    void changeSaveToFile(int state);
    void changeFiletype(int state);
    void primaryButtonPressed();
    void acquisitionStateChanged(ACQUISITION_STATES newState);
    void updateScope(QVector<double> &x, QVector<double> y);

    void changeDMABufferCount(unsigned long size);
    void changeBufferQueueCount(unsigned long size);
};

int mvToADCCode(float inputRange, float dcBiasFloat);
float ADCCodeToMV(float inputRange, int code);
#endif // APPLICATION_H

