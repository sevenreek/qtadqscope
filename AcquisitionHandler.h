#ifndef ACQUISITIONAHNDLER_H
#define ACQUISITIONAHNDLER_H
#include "AcquisitionConfiguration.h"
#include "RecordProcessor.h"
class AcquisitionHandler 
{
public:
    /**
     * @brief Requests the acquisition in config to be configured and started.
     * @param config - specify the acqusitionm configuration to start
     * @param recordProcessors - specify the list of record processors to call as records arrive
     * @return true if the request was acknolwedged, the acquisition configured 
                and the acquisition will start after a stabilization delay
     * @return false if the request could not be processed, or configuration failed
     */
    virtual bool requestStart(AcquisitionConfiguration* config, std::vector<IRecordProcessor*> *recordProcessors) = 0;
    /**
     * @brief Request the acquisition to stop. Because the implementation is most likely 
     * to rely on multiprocessing, the acqusitiion will stop at a later time with a 
     * call to onStateChanged(AcquistionStates::INACTIVE) following.
     * @return true if the request is acknowledged and can be processed.
     * @return false if the request was ingored for any reason
     */
    virtual bool requestStop() = 0;
    /**
     * @brief Returns the current state of acquisition and the data collection thread.
     * @return AcquisitionStates 
     */
    virtual AcquisitionStates state() const = 0;
    /**
     * @brief Returns the percentage usage of the digitizer's internal RAM.
     * Values range from 0 to 1.
     * @return float 
     */
    virtual float ramFill() = 0;  
    /**
     * @brief Returns the average, arbitrary usage of the DMA buffers.
     * Values range from 0 to 1, with 1 being the DMA transfers being unable to go any faster.
     * @return float 
     */
    virtual float dmaUsage() = 0;
};

#endif