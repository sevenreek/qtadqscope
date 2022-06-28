#include "SpectroscopeController.h"
#include "RegisterConstants.h"
#include <thread>
using namespace Spectroscopy;
void SpectroscopeController::setConfig(AcquisitionConfiguration* config)
{
    unsigned int retval;
    // WRITE DC SHIFT
    int dcShift, unclippedDCShift;
    dcShift = config->getTotalDCShift(CHANNEL_SOURCE, unclippedDCShift);
    this->adq->WriteUserRegister(UL_TARGET, DC_SHIFT_REGISTER, 0, dcShift, &retval);

    // WRITE BINCOUNT REDUCTION
    unsigned int binReductionShift = config->spectroscope.binCountReductionShift();
    this->adq->WriteUserRegister(UL_TARGET, PHA_WINDOW_LENGTH_AND_BINCOUNT_REGISTER, ~BINCOUNT_MASK,
                                 binReductionShift << BINCOUNT_SHIFT, &retval);

    // WRITE WINDOW LENGTH
    unsigned int windowLength = config->spectroscope.windowLength();
    this->adq->WriteUserRegister(UL_TARGET, PHA_WINDOW_LENGTH_AND_BINCOUNT_REGISTER,
                                 ~WINDOWLENGTH_MASK, windowLength, &retval);

    // WRITE DMA TRANSFER OF SPECTRAS
    bool transferOverDMA = config->spectroscope.transferOverDMA();
    if (transferOverDMA)
    {
        this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~TRANSFER_SPECTRA_BIT,
                                     TRANSFER_SPECTRA_BIT, &retval);
    }
    else
    {
        this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~TRANSFER_SPECTRA_BIT, 0,
                                     &retval);
    }
}
void SpectroscopeController::enable()
{
    unsigned int retval;
    this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~ACTIVE_SPECTRUM_BIT, 1, &retval);
}
void SpectroscopeController::disable()
{
    unsigned int retval;
    this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~ACTIVE_SPECTRUM_BIT, 0, &retval);
}
void SpectroscopeController::resetSpectrum()
{
    unsigned int retval;
    this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~RESET_SPECTRUM_BIT,
                                 RESET_SPECTRUM_BIT, &retval);
    // maybe sleep for a single ms or get a oneshot timer if you want to be perfect
    this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER, ~RESET_SPECTRUM_BIT, 0, &retval);
}
std::vector<unsigned int> SpectroscopeController::downloadSpectrum(unsigned int binCount)
{
    std::vector<uint32_t> data;
    data.reserve(binCount);
    this->adq->ReadBlockUserRegister(UL_TARGET, FIRST_REGISTER, data.data(),
                                     binCount * sizeof(uint32_t), READ_USER_REGISTER_LIKE_RAM);
    return data;
}
