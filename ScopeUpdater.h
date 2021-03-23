#ifndef SCOPEUPDATER_H
#define SCOPEUPDATER_H
#include "RecordProcessor.h"
#include "qcustomplot.h"

class ScopeUpdater : public RecordProcessor
{
private:
    QCustomPlot &plot;
    QVector<double> x, y;
public:
    ScopeUpdater(unsigned long long sampleCount, QCustomPlot &plot);
    bool processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount);
    unsigned long long finish();
    void reallocate(unsigned long long sampleCount);
};

#endif // SCOPEUPDATER_H
