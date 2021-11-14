#ifndef SCOPEUPDATER_H
#define SCOPEUPDATER_H
#include "RecordProcessor.h"
#include "qcustomplot.h"
#include <QObject>

class ScopeUpdater : public QObject, public IRecordProcessor
{
    Q_OBJECT
private:
    QVector<double> x, y;
    unsigned long long sampleCount;
public:
    bool startNewAcquisition(Acquisition& config) override;
    ScopeUpdater(unsigned long long sampleCount);
    STATUS processRecord(ADQRecord* record, size_t bufferSize) override;
    unsigned long long finish() override;
    void reallocate(unsigned long long sampleCount);
    const char* getName() override;
    unsigned long long getProcessedBytes() override;
signals:
    void onScopeUpdate(QVector<double> &x, QVector<double> y);
};

#endif // SCOPEUPDATER_H
