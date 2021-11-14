#ifndef QADQWRAPPER_H
#define QADQWRAPPER_H

#include <QObject>

#include "ADQAPIIncluder.h"
#include <memory>
#include "spdlog/spdlog.h"
Q_DECLARE_METATYPE(float*)

class ADQInterfaceWrapper {
protected:
    unsigned int deviceNumber;
    ADQInterface* adq;
    void* adqCU;
public:
    ADQInterfaceWrapper(void* adqCU, unsigned int devnum);
    virtual bool SetAdjustableBias(int channel, int code);
    virtual bool SetInputRange(int channel, float target, float *result);
    virtual bool SetLvlTrigEdge(int edge);
    virtual bool SetLvlTrigLevel(int level);
    virtual bool SetLvlTrigChannel(int channel);
    virtual bool SetPreTrigSamples(int pretrigger);
    virtual bool SetTrigLevelResetValue(int reset);
    virtual bool SetClockSource(unsigned int clockSource);
    virtual bool SetTriggerMode(unsigned int triggerMode);
    virtual bool SetSampleSkip(unsigned int sampleSkip);
    virtual bool BypassUserLogic(unsigned int ulTarget, unsigned int bypass);
    virtual bool SetGainAndOffset(unsigned int channel, int gain, int offset);
    virtual bool SetTransferBuffers(unsigned long count, unsigned long size);
    virtual bool SWTrig();
    virtual bool WriteUserRegister(unsigned int target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int *retval);
    virtual bool FlushDMA();
    virtual bool StartStreaming();
    virtual bool StopStreaming();
    virtual bool TriggeredStreamingSetup(
        unsigned int recordCount,
        unsigned int recordLength,
        unsigned int pretrigger,
        unsigned int triggerDelay,
        unsigned int channelMask
    );
    virtual bool ContinuousStreamingSetup(unsigned int channelMask);
    virtual bool GetStreamOverflow();
    virtual bool GetTransferBufferStatus(unsigned int *buffersFilled);
    virtual bool GetDataStreaming(
        void ** targetBuffers,
        void ** targetHeaders,
        unsigned char channelMask,
        unsigned int * samplesAdded,
        unsigned int * headersAdded,
        unsigned int * headerStatus
    );


    // Gen3 functions
    virtual int StartDataAcquisition(void);
    virtual int StopDataAcquisition(void);
    virtual int64_t WaitForRecordBuffer(int *channel, void **buffer, int timeout, struct ADQDataReadoutStatus *status);
    virtual int ReturnRecordBuffer(int channel, void *buffer);
    virtual int GetParameters(enum ADQParameterId id, void *const parameters);
    virtual int SetParameters(void *const parameters);
    virtual int InitializeParameters(enum ADQParameterId id, void *const parameters);
    virtual ~ADQInterfaceWrapper();
    virtual int SetChannelSampleSkip(unsigned int channel, unsigned int skipfactor);
    virtual unsigned int SetupLevelTrigger(int * level, int * edge, int * resetLevel, unsigned int channelMask, unsigned int individualMode);
};

class MutexADQWrapper : public ADQInterfaceWrapper
{
public:
    MutexADQWrapper(void* adqCU, unsigned int devnum);
private:
    std::mutex mutex;

    // ADQInterfaceWrapper interface
public:
    bool SetAdjustableBias(int channel, int code) override;
    bool SetInputRange(int channel, float target, float *result) override;
    bool SetLvlTrigEdge(int edge) override;
    bool SetLvlTrigLevel(int level) override;
    bool SetLvlTrigChannel(int channel) override;
    bool SetPreTrigSamples(int pretrigger) override;
    bool SetTrigLevelResetValue(int reset) override;
    bool SetClockSource(unsigned int clockSource) override;
    bool SetTriggerMode(unsigned int triggerMode) override;
    bool SetSampleSkip(unsigned int sampleSkip) override;
    bool BypassUserLogic(unsigned int ulTarget, unsigned int bypass) override;
    bool SetGainAndOffset(unsigned int channel, int gain, int offset) override;
    bool SetTransferBuffers(unsigned long count, unsigned long size) override;
    bool SWTrig() override;
    bool WriteUserRegister(unsigned int target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int *retval) override;
    bool FlushDMA() override;
    bool StartStreaming() override;
    bool StopStreaming() override;
    bool TriggeredStreamingSetup(unsigned int recordCount, unsigned int recordLength, unsigned int pretrigger, unsigned int triggerDelay, unsigned int channelMask) override;
    bool ContinuousStreamingSetup(unsigned int channelMask) override;
    bool GetStreamOverflow() override;
    bool GetTransferBufferStatus(unsigned int *buffersFilled) override;
    bool GetDataStreaming(void **targetBuffers, void **targetHeaders, unsigned char channelMask, unsigned int *samplesAdded, unsigned int *headersAdded, unsigned int *headerStatus) override;

    // Gen3 functions
    int StartDataAcquisition(void) override;
    int StopDataAcquisition(void) override;
    int64_t WaitForRecordBuffer(int *channel, void **buffer, int timeout, struct ADQDataReadoutStatus *status) override;
    int ReturnRecordBuffer(int channel, void *buffer) override;
    int GetParameters(enum ADQParameterId id, void *const parameters) override;
    int SetParameters(void *const parameters) override;
    int InitializeParameters(enum ADQParameterId id, void *const parameters) override;
    int SetChannelSampleSkip(unsigned int channel, unsigned int skipfactor) override;
    unsigned int SetupLevelTrigger(int * level, int * edge, int * resetLevel, unsigned int channelMask, unsigned int individualMode) override;
};
class QADQWrapper : public QObject
{
    Q_OBJECT
    bool streamActive = false;
    Qt::ConnectionType threadSafeConnectionMode = Qt::QueuedConnection;
    std::shared_ptr<ADQInterface> adq;
public:
    QADQWrapper(std::shared_ptr<ADQInterface> adq);
public slots:
    void setAdjustableBias(int channel, int code);
    void setInputRange(int channel, float target, float *result);
    void setLvlTrigEdge(int edge);
    void setLvlTrigLevel(int level);
    void setTrigLevelResetValue(int reset);
    void SWTrig();
    void writeUserRegister(unsigned int target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int *retval);
    void flushDMA();
    void startStreaming();
    void stopStreaming();
    void changeStreamState(bool streamActive);
signals:
    void streamStateChanged(bool streamActive);
};

#endif // QADQWRAPPER_H
