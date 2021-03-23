#include "ScopeUpdater.h"
#include "spdlog/spdlog.h"
ScopeUpdater::ScopeUpdater(unsigned long long sampleCount, QCustomPlot &plot): plot(plot)
{
    this->plot.addGraph();
    this->reallocate(sampleCount);
}
void ScopeUpdater::reallocate(unsigned long long sampleCount)
{
    this->x.clear();
    this->y.clear();
    this->x.resize(sampleCount);
    this->y.resize(sampleCount);
    this->plot.graph(0)->setData(x,y);
}
bool ScopeUpdater::processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount)
{
    return true; // TODO: REMOVE LATER

    spdlog::debug("Updating scope with {}. Vector size {}:{}", sampleCount, this->x.size(),this->y.size());
    for(unsigned long s = 0; s < sampleCount; s++)
    {
        this->y[s] = (double)(buffer[s]);
    }
    //this->plot.rescaleAxes();
    this->plot.replot();
    spdlog::debug("Replot ok");
    return true;
}
unsigned long long ScopeUpdater::finish()
{
    return 0;
}
