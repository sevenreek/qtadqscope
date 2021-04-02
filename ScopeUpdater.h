#ifndef SCOPEUPDATER_H
#define SCOPEUPDATER_H
#include "RecordProcessor.h"
#include "qcustomplot.h"
#include <QObject>

class ScopeUpdater : public QObject, public RecordProcessor
{
    Q_OBJECT
private:
    QCustomPlot &plot;
    QVector<double> x, y;
public:
    void startNewStream(ApplicationConfiguration& config);
    ScopeUpdater(unsigned long long sampleCount, QCustomPlot &plot);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    unsigned long long finish();
    void reallocate(unsigned long long sampleCount);
    const char* getName();
signals:
    void onScopeUpdate(QVector<double> &x, QVector<double> y);
};

#endif // SCOPEUPDATER_H
