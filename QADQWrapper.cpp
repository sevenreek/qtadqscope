#include "QADQWrapper.h"

QADQWrapper::QADQWrapper(std::shared_ptr<ADQInterface> adq):
adq(adq)
{
    qRegisterMetaType<float*>("float*");
}

void QADQWrapper::setAdjustableBias(int channel, int code)
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "setAdjustableBias", threadSafeConnectionMode,
                                  Q_ARG(int,channel), Q_ARG(int,code));
        return;
    }
    this->adq->SetAdjustableBias(channel, code);
}

void QADQWrapper::setInputRange(int channel, float target, float *result)
{
    /*if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "setInputRange", threadSafeConnectionMode,
                                  Q_ARG(int,channel), Q_ARG(float,target), Q_ARG(float*, result));
        return;
    }*/

    this->adq->SetInputRange(channel, target, result);
    spdlog::debug("SetInputRange target:{} result{}", target, *result);
}

void QADQWrapper::setLvlTrigEdge(int edge)
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "setLvlTrigEdge", threadSafeConnectionMode,
                                  Q_ARG(int,edge));
        return;
    }
    this->adq->SetLvlTrigEdge(edge);
}

void QADQWrapper::setLvlTrigLevel(int level)
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "setLvlTrigLevel", threadSafeConnectionMode,
                                  Q_ARG(int,level));
        return;
    }
    this->adq->SetLvlTrigLevel(level);
}

void QADQWrapper::setTrigLevelResetValue(int reset)
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "setTrigLevelResetValue", threadSafeConnectionMode,
                                  Q_ARG(int,reset));
        return;
    }
    this->adq->SetTrigLevelResetValue(reset);
}

void QADQWrapper::SWTrig()
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "SWTrig", threadSafeConnectionMode);
        return;
    }
    this->adq->SWTrig();
}

void QADQWrapper::writeUserRegister(unsigned int target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int *retval)
{
    spdlog::critical("Write user register not implemented!");
}

void QADQWrapper::flushDMA()
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "flushDMA", threadSafeConnectionMode);
        return;
    }
    this->adq->FlushDMA();
}

void QADQWrapper::startStreaming()
{
    if (QThread::currentThread() != thread()) {
        spdlog::debug("Scheduled start in QADQWrapper");
        QMetaObject::invokeMethod(this, "startStreaming", threadSafeConnectionMode);
        return;
    }
    spdlog::debug("Executing start in QADQWrapper");
    this->changeStreamState(true);
    this->adq->StartStreaming();
}

void QADQWrapper::stopStreaming()
{
    if (QThread::currentThread() != thread()) {
        spdlog::debug("Scheduling stop in QADQWrapper");
        QMetaObject::invokeMethod(this, "stopStreaming", threadSafeConnectionMode);
        spdlog::debug("Scheduled stop in QADQWrapper");
        return;
    }
    spdlog::debug("Executing stop in QADQWrapper");
    this->adq->StopStreaming();
    this->changeStreamState(false);
}

void QADQWrapper::changeStreamState(bool streamActive)
{
    this->streamActive = streamActive;
    emit this->streamStateChanged(streamActive);
}
