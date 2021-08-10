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
    void startNewAcquisition(Acquisition& config);
    ScopeUpdater(unsigned long long sampleCount);
    bool processRecord(ADQRecordHeader* header, short* buffer, unsigned long sampleCount, int channel);
    unsigned long long finish();
    void reallocate(unsigned long long sampleCount);
    const char* getName();
signals:
    void onScopeUpdate(QVector<double> &x, QVector<double> y);
};

#endif // SCOPEUPDATER_H
