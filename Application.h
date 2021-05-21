#ifndef APPLICATION_H
#define APPLICATION_H
#include "MainWindow.h"
#include "ADQAPI.h"
#include "Acquisition.h"
#include <memory>
#include "ApplicationConfiguration.h"
#include "ScopeUpdater.h"
#include "BinaryFileWriter.h"
#include <QTimer>
#include "BuffersDialog.h"
#include "RegisterDialog.h"
class Application : public QObject
{
    Q_OBJECT
private:
    // Reference to the UI
    MainWindow& mainWindow;
    // Control unit created by the appropriate ADQAPI function.
    void* adqControlUnit;
    ApplicationConfiguration config;
    ADQInterface* adqDevice;
    std::unique_ptr<QTimer> updateTimer;
    // Instance of object responsible for updating the plot in real time.
    std::shared_ptr<ScopeUpdater> scopeUpdater;
    // Currently used instance of file writer object that stores the incoming data.
    std::shared_ptr<FileWriter> fileWriter;
    // Pointer to the helper object Acquisition.
    std::unique_ptr<Acquisition> acquisition;
    std::unique_ptr<BuffersDialog> buffersConfigurationDialog;
    std::unique_ptr<RegisterDialog> registerDialog;
    /*
     * Connects all UI signals to appropriate slots in Application and Acquisition.
     */
    void linkSignals();
    void createPeriodicUpdateTimer(unsigned long period);
    /*
     * Sets the UI elements that are used to control acquisition parameters
     * to the values specified in current channel configuration.
     */
    void setUI();
public:
    /*
     * Create an Application object before entering Qt's main loop
     * and call the start function.
     */
    explicit Application(MainWindow& mainWindow);
    /*
     * Populates the application object. Most importantly creates the helper
     * Acquisition object, which creates the DMA and processing threads.
     * Call after creating an Application object.
     */
    int start(int argc, char *argv[]);


public slots:
    // UI SLOTS
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
    void changeLevelTriggerReset(int val);

    void changeUpdateScope(int state);
    void changeAnalyse(int state);
    void changeSaveToFile(int state);
    void changeFiletype(int state);
    void primaryButtonPressed();
    void acquisitionStateChanged(ACQUISITION_STATES newState);
    void changeTimedRunEnabled(int state);
    void changeTimedRunValue(int val);

    /*
     * Connect to this signal using QueuedConnection from the ScopeUpdater unless
     * it ScopeUpdater is running on the same thread as the UI
     */
    void updateScope(QVector<double> &x, QVector<double> y);

    void changeDMABufferCount(unsigned long size);
    void changeBufferQueueCount(unsigned long size);

    void updatePeriodicUIElements();

    void configureDMABuffers();
    void onDMADialogClosed();
    void configureULRegisters();
    void onRegisterDialogClosed();

    void triggerSoftwareTrig();
    void loadConfig();
    void saveConfig();
};

int mvToADCCode(float inputRange, float dcBiasFloat);
float ADCCodeToMV(float inputRange, int code);
#endif // APPLICATION_H

