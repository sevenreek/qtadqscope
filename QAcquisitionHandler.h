#ifndef QACQUISITIONHANDLER_H
#define QACQUISITIONHANDLER_H
#include "AcquisitionConfiguration.h"
#include "AcquisitionHandler.h"
#include "BufferProcessor.h"
#include "DigitizerConstants.h"
#include "RecordProcessor.h"
#include <QObject>
#include <QThread>
#include <QTimer>


class QBufferProcessorProxy : public QObject {
  Q_OBJECT
public:
  QBufferProcessorProxy(BufferProcessor &processor) : processor(processor) {}

private:
  BufferProcessor &processor;
public slots:
  void startLoop();
  void stop();
signals:
  void stateChanged(AcquisitionStates oldState, AcquisitionStates newState);
};

class QAcquisitionHandlerGen3 : public QObject, public AcquisitionHandler {
  Q_OBJECT
public:
  QAcquisitionHandlerGen3(QObject *parent = nullptr);
  ~QAcquisitionHandlerGen3();
  bool requestStart(AcquisitionConfiguration *config,
                    std::vector<IRecordProcessor *> *recordProcessors) override;
  bool requestStop() override;
  float ramFill() override;
  float dmaUsage() override;
  AcquisitionStates state() const override;
signals:
  void requestProcessorProxyStart();
  void requestProcessorProxyStop();
  void ramFillChanged(float ramFill);
  void stateChanged(AcquisitionStates oldState, AcquisitionStates newState);

private:
  struct TriggerConfiguration {
    int levelArray[MAX_NOF_CHANNELS];
    int resetArray[MAX_NOF_CHANNELS];
    int edgeArray[MAX_NOF_CHANNELS];
  };
  TriggerConfiguration trigSetupHelperConfig;
  bool configure(AcquisitionConfiguration *config);
  const std::vector<IRecordProcessor *> *recordProcessors = nullptr;
  const AcquisitionConfiguration *config = nullptr;
  ADQInterface *adq;
  QTimer acquisitionStabilizationTimer;
  QTimer acquisitionTimer;
  std::unique_ptr<QBufferProcessorProxy> processorProxy;
  std::unique_ptr<BufferProcessor> bufferProcessor;
  QThread processingThread;
  void finishRecordProcessors();
  void delayAcquisitionStart();
  void processorStopped();

private slots:
  void startAcquisition();
  void stopAcquisition();
  void onProcessorStateChanged(AcquisitionStates oldState, AcquisitionStates newState);
};

#endif