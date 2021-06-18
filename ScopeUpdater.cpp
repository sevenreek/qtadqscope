#include "ScopeUpdater.h"
#include "spdlog/spdlog.h"
ScopeUpdater::ScopeUpdater(unsigned long long sampleCount, QCustomPlot &plot): plot(plot)
{
    this->sampleCount = sampleCount;
    this->plot.addGraph();
    this->plot.setInteraction(QCP::iRangeDrag, true);
    this->plot.setInteraction(QCP::iRangeZoom, true);
    this->reallocate(sampleCount);
    this->arrow = new QCPItemLine(&plot);
    this->arrow->setPen(QPen(Qt::red));
}
void ScopeUpdater::reallocate(unsigned long long sampleCount)
{
    this->sampleCount = sampleCount;
    this->x.clear();
    this->y.clear();
    this->x.resize(sampleCount);
    for(unsigned long s = 0; s < sampleCount; s++)
    {
        this->x[s] = (s);
    }
    this->y.resize(sampleCount);
    this->plot.graph(0)->setData(x,y);
    this->plot.replot();
}
bool ScopeUpdater::processRecord(StreamingHeader_t* header, short* buffer, unsigned long sampleCount, int channel)
{
    //spdlog::debug("Updating scope with {}. Vector size {}:{}", sampleCount, this->x.size(),this->y.size());
    for(unsigned long s = 0; s < sampleCount; s++)
    {
        this->y[s] = (double)(buffer[s]);
    }
    emit this->onScopeUpdate(x, y); // scope must update from the GUI thread
    return true;
}
void ScopeUpdater::startNewStream(ApplicationConfiguration& config)
{
    if(config.getCurrentChannelConfig().isContinuousStreaming) // continuous
    {
        this->reallocate(config.transferBufferSize/sizeof(short));
    }
    else
    {
        this->reallocate(config.getCurrentChannelConfig().recordLength);
    }
}
unsigned long long ScopeUpdater::finish()
{
    return 0;
}
const char* ScopeUpdater::getName()
{
    return "ScopeUpdater";
}


void ScopeUpdater::changePlotTriggerLine(short pos, unsigned long sampleCount)
{
    if(sampleCount == 0)
        sampleCount = this->sampleCount;
    this->arrow->start->setCoords(0, pos);
    this->arrow->end->setCoords(sampleCount-1, pos);
    this->plot.replot();
}

void ScopeUpdater::changePlotTriggerLine(ChannelConfiguration& channelConfig)
{
    this->changePlotTriggerLine(channelConfig.getDCBiasedTriggerValue(), channelConfig.recordLength);
}
