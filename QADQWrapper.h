#ifndef QADQWRAPPER_H
#define QADQWRAPPER_H

#include <QObject>
#ifdef MOCK_ADQAPI
    #include "MockADQAPI.h"
#else
    #include "ADQAPI.h"
#endif
#include <memory>
#include <QThread>
#include "spdlog/spdlog.h"
Q_DECLARE_METATYPE(float*)

class QADQWrapper : public QObject
{
    Q_OBJECT
private:
    std::shared_ptr<ADQInterface> adq;
    bool streamActive = false;
    Qt::ConnectionType threadSafeConnectionMode = Qt::QueuedConnection;
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
