#include "SpectroscopeController.h"
#include "RegisterConstants.h"
using namespace Spectroscopy;
void SpectroscopeController::setConfig(AcquisitionConfiguration *config) {
  unsigned int retval;
  // WRITE DC SHIFT
  int dcShift, unclippedDCShift;
  dcShift = config->getTotalDCShift(CHANNEL_SOURCE, unclippedDCShift);
  this->adq->WriteUserRegister(UL_TARGET, DC_SHIFT_REGISTER, 0, dcShift,
                               &retval);
  unsigned int binReductionShift = config->spectroscope.binCountReductionShift;
  this->adq->WriteUserRegister(
      UL_TARGET, PHA_WINDOW_LENGTH_AND_BINCOUNT_REGISTER, ~BINCOUNT_MASK,
      binReductionShift << BINCOUNT_SHIFT, &retval);
  unsigned int windowLength = config->spectroscope.windowLength;
  this->adq->WriteUserRegister(UL_TARGET,
                               PHA_WINDOW_LENGTH_AND_BINCOUNT_REGISTER,
                               ~WINDOWLENGTH_MASK, windowLength, &retval);
  bool transferOverDMA = config->spectroscope.transferOverDMA;
  if (transferOverDMA) {
    this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER,
                                 ~TRANSFER_SPECTRA_BIT, TRANSFER_SPECTRA_BIT,
                                 &retval);
  } else {
    this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER,
                                 ~TRANSFER_SPECTRA_BIT, 0,
                                 &retval);
  }
}
void SpectroscopeController::enable() {
  unsigned int retval;
  this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER,
                               ~ACTIVE_SPECTRUM_BIT, 1, &retval);
}
void SpectroscopeController::disable() {
  unsigned int retval;
  this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER,
                               ~ACTIVE_SPECTRUM_BIT, 0, &retval);
}
void SpectroscopeController::resetSpectrum() {
  unsigned int retval;
  this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER,
                               ~RESET_SPECTRUM_BIT, RESET_SPECTRUM_BIT, &retval);
  this->adq->WriteUserRegister(UL_TARGET, PHA_CONTROL_REGISTER,
                               ~RESET_SPECTRUM_BIT, 0, &retval);
}
