#ifndef DIGITIZER_H
#define DIGITIZER_H
#include <string>
#include <QObject>
#include "DigitizerConfiguration.h"
#include "QADQWrapper.h"
#include "RecordProcessor.h"
#include <QThread>
#include "StreamingBuffers.h"
#include "BufferProcessor.h"
#include "AcquisitionThreads.h"
#include <QTimer>
#include "CalibrationTable.h"
class Digitizer : public QObject
{
    Q_OBJECT

public:
    enum DIGITIZER_STATE {
        READY,
        STOPPING,
        ACTIVE
    };
    enum DIGITIZER_TRIGGER_MODE {
        CONTINUOUS,
        SOFTWARE,
        LEVEL,
        INTERNAL,
        EXTERNAL,
        EXTERNAL2,
        EXTERNAL3
    };

    Q_ENUM(DIGITIZER_TRIGGER_MODE)
    Q_ENUM(DIGITIZER_STATE)
    Q_ENUM(CLOCK_SOURCES)
    Q_ENUM(TRIGGER_EDGES)

    Q_PROPERTY(DIGITIZER_STATE state READ getDigitizerState NOTIFY digitizerStateChanged)
    Q_PROPERTY(DIGITIZER_TRIGGER_MODE triggerMode READ getTriggerMode WRITE setTriggerMode NOTIFY triggerModeChanged)

    Q_PROPERTY(unsigned long duration READ getDuration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(unsigned long transferBufferSize READ getTransferBufferSize WRITE setTransferBufferSize NOTIFY transferBufferSizeChanged )
    Q_PROPERTY(unsigned long transferBufferCount READ getTransferBufferCount WRITE setTransferBufferCount NOTIFY transferBufferCountChanged )
    Q_PROPERTY(unsigned long transferBufferQueueSize READ getTransferBufferQueueSize WRITE setTransferBufferQueueSize NOTIFY transferBufferQueueSizeChanged)
    Q_PROPERTY(unsigned long long fileSizeLimit READ getFileSizeLimit NOTIFY fileSizeLimitChanged WRITE setFileSizeLimit)
    Q_PROPERTY(unsigned char userLogicBypass READ getUserLogicBypass NOTIFY userLogicBypassChanged WRITE setUserLogicBypass)
    Q_PROPERTY(CLOCK_SOURCES clockSource READ getClockSource NOTIFY clockSourceChanged WRITE setClockSource)
    Q_PROPERTY(TRIGGER_EDGES triggerEdge READ getTriggerEdge NOTIFY triggerEdgeChanged WRITE setTriggerEdge)
    Q_PROPERTY(int triggerLevel READ getTriggerLevel NOTIFY triggerLevelChanged WRITE setTriggerLevel)
    Q_PROPERTY(int triggerReset READ getTriggerReset NOTIFY triggerResetChanged WRITE setTriggerReset)
    Q_PROPERTY(unsigned char triggerMask READ getTriggerMask NOTIFY triggerMaskChanged WRITE setTriggerMask)
    Q_PROPERTY(unsigned short pretrigger READ getPretrigger NOTIFY pretriggerChanged WRITE setPretrigger)
    Q_PROPERTY(unsigned short triggerDelay READ getTriggerDelay NOTIFY triggerDelayChanged WRITE setTriggerDelay)
    Q_PROPERTY(unsigned long recordCount READ getRecordCount NOTIFY recordCountChanged WRITE setRecordCount)
    Q_PROPERTY(unsigned long recordLength READ getRecordLength NOTIFY recordLengthChanged WRITE setRecordLength)
    Q_PROPERTY(unsigned char channelMask READ getChannelMask NOTIFY channelMaskChanged WRITE setChannelMask)
    Q_PROPERTY(unsigned int sampleSkip READ getSampleSkip NOTIFY sampleSkipChanged WRITE setSampleSkip)
private:
    std::list<IRecordProcessor*> defaultRecordProcessors;
    std::list<IRecordProcessor*> &recordProcessors;
    ADQInterfaceWrapper &adq;
    Acquisition defaultAcquisition;
    DIGITIZER_STATE currentState = DIGITIZER_STATE::READY;
    DIGITIZER_TRIGGER_MODE currentTriggerMode = DIGITIZER_TRIGGER_MODE::CONTINUOUS;
    void changeDigitizerState(DIGITIZER_STATE newState);
    WriteBuffers writeBuffers;
    std::unique_ptr<IBufferProcessor> bufferProcessor;
    std::unique_ptr<LoopBufferProcessor> bufferProcessorHandler;
    std::unique_ptr<DMAChecker> dmaChecker;
    QThread bufferProcessingThread;
    QThread ADQThread;
    QTimer acquisitionTimer;
    CalibrationTable defaultCalibrationTable;
    bool isStreamFullyStopped();
    void joinThreads();
    bool configureAcquisition(Acquisition &acq, std::list<IRecordProcessor*> &recordProcessors, CalibrationTable &calibrations);
public slots:
    bool stopAcquisition();
    bool runAcquisition();
    void loopStopped();
public:
    Digitizer(ADQInterfaceWrapper &digitizerWrapper);
    ~Digitizer();
    bool runOverridenAcquisition(Acquisition &acq, std::list<IRecordProcessor*> &recordProcessors, CalibrationTable &calibration);
    bool setAcquisition(const Acquisition acq);
    void appendRecordProcessor(IRecordProcessor *rp);
    void removeRecordProcessor(IRecordProcessor *rp);


    DIGITIZER_STATE getDigitizerState();
    DIGITIZER_TRIGGER_MODE getTriggerMode();
    unsigned long getDuration();
    unsigned long getTransferBufferSize();
    unsigned long getTransferBufferCount();
    unsigned long getTransferBufferQueueSize();
    unsigned long long getFileSizeLimit();
    unsigned char getUserLogicBypass();
    CLOCK_SOURCES getClockSource();
    TRIGGER_EDGES getTriggerEdge();
    unsigned char getTriggerMask();
    int getTriggerLevel();
    int getTriggerReset();
    unsigned short getPretrigger();
    unsigned short getTriggerDelay();
    unsigned long getRecordCount();
    unsigned long getRecordLength();
    unsigned char getChannelMask();
    unsigned int getSampleSkip();
    INPUT_RANGES getInputRange(int ch);
    int getDCBias(int ch);
    int getDigitalGain(int ch);
    int getDigitalOffset(int ch, int ir);
    int getAnalogOffset(int ch, int ir);
    double getObtainedRange(int ch);
    std::string getAcquisitionTag();

    unsigned long getSamplesPerRecordComplete();

    unsigned long long getLastBuffersFill();
    unsigned long long getQueueFill();

    CalibrationTable getDefaultCalibrationTable() const;


    void setTriggerMode(DIGITIZER_TRIGGER_MODE triggerMode);
    void setDuration(unsigned long duration);
    void setTransferBufferSize(unsigned long bufferSize);
    void setTransferBufferCount(unsigned long bufferCount);
    void setTransferBufferQueueSize(unsigned long queueSize);
    void setFileSizeLimit(unsigned long long size);
    void setUserLogicBypass(unsigned char ulBypass);
    void setClockSource(CLOCK_SOURCES clockSource);
    void setTriggerEdge(TRIGGER_EDGES edge);
    void setTriggerMask(unsigned char mask);
    void setTriggerLevel(int lvl);
    void setTriggerReset(int rst);
    void setPretrigger(unsigned short pretrigger);
    void setTriggerDelay(unsigned short delay);
    void setRecordCount(unsigned long count);
    void setRecordLength(unsigned long length);
    void setChannelMask(unsigned char mask);
    void setSampleSkip(unsigned int sampleSkip);
    void setInputRange(int ch, INPUT_RANGES range);
    void setDCBias(int ch, int bias);
    void setDigitalGain(int ch, int gain);
    void setDigitalOffset(int ch, int ir, int offset);
    void setAnalogOffset(int ch, int ir, int offset);
    void setAcquisitionTag(std::string tag);
    Acquisition getAcquisition() const;

    void setDefaultCalibrationTable(const CalibrationTable &value);

signals:
    void acquisitionStarted();
    void digitizerStateChanged(DIGITIZER_STATE newState);
    void triggerModeChanged(DIGITIZER_TRIGGER_MODE newMode);
    void durationChanged(unsigned long duration);
    void transferBufferSizeChanged(unsigned long bufferSize);
    void transferBufferCountChanged(unsigned long bufferCount);
    void transferBufferQueueSizeChanged(unsigned long queueSize);
    void fileSizeLimitChanged(unsigned long long size);
    void userLogicBypassChanged(unsigned char ulBypass);
    void clockSourceChanged(CLOCK_SOURCES clockSource);
    void triggerEdgeChanged(TRIGGER_EDGES edge);
    void triggerLevelChanged(int lvl);
    void triggerResetChanged(int rst);
    void triggerMaskChanged(unsigned char mask);
    void pretriggerChanged(unsigned short pretrigger);
    void triggerDelayChanged(unsigned short delay);
    void recordCountChanged(unsigned long count);
    void recordLengthChanged(unsigned long length);
    void channelMaskChanged(unsigned char mask);
    void sampleSkipChanged(unsigned int sampleSkip);
    void inputRangeChanged(int ch, INPUT_RANGES range);
    void dcBiasChanged(int ch, int bias);
    void digitalGainChanged(int ch, int gain);
    void digitalOffsetChanged(int ch, int offset);
    void analogOffsetChanged(int ch, int offset);
    void acquisitionTagChanged(std::string tag);
};

#endif // DIGITIZER_H
