/*
 * (C)opyright 2008-2015 Signal Processing Devices Sweden AB
 */
#define LINUX
#ifndef ADQAPI_REVISION_H
#define ADQAPI_REVISION_H
#define ADQAPI_REVISION 24448
#endif

#ifndef ADQAPI
#define ADQAPI
#define VALIDATE_DLL(p) if(!IS_VALID_DLL(p)) exit(-1);
#define IS_VALID_DLL(p) ((p->ValidateDll() == 0x11AABEEF) && (ADQAPI_GetRevision() == ADQAPI_REVISION))
#define IS_VALID_DLL_REVISION(r) (r == ADQAPI_REVISION)
#define ADQControlUnit_UserLogMessage(ADQ_CU, A, B, ...) ADQControlUnit_UserLogMessageAtLine(ADQ_CU, A, __FILE__, __FUNCTION__, __LINE__, B, ##__VA_ARGS__)

#ifdef LINUX
#include <sys/types.h>
#define OS_SETTING_NOWINDOWS
#define DLL_EXPORT
#define DLL_IMPORT
#define UINT8 u_int8_t
#define UINT32 u_int32_t
#else
#include "windows.h"
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
#ifndef USE_CPP_API
#define USE_CPP_API
#endif
#endif

enum ADQProductID_Enum {
  PID_ADQ214 = 0x0001,
  PID_ADQ114 = 0x0003,
  PID_ADQ112 = 0x0005,
  PID_SphinxHS = 0x000B,
  PID_SphinxLS = 0x000C,
  PID_ADQ108 = 0x000E,
  PID_ADQDSP = 0x000F,
  PID_SphinxAA14 = 0x0011,
  PID_SphinxAA16 = 0x0012,
  PID_ADQ412 = 0x0014,
  PID_ADQ212 = 0x0015,
  PID_SphinxAA_LS2 = 0x0016,
  PID_SphinxHS_LS2 = 0x0017,
  PID_SDR14 = 0x001B,
  PID_ADQ1600 = 0x001C,
  PID_SphinxXT = 0x001D,
  PID_ADQ208 = 0x001E,
  PID_DSU = 0x001F,
  PID_ADQ14 = 0x0020,
  PID_SDR14RF = 0x0021,
  PID_TX320 = 0x201A,
  PID_RX320 = 0x201C,
  PID_S6000 = 0x2019
};

enum ADQHWIFEnum {
  HWIF_USB,
  HWIF_PCIE,
  HWIF_USB3,
  HWIF_PCIELITE
};

struct ADQInfoListEntry
{
  enum ADQHWIFEnum HWIFType;
  enum ADQProductID_Enum ProductID;
  unsigned int VendorID;
  unsigned int AddressField1;
  unsigned int AddressField2;
  char DevFile[64];
  unsigned int DeviceInterfaceOpened;
  unsigned int DeviceSetupCompleted;
};

struct ADQInfoListPreAlloArray
{
  struct ADQInfoListEntry ADQlistArray[40];    //Preallocate for 40 devices
};

#ifdef USE_CPP_API

struct ADQInterface
{



  virtual void* GetPtrData(unsigned int channel) = 0;

  virtual int SetDataFormat(unsigned int format) = 0;

  virtual int GetADQType() = 0;

  virtual unsigned int GetNofChannels() = 0;
  virtual unsigned int GetNofFPGAs() = 0;
  virtual unsigned int GetDRAMPhysEndAddr(unsigned int* DRAM_MAX_END_ADDRESS) = 0;


  virtual int* GetPtrDataChA() = 0;
  virtual int* GetPtrDataChB() = 0;

  virtual int SetLvlTrigChannel(int channel) = 0;
  virtual int GetLvlTrigChannel() = 0;
  virtual int GetTriggedCh() = 0;
  virtual int SetAlgoNyquistBand(unsigned int band) = 0;
  virtual int SetAlgoStatus(int status) = 0;
  virtual unsigned int SetSampleSkip(unsigned int DecimationFactor) = 0;
  virtual unsigned int GetSampleSkip() = 0;
  virtual unsigned int MultiRecordGetRecord(int RecordNumber) = 0;
  virtual unsigned int CollectRecord(int RecordNumber) = 0;
  virtual unsigned int SetSampleDecimation(unsigned int SampleDecimation) = 0;
  virtual unsigned int GetSampleDecimation() = 0;
  virtual int SetAfeSwitch(unsigned int afe) = 0;
  virtual unsigned int GetAfeSwitch(unsigned char Channel, unsigned char* afemode) = 0;
  virtual unsigned int SetGainAndOffset(unsigned char Channel, int Gain, int Offset) = 0;
  virtual unsigned int GetGainAndOffset(unsigned char Channel, int* Gain, int* Offset) = 0;
  virtual unsigned int SetInterleavingMode(char interleaving) = 0;
  virtual unsigned int SetInterleavingIPEstimationMode(unsigned char IPInstanceAddr, unsigned int updatetype) = 0;
  virtual unsigned int GetInterleavingIPEstimationMode(unsigned char IPInstanceAddr, unsigned int* updatetype) = 0;
  virtual unsigned int SetInterleavingIPBypassMode(unsigned char IPInstanceAddr, unsigned int bypassflag) = 0;
  virtual unsigned int GetInterleavingIPBypassMode(unsigned char IPInstanceAddr, unsigned int* bypassflag) = 0;
  virtual unsigned int SetInterleavingIPCalibration(unsigned char IPInstanceAddr, unsigned int* calibration) = 0;
  virtual unsigned int GetInterleavingIPCalibration(unsigned char IPInstanceAddr, unsigned int* calibration) = 0;
  virtual unsigned int ResetInterleavingIP(unsigned char IPInstanceAddr) = 0;
  virtual const char* GetCardOption() = 0;
  virtual unsigned int ReadADCCalibration(unsigned char ADCNo, unsigned short* calibration) = 0;
  virtual unsigned int WriteADCCalibration(unsigned char ADCNo, unsigned short* calibration) = 0;
  virtual unsigned int EnableClockRefOut(unsigned int enable) = 0;
  virtual unsigned int GetPPTStatus() = 0;
  virtual unsigned int InitPPT() = 0;
  virtual unsigned int SetPPTActive(unsigned int active) = 0;
  virtual unsigned int SetPPTInitOffset(unsigned int init_offset) = 0;
  virtual unsigned int SetPPTPeriod(unsigned int period) = 0;
  virtual unsigned int SetPPTBurstMode(unsigned int burst_mode) = 0;
  virtual unsigned int GetComFlashEnableBit() = 0;
  virtual unsigned int RebootCOMFPGAFromSecondaryImage(unsigned int PCIeAddress, unsigned int PromAddress) = 0;
  virtual unsigned int RebootALGFPGAFromPrimaryImage() = 0;
  virtual unsigned int ReBootADQFromFlash(unsigned int partition) = 0;
  virtual unsigned int ProcessorFlashControl(unsigned char cmd, unsigned int data) = 0;
  virtual unsigned int ProcessorFlashControlData(unsigned int* data, unsigned int len) = 0;
  virtual unsigned int SetInternalTriggerFrequency(unsigned int Int_Trig_Freq) = 0;
  virtual unsigned int TriggeredStreamingOneChannelSetup(unsigned int SamplePerRecord, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int ArmMode, unsigned int ReadOutSpeed, unsigned int Channel) = 0;
  virtual unsigned int TriggeredStreamingSetupV5(unsigned int SamplePerRecord, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int TriggeredStreamingFlags) = 0;
  virtual unsigned int TriggeredStreamingArmV5() = 0;
  virtual unsigned int TriggeredStreamingGetStatusV5(unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle) = 0;
  virtual unsigned int TriggeredStreamingShutdownV5() = 0;

  virtual unsigned int TriggeredStreamingGetWaveformV5(short* waveform_data_short) = 0;
  virtual unsigned int TriggeredStreamingDisarmV5() = 0;
  virtual unsigned int TriggeredStreamingParseDataStream(unsigned int samples_per_record, int* data_stream, int** data_target) = 0;
  virtual unsigned int WaveformAveragingSetup(unsigned int NofWaveforms, unsigned int NofSamples,
      unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples,
      unsigned int WaveformAveragingFlags) = 0;
  virtual unsigned int WaveformAveragingArm() = 0;
  virtual unsigned int WaveformAveragingDisarm() = 0;
  virtual unsigned int WaveformAveragingStartReadout() = 0;
  virtual unsigned int WaveformAveragingGetWaveform(int* waveform_data) = 0;
  virtual unsigned int WaveformAveragingGetStatus(unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle) = 0;
  virtual unsigned int WaveformAveragingShutdown() = 0;
  virtual unsigned int WaveformAveragingParseDataStream(unsigned int samples_per_record, int* data_stream, int** data_target) = 0;
  virtual unsigned int WaveformAveragingSoftwareTrigger() = 0;
  virtual int ResetOverheat() = 0;
  virtual int GetDSPData() = 0;
  virtual int GetDSPDataNowait() = 0;
  virtual unsigned int PacketStreamingSetup(unsigned int PacketSizeSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples) = 0;
  virtual unsigned int PacketStreamingArm() = 0;
  virtual unsigned int PacketStreamingDisarm() = 0;
  virtual int InitTransfer() = 0;
  virtual int SetSendLength(unsigned int length) = 0;
  virtual unsigned int GetSendLength() = 0;
  virtual int WaitForPCIeDMAFinish(unsigned int length) = 0;
  virtual unsigned long GetPhysicalAddress() = 0;
  virtual unsigned int SetDACOffsetVoltage(unsigned char channel, float v) = 0;
  virtual unsigned int AWGReset(unsigned int dacId) = 0;
  virtual unsigned int AWGmalloc(unsigned int dacId, unsigned int LengthSeg1, unsigned int LengthSeg2, unsigned int LengthSeg3, unsigned int LengthSeg4) = 0;
  virtual unsigned int AWGSegmentMalloc(unsigned int dacId, unsigned int segId, unsigned int length, unsigned char reallocate) = 0;
  virtual unsigned int AWGWriteSegment(unsigned int dacId, unsigned int segId, unsigned int enable, unsigned int NofLaps, unsigned int length, int *data) = 0;
  virtual unsigned int AWGWriteSegments(unsigned int dacId, unsigned int NofSegs, unsigned int* segId, unsigned int* NofLaps, unsigned int* length, short int** data) = 0;
  virtual unsigned int AWGWritePlaylist(unsigned int dacId, unsigned int NofPlaylistElements, unsigned int* index, unsigned int* write_mask,
                                        unsigned int* segId, unsigned int* NofLaps, unsigned int* nextIndex,
                                        unsigned int* triggerType, unsigned int* triggerLength, unsigned int* triggerPolarity, unsigned int* triggerSample,
                                        unsigned int* triggerULSignals) = 0;
  virtual unsigned int AWGWritePlaylistItem(unsigned int dacId, unsigned int index, unsigned int write_mask,
      unsigned int segId, unsigned int NofLaps, unsigned int nextIndex,
      unsigned int triggerType, unsigned int triggerLength, unsigned int triggerPolarity, unsigned int triggerSample,
      unsigned int triggerULSignals) = 0;
  virtual unsigned int AWGPlaylistMode(unsigned int dacId, unsigned int mode) = 0;
  virtual unsigned int AWGEnableSegments(unsigned int dacId, unsigned int enableSeg) = 0;
  virtual unsigned int AWGAutoRearm(unsigned int dacId, unsigned int enable) = 0;
  virtual unsigned int AWGContinuous(unsigned int dacId, unsigned int enable) = 0;
  virtual unsigned int AWGTrigMode(unsigned int dacId, unsigned int trigmode) = 0;
  virtual unsigned int AWGArm(unsigned int dacId) = 0;
  virtual unsigned int AWGDisarm(unsigned int dacId) = 0;
  virtual unsigned int AWGTrig(unsigned int dacId) = 0;
  virtual unsigned int SendIPCommand(unsigned char IPInstanceAddr, unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int* answer) = 0;
  virtual unsigned int OffsetDACSpiWrite(unsigned char channel, unsigned int data) = 0;
  virtual unsigned int DACSpiWrite(unsigned char channel, const unsigned char address, const unsigned char data) = 0;
  virtual unsigned int DACSpiRead(unsigned char channel, unsigned char address, unsigned char *data) = 0;
  virtual int TrigOutEn(unsigned int en) = 0;
  virtual int ADCCalibrate() = 0;
  virtual unsigned int WriteSTARBDelay(unsigned int starbdelay, unsigned int writetoeeprom) = 0;
  virtual unsigned int EnablePXIeTriggers(unsigned int port, unsigned int bitflags) = 0;
  virtual unsigned int EnablePXIeTrigout(unsigned int port, unsigned int bitflags) = 0;
  virtual unsigned int PXIeSoftwareTrigger() = 0;
  virtual unsigned int SetPXIeTrigDirection(unsigned int trig0output, unsigned int trig1output) = 0;
  virtual unsigned int AWGSetupTrigout(unsigned int dacId, unsigned int trigoutmode, unsigned int pulselength, unsigned int enableflags, unsigned int autorearm) = 0;
  virtual unsigned int AWGTrigoutArm(unsigned int dacId) = 0;
  virtual unsigned int AWGTrigoutDisarm(unsigned int dacId) = 0;
  virtual unsigned int AWGSetTriggerEnable(unsigned int dacId, unsigned int bitflags) = 0;
  virtual unsigned int AWGSetInterpolationFilter(unsigned int dacId, unsigned char interpolation_filter) = 0;
  virtual unsigned int ResetRecorder(unsigned int inst) = 0;
  virtual unsigned int ResetFIFOPaths(unsigned int inst) = 0;
  virtual unsigned int RunRecorderSelfTest(unsigned int inst, unsigned int* inout_vector) = 0;
  virtual unsigned int GetRecorderStatus(unsigned int inst, unsigned int* status) = 0;
  virtual unsigned int GetRecorderDiskStatus(unsigned int inst, unsigned int diskno, unsigned int* status) = 0;
  virtual unsigned int BreakRecorderCommand(unsigned int inst) = 0;
  virtual unsigned int SendRecorderCommand(unsigned int inst, unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int* answer) = 0;
  virtual unsigned int WriteDataToDSU(unsigned int inst, unsigned int start_address, unsigned int nofbytes, unsigned char* data) = 0;
  virtual unsigned int ReadDataFromDSU(unsigned int inst, unsigned int start_address, unsigned int nofbytes, unsigned char* data) = 0;
  virtual unsigned int SetupDSUAcquisition(unsigned int inst, unsigned int start_address, unsigned int end_address) = 0;
  virtual unsigned int StartDSUAcquisition(unsigned int inst) = 0;
  virtual unsigned int GetNextDSURecordingAddress(unsigned int inst, unsigned int* next_address) = 0;
  virtual unsigned int GetPCIeLinkWidth() = 0;
  virtual unsigned int GetPCIeLinkRate() = 0;
  virtual unsigned int GetPCIeTLPSize() = 0;
  virtual unsigned int SetExtTrigThreshold(unsigned int trignum, double vthresh) = 0;
  virtual int Blink() = 0;
  virtual unsigned int GetInterleavingIPFrequencyCalibrationMode(unsigned char IPInstanceAddr, unsigned int* freqcalflag) = 0;
  virtual unsigned int SetInterleavingIPFrequencyCalibrationMode(unsigned char IPInstanceAddr, unsigned int freqcalmode) = 0;
  virtual unsigned int WriteUserRegister(unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval) = 0;
  virtual unsigned int ReadUserRegister(unsigned int ul_target, unsigned int regnum, unsigned int* retval) = 0;
  virtual unsigned int WriteBlockUserRegister(unsigned int ul_target, unsigned int start_addr, unsigned int *data, unsigned int num_bytes, unsigned int options) = 0;

  virtual unsigned int ReadBlockUserRegister(unsigned int ul_target, unsigned int start_addr, unsigned int *data, unsigned int num_bytes, unsigned int options) = 0;
  virtual unsigned int BypassUserLogic(unsigned int ul_target, unsigned int bypass) = 0;
  virtual unsigned int GetProductFamily(unsigned int* family) = 0;
  virtual unsigned int GetHardwareAssemblyPartNumber(char* partnum) = 0;
  virtual unsigned int GetHardwareSubassemblyPartNumber(char* partnum) = 0;
  virtual unsigned int GetPCBAssemblyPartNumber(char* partnum) = 0;
  virtual unsigned int GetPCBPartNumber(char* partnum) = 0;
  virtual unsigned int GetFPGApart(unsigned int fpganum, char* partstr) = 0;
  virtual unsigned int GetFPGATempGrade(unsigned int fpganum, char* tgrade) = 0;
  virtual unsigned int GetFPGASpeedGrade(unsigned int fpganum, unsigned int* sgrade) = 0;
  virtual unsigned int SetBiasDACPercentage(unsigned int channel, float percent) = 0;
  virtual unsigned int SetupLevelTrigger(int* level, int* edge, int* resetlevel, unsigned int channelsmask, unsigned int individual_mode) = 0;
  virtual unsigned int GetRxFifoOverflow() = 0;
  virtual unsigned int HasTriggeredStreamingFunctionality() = 0;
  virtual unsigned int TriggeredStreamingSetup(unsigned int NofRecords,
      unsigned int NofSamples,
      unsigned int NofPreTrigSamples,
      unsigned int NofHoldOffSamples,
      unsigned char ChannelsMask) = 0;
  virtual unsigned int SetTriggeredStreamingTotalNofRecords(unsigned int MaxNofRecordsTotal) = 0;
  virtual unsigned int GetTriggeredStreamingRecordSizeBytes() = 0;
  virtual unsigned int GetTriggeredStreamingHeaderSizeBytes() = 0;
  virtual unsigned int GetTriggeredStreamingRecords(unsigned int NofRecordsToRead, void** data_buf, void* header_buf, unsigned int* NofRecordsRead) = 0;
  virtual unsigned int TriggeredStreamingGetNofRecordsCompleted(unsigned int ChannelsMask,  unsigned int* NofRecordsCompleted) = 0;
  virtual unsigned int TriggeredStreamingGetStatus(unsigned int* InIdle, unsigned int* TriggerSkipped, unsigned int* Overflow) = 0;
  virtual unsigned int SetTriggeredStreamingHeaderRegister(char RegValue) = 0;
  virtual unsigned int SetTriggeredStreamingHeaderSerial(unsigned int SerialNumber) = 0;
  virtual unsigned int TriggeredStreamingArm() = 0;
  virtual unsigned int TriggeredStreamingDisarm() = 0;
  virtual unsigned int ParseTriggeredStreamingHeader(void* HeaderPtr, unsigned long long* Timestamp, unsigned int* Channel, unsigned int* ExtraAccuracy,
      int* RegisterValue, unsigned int* SerialNumber, unsigned int* RecordCounter) = 0;
  virtual unsigned int HasAdjustableInputRange() = 0;
  virtual unsigned int SetInputRange(unsigned int channel, float inputrangemVpp, float* result) = 0;
  virtual unsigned int SetAdjustableBias(unsigned int channel, int ADCcodes) = 0;
  virtual unsigned int WriteADQATTStateManual(unsigned int channel, unsigned int relay16, unsigned int relay8, unsigned int ptap8, unsigned int ptap4,
      unsigned int ptap2, unsigned int ptap1, unsigned int ptap05, unsigned int ptap025) = 0;
  virtual unsigned int GetAdjustableBias(unsigned int channel, int *ADCcodes) = 0;
  virtual unsigned int GetInputRange(unsigned int channel, float *InpRange) = 0;
  virtual unsigned int GetRecorderBytesPerAddress() = 0;
  virtual unsigned int SetFanControl(unsigned int fan_control) = 0;
  virtual int          ADCReg(unsigned char addr, unsigned char adc, unsigned int val) = 0;
  virtual unsigned int SetupDBS(unsigned char DBS_instance, unsigned int bypass, int dc_target, int lower_saturation_level, int upper_saturation_level) = 0;
  virtual unsigned int EnableWFATriggerCounter() = 0;
  virtual unsigned int DisableWFATriggerCounter() = 0;
  virtual unsigned int StartWFATriggerCounter() = 0;
  virtual unsigned int SetWFANumberOfTriggers(unsigned int number_of_triggers) = 0;
  virtual unsigned int MeasureInputOffset(unsigned int channel, int* value) = 0;
  virtual unsigned int SetOffsetCompensationDAC(unsigned int channel, unsigned int daccode) = 0;
  virtual unsigned int RunCalibrationADQ412DC(unsigned int calmode) = 0;
  virtual unsigned int ResetCalibrationStateADQ412DC() = 0;
  virtual unsigned int DisconnectInputs(unsigned int channelmask) = 0;
  virtual unsigned int RxSetFrequency(unsigned long long int frequency) = 0;
  virtual unsigned int RxSetRfAmplifier(unsigned char amplifier, unsigned char mode) = 0;
  virtual unsigned int RxSetRfPath(unsigned char mode) = 0;
  virtual unsigned int RxSetLoOut(unsigned char mode) = 0;
  virtual unsigned int RxSetRfAttenuation(unsigned char att_index, unsigned char att) = 0;
  virtual unsigned int RxSetRfFilter(unsigned char filter) = 0;
  virtual unsigned int RxSetLoFilter(unsigned char filter) = 0;
  virtual unsigned int RxSetIfGainDac(unsigned char channel, unsigned char dacValue) = 0;
  virtual unsigned int RxSetVcomDac(unsigned char channel, unsigned short dacValue) = 0;
  virtual unsigned int RxSetDcOffsetDac(unsigned char channel, unsigned short dacValue) = 0;
  virtual unsigned int RxSetLinearityDac(unsigned short dacValue) = 0;
  virtual unsigned int TxSetFrequency(unsigned long long int frequency) = 0;
  virtual unsigned int TxSetRfAmplifier(unsigned char amplifier, unsigned char mode) = 0;
  virtual unsigned int TxSetRfPath(unsigned char mode) = 0;
  virtual unsigned int TxSetLoOut(unsigned char mode) = 0;
  virtual unsigned int TxSetRfAttenuation(unsigned char att_index, unsigned char att) = 0;
  virtual unsigned int TxSetRfFilter(unsigned char filter) = 0;
  virtual unsigned int TxSetLoFilter(unsigned char filter) = 0;
  virtual unsigned int TxSetDcOffsetDac(unsigned char channel, unsigned short dacValue) = 0;
  virtual unsigned int TxSetLinearityDac(unsigned char channel, unsigned short dacValue) = 0;
  virtual unsigned int SynthSetDeviceStandby(unsigned char channel, unsigned char standbyStatus) = 0;
  virtual unsigned int SynthSetFrequency(unsigned char channel, unsigned long long int frequency) = 0;
  virtual unsigned int SynthSetPowerLevel(unsigned char channel, float powerLevel) = 0;
  virtual unsigned int SynthSetRfOutput(unsigned char channel, unsigned char mode) = 0;
  virtual unsigned int SynthSetAlcMode(unsigned char channel, unsigned char mode) = 0;
  virtual unsigned int SynthDisableAutoLevel(unsigned char channel, unsigned char mode) = 0;
  virtual unsigned int SynthSetClockReference(unsigned char lockExtEnable, unsigned char refOutEnable, unsigned char pxiClk) = 0;
  virtual unsigned int SynthSetReferenceDac(unsigned int dacValue) = 0;
  virtual unsigned int SynthSetAlcDac(unsigned char channel, unsigned int dacValue) = 0;
  virtual unsigned int SynthGetAlcDac(unsigned char channel, unsigned int* dacValue) = 0;
  virtual unsigned int GetDeviceStatus(unsigned int* status) = 0;

  virtual int GetData(void** target_buffers,
                      unsigned int target_buffer_size,
                      unsigned char target_bytes_per_sample,
                      unsigned int StartRecordNumber,
                      unsigned int NumberOfRecords,
                      unsigned char ChannelsMask,
                      unsigned int StartSample,
                      unsigned int nSamples,
                      unsigned char TransferMode) = 0;

  virtual int GetDataWH(void** target_buffers,
                        void* target_headers,
                        unsigned int target_buffer_size,
                        unsigned char target_bytes_per_sample,
                        unsigned int StartRecordNumber,
                        unsigned int NumberOfRecords,
                        unsigned char ChannelsMask,
                        unsigned int StartSample,
                        unsigned int nSamples,
                        unsigned char TransferMode) = 0;
  virtual int GetDataWHTS(void** target_buffers,
                          void* target_headers,
                          void* target_timestamps,
                          unsigned int target_buffer_size,
                          unsigned char target_bytes_per_sample,
                          unsigned int StartRecordNumber,
                          unsigned int NumberOfRecords,
                          unsigned char ChannelsMask,
                          unsigned int StartSample,
                          unsigned int nSamples,
                          unsigned char TransferMode) = 0;
  virtual int GetDataStreaming(void** target_buffers,
                               void** target_headers,
                               unsigned char channels_mask,
                               unsigned int* samples_added,
                               unsigned int* headers_added,
                               unsigned int* header_status) = 0;
  virtual unsigned int GetLastError() = 0;
  virtual void* GetPtrStream() = 0;
  virtual unsigned int GetErrorVector() = 0;
  virtual unsigned int IsAlive() = 0;
  virtual unsigned int ReadEEPROM(unsigned int addr) = 0;
  virtual unsigned int ReadEEPROMDB(unsigned int addr) = 0;
  virtual unsigned int ReadEEPROM(unsigned int addr, unsigned int i2c_addr) = 0;
  virtual unsigned int ResetDevice(int resetlevel) = 0;
  virtual unsigned int InvalidateCache() = 0;
  virtual int SetCacheSize(unsigned int newSizeInBytes) = 0;
  virtual int SetTransferBuffers(unsigned int nOfBuffers, unsigned int bufferSize) = 0;
  virtual unsigned int GetTransferBufferStatus(unsigned int* filled) = 0;
  virtual unsigned int FlushDMA() = 0;
  virtual unsigned int WriteEEPROM(unsigned int addr, unsigned int data, unsigned int accesscode) = 0;
  virtual unsigned int WriteEEPROMDB(unsigned int addr, unsigned int data, unsigned int accesscode) = 0;
  virtual unsigned int WriteEEPROM(unsigned int addr, unsigned int data, unsigned int accesscode, unsigned int i2c_addr) = 0;
  virtual int ParseEEPROMBlock(char* blockname, char* map_version, unsigned int buffer_len, unsigned char* buffer, unsigned int i2c_addr) = 0;
  virtual int SetDelayLineValues(int samplerate, unsigned int linear_interpolation) = 0;
  virtual int SetDelayLineValuesDirect(unsigned int delay1, unsigned int delay2) = 0;
  virtual int SetLvlTrigLevel(int level) = 0;
  virtual int SetLvlTrigEdge(int edge) = 0;
  virtual int SetClockSource(int source) = 0;
  virtual int SetClockFrequencyMode(int clockmode) = 0;
  virtual int SetPllFreqDivider(int divider) = 0;
  virtual int SetPll(int n_divider, int r_divider, int vco_divider, int channel_divider) = 0;
  virtual int SetTriggerMode(int trig_mode) = 0;
  virtual int SetAuxTriggerMode(int trig_mode) = 0;
  virtual int SetPreTrigSamples(unsigned int PreTrigSamples) = 0;
  virtual int SetTriggerHoldOffSamples(unsigned int TriggerHoldOffSamples) = 0;
  virtual int SetBufferSizePages(unsigned int pages) = 0;
  virtual int SetBufferSizeWords(unsigned int words) = 0;
  virtual int SetBufferSize(unsigned int samples) = 0;
  virtual int SetNofBits(unsigned int NofBits) = 0;

  virtual int SetSampleWidth(unsigned int SampleWidth) = 0;
  virtual int SetWordsPerPage(unsigned int WordsPerPage) = 0;
  virtual int SetPreTrigWords(unsigned int PreTrigWords) = 0;
  virtual int SetWordsAfterTrig(unsigned int WordsAfterTrig) = 0;
  virtual int SetTrigLevelResetValue(int OffsetValue) = 0;
  virtual int SetTrigMask1(unsigned int TrigMask) = 0;
  virtual int SetTrigLevel1(int TrigLevel) = 0;
  virtual int SetTrigPreLevel1(int TrigLevel) = 0;
  virtual int SetTrigCompareMask1(unsigned int TrigCompareMask) = 0;
  virtual int SetTrigMask2(unsigned int TrigMask) = 0;
  virtual int SetTrigLevel2(int TrigLevel) = 0;
  virtual int SetTrigPreLevel2(int TrigLevel) = 0;
  virtual int SetTrigCompareMask2(unsigned int TrigCompareMask) = 0;
  virtual unsigned int SetExternTrigEdge(unsigned int edge) = 0;
  virtual unsigned int GetExternTrigEdge(unsigned int* edge) = 0;
  virtual unsigned int SetExternalTriggerDelay(unsigned char delaycycles) = 0;
  virtual int ArmTrigger() = 0;
  virtual int DisarmTrigger() = 0;
  virtual int StartStreaming() = 0;
  virtual int StopStreaming() = 0;
  virtual int SWTrig() = 0;
  virtual int CollectDataNextPage() = 0;
  virtual int CollectDataNextPageWithPrefetch(unsigned int prefetch) = 0;
  virtual int GetWaitingForTrigger() = 0;
  virtual int GetAcquired() = 0;
  virtual int GetTrigged() = 0;
  virtual unsigned int GetAcquiredRecords() = 0;
  virtual unsigned int GetAcquiredRecordsAndLoopCounter(unsigned int* acquired_records, unsigned int* loop_counter) = 0;
  virtual unsigned int GetPageCount() = 0;
  virtual int GetLvlTrigLevel() = 0;
  virtual int GetLvlTrigFlank() = 0;
  virtual int GetLvlTrigEdge() = 0;
  virtual unsigned int GetOutputWidth() = 0;
  virtual int GetPllFreqDivider() = 0;
  virtual int GetClockSource() = 0;
  virtual int GetTriggerMode() = 0;
  virtual unsigned int GetBufferSizePages() = 0;
  virtual unsigned int GetBufferSize() = 0;
  virtual unsigned long long GetMaxBufferSize() = 0;
  virtual unsigned int GetMaxBufferSizePages() = 0;
  virtual unsigned int GetSamplesPerPage() = 0;
  virtual unsigned int GetUSBAddress() = 0;
  virtual unsigned long GetPCIeAddress() = 0;
  virtual unsigned int GetBcdDevice() = 0;
  virtual int SetStreamConfig(unsigned int option, unsigned int value) = 0;
  virtual int GetStreamConfig(unsigned int option, unsigned int *value) = 0;
  virtual int GetStreamStatus() = 0;
  virtual int SetStreamStatus(int status) = 0;
  virtual int GetStreamOverflow() = 0;
  virtual char* GetBoardSerialNumber() = 0;
  virtual int* GetRevision() = 0;
  virtual int GetTriggerInformation() = 0;
  virtual int GetTrigPoint() = 0;
  virtual unsigned int GetTrigType() = 0;
  virtual int GetOverflow() = 0;
  virtual unsigned int GetRecordSize() = 0;
  virtual unsigned int GetNofRecords() = 0;
  virtual int IsUSBDevice() = 0;
  virtual int IsUSB3Device() = 0;
  virtual int IsPCIeDevice() = 0;
  virtual int IsPCIeLiteDevice() = 0;
  virtual unsigned int SendProcessorCommand(int command, int argument) = 0;
  virtual unsigned int SendProcessorCommand(unsigned int command, unsigned int addr, unsigned int mask, unsigned int data) = 0;
  virtual unsigned int SendLongProcessorCommand(unsigned int command, unsigned int addr, unsigned int mask, unsigned int data) = 0;
  virtual unsigned int WriteRegister(unsigned int addr, unsigned int mask, unsigned int data) = 0;
  virtual unsigned int ReadRegister(unsigned int addr) = 0;
  virtual unsigned int WriteAlgoRegister(unsigned int addr, unsigned int mask, unsigned int data) = 0;
  virtual unsigned int ReadAlgoRegister(unsigned int addr) = 0;
  virtual unsigned int WriteI2C(unsigned int addr, unsigned int nbytes, unsigned int data) = 0;
  virtual unsigned int WriteDBI2C(unsigned int addr, unsigned int nbytes, unsigned int data) = 0;
  virtual unsigned int ReadI2C(unsigned int addr, unsigned int nbytes) = 0;
  virtual unsigned int ReadDBI2C(unsigned int addr, unsigned int nbytes) = 0;
  virtual unsigned int WriteReadI2C(unsigned int addr, unsigned int rbytes, unsigned int wbytes, unsigned int wrdata) = 0;
  virtual unsigned int GetTemperature(unsigned int addr) = 0;
  virtual unsigned int GetTemperatureFloat(unsigned int addr, float* temperature) = 0;
  virtual unsigned int MultiRecordSetup(unsigned int NumberOfRecords, unsigned int SamplesPerRecord) = 0;
  virtual unsigned int MultiRecordSetupGP(unsigned int NumberOfRecords, unsigned int SamplesPerRecord, unsigned int* mrinfo) = 0;
  virtual unsigned int GetMaxNofSamplesFromNofRecords(unsigned int NofRecords, unsigned int* MaxNofSamples) = 0;
  virtual unsigned int GetMaxNofRecordsFromNofSamples(unsigned int NofSamples, unsigned int* MaxNofRecords) = 0;
  virtual unsigned int GetDataMultiRecordSetup(unsigned int NumberOfRecords, unsigned int SamplesPerRecord) = 0;
  virtual unsigned int MultiRecordClose() = 0;
  virtual unsigned int GetAcquiredAll() = 0;
  virtual unsigned int GetTriggedAll() = 0;
  virtual unsigned int IsStartedOK() = 0;
  virtual unsigned int MemoryDump(unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize) = 0;
  virtual unsigned int MemoryDumpRecords(unsigned int StartRecord, unsigned int NofRecords, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize) = 0;
  virtual unsigned int MemoryShadow(void* MemoryArea, unsigned int ByteSize) = 0;
  virtual unsigned int GetDataFormat() = 0;
  virtual unsigned int SetTestPatternMode(int mode) = 0;
  virtual unsigned int SetTestPatternConstant(int value) = 0;
  virtual int SetDirectionTrig(int direction) = 0;
  virtual int WriteTrig(int data) = 0;
  virtual unsigned int SetConfigurationTrig(unsigned int mode, unsigned int pulselength, unsigned int invertoutput) = 0;
  virtual unsigned int EnableInternalTriggerCounts() = 0;
  virtual unsigned int DisableInternalTriggerCounts() = 0;
  virtual unsigned int ClearInternalTriggerCounts() = 0;
  virtual unsigned int SetInternalTriggerCounts(unsigned int trigger_counts) = 0;

  virtual unsigned int ConfigureDebugCounter(unsigned int direction, unsigned int output_mode, unsigned int counter_mode, int initial_value) = 0;
  virtual int SetDirectionGPIO(unsigned int direction, unsigned int mask) = 0;
  virtual int WriteGPIO(unsigned int data, unsigned int mask) = 0;
  virtual unsigned int ReadGPIO() = 0;
  virtual unsigned int* GetMultiRecordHeader() = 0;
  virtual unsigned long long GetTrigTime() = 0;
  virtual unsigned long long GetTrigTimeCycles() = 0;
  virtual unsigned int GetTrigTimeSyncs() = 0;
  virtual unsigned int GetTrigTimeStart() = 0;
  virtual int SetTrigTimeMode(int TrigTimeMode) = 0;
  virtual int ResetTrigTimer(int TrigTimeRestart) = 0;
  virtual unsigned int ra(const char* regname) = 0;
  virtual unsigned int RegisterNameLookup(const char* regname, unsigned int* address, unsigned int allow_assertion) = 0;
  virtual int SpiSend(unsigned char addr, const char* data,
                      unsigned char length, unsigned int negedge, unsigned int* ret) = 0;
  virtual unsigned int GetExternalClockReferenceStatus(unsigned int *extrefstatus) = 0;
  virtual unsigned int SetTransferTimeout(unsigned int value) = 0;
  virtual unsigned int StorePCIeConfig(UINT32* pci_space) = 0;
  virtual unsigned int ReloadPCIeConfig(UINT32* pci_space) = 0;
  virtual const char*  GetADQDSPOption() = 0;
  virtual unsigned int SetEthernetPllFreq(unsigned char eth10_freq, unsigned char eth1_freq) = 0;
  virtual unsigned int SetPointToPointPllFreq(unsigned char pp_freq) = 0;
  virtual unsigned int SetEthernetPll(unsigned short refdiv, unsigned char useref2, unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv, unsigned char eth10_outdiv, unsigned char eth1_outdiv) = 0;
  virtual unsigned int SetPointToPointPll(unsigned short refdiv, unsigned char useref2,  unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv, unsigned char pp_outdiv, unsigned char ppsync_outdiv) = 0;
  virtual unsigned int SetDirectionMLVDS(unsigned char direction) = 0;
  virtual const char*  GetNGCPartNumber() = 0;
  virtual const char*  GetUserLogicPartNumber() = 0;
  virtual unsigned int GetProductVariant(unsigned int* ProductVariant) = 0;
  virtual unsigned int SetInternalTriggerPeriod(unsigned int TriggerPeriodClockCycles) = 0;
  virtual unsigned int FX2ReadRequest(unsigned int requestcode, unsigned int value, unsigned int index, long len, char *buf) = 0;
  virtual unsigned int FX2WriteRequest(unsigned int requestcode, unsigned int value, unsigned int index, long len, char *data) = 0;
  virtual unsigned int GetProductID() = 0;
  virtual unsigned int BootAdqFromFlash(unsigned int addr) = 0;
  virtual unsigned int IsBootloader() = 0;
  virtual unsigned int TrigoutEnable(unsigned int bitflags) = 0;
  virtual unsigned int HasTrigHardware(unsigned int trignum) = 0;
  virtual unsigned int HasTrigoutHardware(unsigned int trignum) = 0;
  virtual unsigned int HasVariableTrigThreshold(unsigned int trignum) = 0;
  virtual unsigned int WriteToDataEP(unsigned int *pData, unsigned int length) = 0;
  virtual unsigned int SendDataDev2Dev(unsigned long PhysicalAddress, unsigned int channel, unsigned int options) = 0;
  virtual unsigned int GetP2pStatus(unsigned int* pending, unsigned int channel) = 0;
  virtual unsigned int SetP2pSize(unsigned int bytes, unsigned int channel) = 0;
  virtual unsigned int GetP2pSize(unsigned int channel) = 0;
  virtual unsigned int HasAdjustableBias() = 0;
  virtual unsigned int GetUSB3Config(unsigned int option, unsigned int* value) = 0;
  virtual unsigned int SetUSB3Config(unsigned int option, unsigned int value) = 0;
  virtual unsigned int GetInputImpedance(unsigned int channel, float* impedance) = 0;
  virtual char* GetBoardProductName() = 0;
  virtual unsigned int GetNofBytesPerSample(unsigned int* bytes_per_sample) = 0;
  virtual unsigned int GetNofDBSInstances(unsigned int* nof_dbs_instances) = 0;
  virtual unsigned int SetCalibrationModeADQ412DC(unsigned int calibmode) = 0;

  virtual unsigned int SetFPGARebootMethod(unsigned int RebootMethod) = 0;

  virtual unsigned int GetNofRecorderIP(unsigned int* answer) = 0;

  virtual unsigned int SetOvervoltageProtection(unsigned int enabled) = 0;
  virtual unsigned int SetADCClockDelay(unsigned int adcnum, float delayval) = 0;
  virtual unsigned int SetAttenuators(unsigned int channel, unsigned int attmask) = 0;
  virtual int ATDGetWFAStatus(unsigned int *status) = 0;
  virtual int ATDSetupWFAAdvanced(unsigned int segment_length, unsigned int segments_per_record, unsigned int records_per_dram, unsigned int record_length, unsigned int nof_accumulations, unsigned int accumulator_offset, unsigned int acc_raw_ratio, unsigned int bypass) = 0;
  virtual int ATDSetupWFA(unsigned int record_length, unsigned int nof_accumulations) = 0;
  virtual int ATDSetupThreshold(unsigned int channel_id, unsigned int hysteresis, unsigned int threshold, unsigned int baseline, unsigned int negative_pulse_enable, unsigned int bypass) = 0;
  virtual int ATDSetThresholdFilter(unsigned int channel_id, unsigned int* coefficients) = 0;
  virtual int GetDNA(unsigned int* dna) = 0;
  virtual int ResetDNA(unsigned int assert) = 0;
  virtual int PDSetupStreaming(unsigned char channels_mask) = 0;
  virtual int PDSetupLevelTrig(unsigned int channel, int trigger_level, int reset_hysteresis, int trigger_arm_hysteresis, int reset_arm_hysteresis, unsigned int trigger_polarity, unsigned int reset_polarity) = 0;
  virtual int PDEnableLevelTrig(unsigned int enable) = 0;
  virtual int PDGetLevelTrigStatus(unsigned int* status) = 0;
  virtual int PDSetupTiming(unsigned int channel, unsigned int nof_pretrigger_samples, unsigned int nof_moving_average_samples, unsigned int moving_average_delay, unsigned int trailing_edge_window, unsigned int number_of_records, unsigned int record_variable_length) = 0;
  virtual int PDSetupTriggerCoincidence(unsigned int channel, unsigned int window_length, unsigned int mask) = 0;
  virtual int PDEnableTriggerCoincidence(unsigned int enable) = 0;
  virtual int PDGetEventCounters(unsigned int* lt_tevent_counter, unsigned int* lt_revent_counter, unsigned int* coin_tevent_counter, unsigned int* coin_revent_counter, unsigned int* pt_tevent_counter, unsigned int* pt_revent_counter, unsigned int* acq_tevent_counter, unsigned int* acq_revent_counter, unsigned int* acq_revent_pt_counter) = 0;
  virtual int PDAutoTrig(unsigned int channel, int* detected_trigger_level, unsigned int* detected_arm_hystersis) = 0;
  virtual int HasFeature(char *featurename) = 0;
  virtual int InitializeStreamingTransfer() = 0;

  virtual int SetSWTrigValue(float samples) = 0;
  
  virtual unsigned int SetDACPercentage(unsigned int spi_addr, unsigned int output_num, float percent) = 0;

  virtual unsigned int MultiRecordSetChannelMask(unsigned int channelmask) = 0;


  virtual unsigned int USBReConnect() = 0;

  virtual int OCTDebug(unsigned int arg1, unsigned int arg2, unsigned int arg3) = 0;
  virtual int OCTResetOVP() = 0;
  virtual int OCTSetOVPLevel(unsigned int level) = 0;
  virtual int OCTGetOVPLevel(unsigned int* level) = 0;
  virtual int OCTGetOVPStatus(unsigned int* neg_ov, unsigned int* pos_ov) = 0;
  virtual int OCTSetTriggerCount(unsigned int count) = 0;
  virtual int OCTSetFFTLength(unsigned int length) = 0;
  virtual int OCTSetFFTEnable(unsigned int enable) = 0;
  virtual int OCTSetFFTEnableWin(unsigned int enable) = 0;
  virtual int OCTSetFFTHoldOff(unsigned int holdoff) = 0;
  virtual int EnableGPIOSupplyOutput(unsigned int enable) = 0;

  virtual int USBLinkupTest(unsigned int retries) = 0;

  
  virtual int SetClockInputImpedance(unsigned int input_num, unsigned int mode) = 0;
  
  virtual int SetTriggerInputImpedance(unsigned int input_num, unsigned int mode) = 0;
  
  virtual int MeasureSupplyVoltage(unsigned int sensor_num, float* value) = 0;

  virtual unsigned int ValidateDll() = 0; //MUST BE LAST FUNCTION !!!


};

#endif


#ifndef OS_SETTING_NOWINDOWS
#include "windows.h"
#endif

#define ADQ_CLOCK_INT_INTREF    0
#define ADQ_CLOCK_INT_EXTREF    1
#define ADQ_CLOCK_EXT        2
#define ADQ_CLOCK_INT_PXIREF    3

#define ADQ_TRANSFER_MODE_NORMAL        0x00
#define ADQ_TRANSFER_MODE_HEADER_ONLY   0x01
#define ADQ_ALL_CHANNELS_MASK           0xff
#define ADQ_GETDATA_MAX_NOF_CHANNELS    8

#define ADQ_SW_TRIGGER_MODE      1
#define ADQ_EXT_TRIGGER_MODE    2
#define ADQ_LEVEL_TRIGGER_MODE    3
#define ADQ_INTERNAL_TRIGGER_MODE   4

#define ADQ_LEVEL_TRIGGER_USE_DEFAULT_RESETLEVEL 0xffffffff
#define ADQ_LEVEL_TRIGGER_ALL_CHANNELS           0xf

#define ADQ_D2DFLAG_CONST_ADDR 0x1u

#define LOG_LEVEL_ERROR                                         0x1
#define LOG_LEVEL_WARN                                          0x2
#define LOG_LEVEL_INFO                                          0x3

#ifdef __cplusplus
extern "C"
{
#endif

DLL_IMPORT void* CreateADQControlUnit();
#ifndef OS_SETTING_NOWINDOWS
DLL_IMPORT void* CreateADQControlUnitWN(HANDLE ReceiverWnd);
#endif
DLL_IMPORT void DeleteADQControlUnit(void* adq_cu_ptr);
DLL_IMPORT int ADQAPI_GetRevision();
DLL_IMPORT int ADQControlUnit_FindDevices(void* adq_cu_ptr);
DLL_IMPORT unsigned int ADQControlUnit_ListDevices(void* adq_cu_ptr, struct ADQInfoListEntry** retList, unsigned int* retLen);
DLL_IMPORT unsigned int ADQControlUnit_OpenDeviceInterface(void* adq_cu_ptr, int ADQInfoListEntryNumber);
DLL_IMPORT unsigned int ADQControlUnit_SetupDevice(void* adq_cu_ptr, int ADQInfoListEntryNumber);
DLL_IMPORT unsigned int ADQControlUnit_CloseDevice(void* adq_cu_ptr, int ADQInfoListEntryNumber);
DLL_IMPORT int ADQControlUnit_GetFailedDeviceCount(void* adq_cu_ptr);
DLL_IMPORT unsigned int ADQControlUnit_GetLastFailedDeviceError(void* adq_cu_ptr);
DLL_IMPORT unsigned int ADQControlUnit_ClearLastFailedDeviceError(void* adq_cu_ptr);
DLL_IMPORT unsigned int ADQControlUnit_EnableErrorTrace(void* adq_cu_ptr, unsigned int trace_level, const char* trace_file_dir);
DLL_IMPORT unsigned int ADQControlUnit_EnableErrorTraceAppend(void* adq_cu_ptr, unsigned int trace_level, const char* trace_file_dir);
DLL_IMPORT unsigned int ADQControlUnit_UserLogMessageAtLine(void* adq_cu_ptr,  unsigned int trace_level, const char* loc_file, const char* loc_func, const int loc_line, const char* message, ...);

DLL_IMPORT unsigned int ADQControlUnit_GetDevicesInfoList(void* adq_cu_ptr, struct ADQInfoListPreAlloArray* InfoList, unsigned int* retLen);
DLL_IMPORT int ADQControlUnit_NofADQ(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQDSP(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofDSU(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQ112(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQ114(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQ214(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQ212(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQ108(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQ412(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofSDR14(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQ1600(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofSphinxAA(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQ208(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofEXT(void* adq_cu_ptr);
DLL_IMPORT int ADQControlUnit_NofADQ14(void* adq_cu_ptr);

#ifdef USE_CPP_API
DLL_IMPORT ADQInterface* ADQControlUnit_GetADQ(void* adq_cu_ptr, int adq_num);
#endif
DLL_IMPORT void ADQControlUnit_DeleteADQ(void* adq_cu_ptr, int ADQ_num);
DLL_IMPORT void ADQControlUnit_DeleteADQDSP(void* adq_cu_ptr, int ADQDSP_num);
DLL_IMPORT void ADQControlUnit_DeleteDSU(void* adq_cu_ptr, int ADQDSP_num);
DLL_IMPORT void ADQControlUnit_DeleteADQ112(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT void ADQControlUnit_DeleteADQ114(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT void ADQControlUnit_DeleteADQ214(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT void ADQControlUnit_DeleteADQ212(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT void ADQControlUnit_DeleteADQ108(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT void ADQControlUnit_DeleteADQ412(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT void ADQControlUnit_DeleteSDR14(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT void ADQControlUnit_DeleteADQ1600(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT void ADQControlUnit_DeleteADQ208(void* adq_cu_ptr, int adq208_num);

#define ETH10_FREQ_156_25MHZ 1
#define ETH10_FREQ_125MHZ 0

#define ETH1_FREQ_156_25MHZ 1
#define ETH1_FREQ_125MHZ 0

#define PP_FREQ_330MHZ 3
#define PP_FREQ_250MHZ 2
#define PP_FREQ_156_25MHZ 1
#define PP_FREQ_125MHZ 0

#ifdef __cplusplus
}
#endif


#ifndef LINUX
#include <stdio.h>
#include <Windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <string.h>
#include <stdlib.h>
#include <tchar.h>
#endif

#define INCLUDE_ADQ
#ifdef __cplusplus
extern "C" {
#endif
DLL_IMPORT  void* ADQ_GetPtrData(void* adq_cu_ptr, int adq_num , unsigned int channel);
DLL_IMPORT  int ADQ_SetDataFormat(void* adq_cu_ptr, int adq_num , unsigned int format);
DLL_IMPORT  int ADQ_GetADQType(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetNofChannels(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetNofFPGAs(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetDRAMPhysEndAddr(void* adq_cu_ptr, int adq_num , unsigned int* DRAM_MAX_END_ADDRESS);
DLL_IMPORT  int* ADQ_GetPtrDataChA(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int* ADQ_GetPtrDataChB(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SetLvlTrigChannel(void* adq_cu_ptr, int adq_num , int channel);
DLL_IMPORT  int ADQ_GetLvlTrigChannel(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetTriggedCh(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SetAlgoNyquistBand(void* adq_cu_ptr, int adq_num , unsigned int band);
DLL_IMPORT  int ADQ_SetAlgoStatus(void* adq_cu_ptr, int adq_num , int status);
DLL_IMPORT  unsigned int ADQ_SetSampleSkip(void* adq_cu_ptr, int adq_num , unsigned int DecimationFactor);
DLL_IMPORT  unsigned int ADQ_GetSampleSkip(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_MultiRecordGetRecord(void* adq_cu_ptr, int adq_num , int RecordNumber);
DLL_IMPORT  unsigned int ADQ_CollectRecord(void* adq_cu_ptr, int adq_num , int RecordNumber);
DLL_IMPORT  unsigned int ADQ_SetSampleDecimation(void* adq_cu_ptr, int adq_num , unsigned int SampleDecimation);
DLL_IMPORT  unsigned int ADQ_GetSampleDecimation(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SetAfeSwitch(void* adq_cu_ptr, int adq_num , unsigned int afe);
DLL_IMPORT  unsigned int ADQ_GetAfeSwitch(void* adq_cu_ptr, int adq_num , unsigned char Channel, unsigned char* afemode);
DLL_IMPORT  unsigned int ADQ_SetGainAndOffset(void* adq_cu_ptr, int adq_num , unsigned char Channel, int Gain, int Offset);
DLL_IMPORT  unsigned int ADQ_GetGainAndOffset(void* adq_cu_ptr, int adq_num , unsigned char Channel, int* Gain, int* Offset);
DLL_IMPORT  unsigned int ADQ_SetInterleavingMode(void* adq_cu_ptr, int adq_num , char interleaving);
DLL_IMPORT  unsigned int ADQ_SetInterleavingIPEstimationMode(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr, unsigned int updatetype);
DLL_IMPORT  unsigned int ADQ_GetInterleavingIPEstimationMode(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr, unsigned int* updatetype);
DLL_IMPORT  unsigned int ADQ_SetInterleavingIPBypassMode(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr, unsigned int bypassflag);
DLL_IMPORT  unsigned int ADQ_GetInterleavingIPBypassMode(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr, unsigned int* bypassflag);
DLL_IMPORT  unsigned int ADQ_SetInterleavingIPCalibration(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr, unsigned int* calibration);
DLL_IMPORT  unsigned int ADQ_GetInterleavingIPCalibration(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr, unsigned int* calibration);
DLL_IMPORT  unsigned int ADQ_ResetInterleavingIP(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr);
DLL_IMPORT  const char* ADQ_GetCardOption(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_ReadADCCalibration(void* adq_cu_ptr, int adq_num , unsigned char ADCNo, unsigned short* calibration);
DLL_IMPORT  unsigned int ADQ_WriteADCCalibration(void* adq_cu_ptr, int adq_num , unsigned char ADCNo, unsigned short* calibration);
DLL_IMPORT  unsigned int ADQ_EnableClockRefOut(void* adq_cu_ptr, int adq_num , unsigned int enable);
DLL_IMPORT  unsigned int ADQ_GetPPTStatus(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_InitPPT(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetPPTActive(void* adq_cu_ptr, int adq_num , unsigned int active);
DLL_IMPORT  unsigned int ADQ_SetPPTInitOffset(void* adq_cu_ptr, int adq_num , unsigned int init_offset);
DLL_IMPORT  unsigned int ADQ_SetPPTPeriod(void* adq_cu_ptr, int adq_num , unsigned int period);
DLL_IMPORT  unsigned int ADQ_SetPPTBurstMode(void* adq_cu_ptr, int adq_num , unsigned int burst_mode);
DLL_IMPORT  unsigned int ADQ_GetComFlashEnableBit(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_RebootCOMFPGAFromSecondaryImage(void* adq_cu_ptr, int adq_num , unsigned int PCIeAddress, unsigned int PromAddress);
DLL_IMPORT  unsigned int ADQ_RebootALGFPGAFromPrimaryImage(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_ReBootADQFromFlash(void* adq_cu_ptr, int adq_num , unsigned int partition);
DLL_IMPORT  unsigned int ADQ_ProcessorFlashControl(void* adq_cu_ptr, int adq_num , unsigned char cmd, unsigned int data);
DLL_IMPORT  unsigned int ADQ_ProcessorFlashControlData(void* adq_cu_ptr, int adq_num , unsigned int* data, unsigned int len);
DLL_IMPORT  unsigned int ADQ_SetInternalTriggerFrequency(void* adq_cu_ptr, int adq_num , unsigned int Int_Trig_Freq);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingOneChannelSetup(void* adq_cu_ptr, int adq_num , unsigned int SamplePerRecord, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int ArmMode, unsigned int ReadOutSpeed, unsigned int Channel);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingSetupV5(void* adq_cu_ptr, int adq_num , unsigned int SamplePerRecord, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int TriggeredStreamingFlags);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingArmV5(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingGetStatusV5(void* adq_cu_ptr, int adq_num , unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingShutdownV5(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingGetWaveformV5(void* adq_cu_ptr, int adq_num , short* waveform_data_short);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingDisarmV5(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingParseDataStream(void* adq_cu_ptr, int adq_num , unsigned int samples_per_record, int* data_stream, int** data_target);
DLL_IMPORT  unsigned int ADQ_WaveformAveragingSetup(void* adq_cu_ptr, int adq_num , unsigned int NofWaveforms, unsigned int NofSamples,unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples,unsigned int WaveformAveragingFlags);
DLL_IMPORT  unsigned int ADQ_WaveformAveragingArm(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_WaveformAveragingDisarm(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_WaveformAveragingStartReadout(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_WaveformAveragingGetWaveform(void* adq_cu_ptr, int adq_num , int* waveform_data);
DLL_IMPORT  unsigned int ADQ_WaveformAveragingGetStatus(void* adq_cu_ptr, int adq_num , unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle);
DLL_IMPORT  unsigned int ADQ_WaveformAveragingShutdown(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_WaveformAveragingParseDataStream(void* adq_cu_ptr, int adq_num , unsigned int samples_per_record, int* data_stream, int** data_target);
DLL_IMPORT  unsigned int ADQ_WaveformAveragingSoftwareTrigger(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_ResetOverheat(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetDSPData(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetDSPDataNowait(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_PacketStreamingSetup(void* adq_cu_ptr, int adq_num , unsigned int PacketSizeSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples);
DLL_IMPORT  unsigned int ADQ_PacketStreamingArm(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_PacketStreamingDisarm(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_InitTransfer(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SetSendLength(void* adq_cu_ptr, int adq_num , unsigned int length);
DLL_IMPORT  unsigned int ADQ_GetSendLength(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_WaitForPCIeDMAFinish(void* adq_cu_ptr, int adq_num , unsigned int length);
DLL_IMPORT  unsigned long ADQ_GetPhysicalAddress(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetDACOffsetVoltage(void* adq_cu_ptr, int adq_num , unsigned char channel, float v);
DLL_IMPORT  unsigned int ADQ_AWGReset(void* adq_cu_ptr, int adq_num , unsigned int dacId);
DLL_IMPORT  unsigned int ADQ_AWGmalloc(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int LengthSeg1, unsigned int LengthSeg2, unsigned int LengthSeg3, unsigned int LengthSeg4);
DLL_IMPORT  unsigned int ADQ_AWGSegmentMalloc(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int segId, unsigned int length, unsigned char reallocate);
DLL_IMPORT  unsigned int ADQ_AWGWriteSegment(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int segId, unsigned int enable, unsigned int NofLaps, unsigned int length, int *data);
DLL_IMPORT  unsigned int ADQ_AWGWriteSegments(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int NofSegs, unsigned int* segId, unsigned int* NofLaps, unsigned int* length, short int** data);
DLL_IMPORT  unsigned int ADQ_AWGWritePlaylist(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int NofPlaylistElements, unsigned int* index, unsigned int* write_mask,unsigned int* segId, unsigned int* NofLaps, unsigned int* nextIndex,unsigned int* triggerType, unsigned int* triggerLength, unsigned int* triggerPolarity, unsigned int* triggerSample,unsigned int* triggerULSignals);
DLL_IMPORT  unsigned int ADQ_AWGWritePlaylistItem(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int index, unsigned int write_mask,unsigned int segId, unsigned int NofLaps, unsigned int nextIndex,unsigned int triggerType, unsigned int triggerLength, unsigned int triggerPolarity, unsigned int triggerSample,unsigned int triggerULSignals);
DLL_IMPORT  unsigned int ADQ_AWGPlaylistMode(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int mode);
DLL_IMPORT  unsigned int ADQ_AWGEnableSegments(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int enableSeg);
DLL_IMPORT  unsigned int ADQ_AWGAutoRearm(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int enable);
DLL_IMPORT  unsigned int ADQ_AWGContinuous(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int enable);
DLL_IMPORT  unsigned int ADQ_AWGTrigMode(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int trigmode);
DLL_IMPORT  unsigned int ADQ_AWGArm(void* adq_cu_ptr, int adq_num , unsigned int dacId);
DLL_IMPORT  unsigned int ADQ_AWGDisarm(void* adq_cu_ptr, int adq_num , unsigned int dacId);
DLL_IMPORT  unsigned int ADQ_AWGTrig(void* adq_cu_ptr, int adq_num , unsigned int dacId);
DLL_IMPORT  unsigned int ADQ_SendIPCommand(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr, unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int* answer);
DLL_IMPORT  unsigned int ADQ_OffsetDACSpiWrite(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned int data);
DLL_IMPORT  unsigned int ADQ_DACSpiWrite(void* adq_cu_ptr, int adq_num , unsigned char channel, const unsigned char address, const unsigned char data);
DLL_IMPORT  unsigned int ADQ_DACSpiRead(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned char address, unsigned char *data);
DLL_IMPORT  int ADQ_TrigOutEn(void* adq_cu_ptr, int adq_num , unsigned int en);
DLL_IMPORT  int ADQ_ADCCalibrate(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_WriteSTARBDelay(void* adq_cu_ptr, int adq_num , unsigned int starbdelay, unsigned int writetoeeprom);
DLL_IMPORT  unsigned int ADQ_EnablePXIeTriggers(void* adq_cu_ptr, int adq_num , unsigned int port, unsigned int bitflags);
DLL_IMPORT  unsigned int ADQ_EnablePXIeTrigout(void* adq_cu_ptr, int adq_num , unsigned int port, unsigned int bitflags);
DLL_IMPORT  unsigned int ADQ_PXIeSoftwareTrigger(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetPXIeTrigDirection(void* adq_cu_ptr, int adq_num , unsigned int trig0output, unsigned int trig1output);
DLL_IMPORT  unsigned int ADQ_AWGSetupTrigout(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int trigoutmode, unsigned int pulselength, unsigned int enableflags, unsigned int autorearm);
DLL_IMPORT  unsigned int ADQ_AWGTrigoutArm(void* adq_cu_ptr, int adq_num , unsigned int dacId);
DLL_IMPORT  unsigned int ADQ_AWGTrigoutDisarm(void* adq_cu_ptr, int adq_num , unsigned int dacId);
DLL_IMPORT  unsigned int ADQ_AWGSetTriggerEnable(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned int bitflags);
DLL_IMPORT  unsigned int ADQ_AWGSetInterpolationFilter(void* adq_cu_ptr, int adq_num , unsigned int dacId, unsigned char interpolation_filter);
DLL_IMPORT  unsigned int ADQ_ResetRecorder(void* adq_cu_ptr, int adq_num , unsigned int inst);
DLL_IMPORT  unsigned int ADQ_ResetFIFOPaths(void* adq_cu_ptr, int adq_num , unsigned int inst);
DLL_IMPORT  unsigned int ADQ_RunRecorderSelfTest(void* adq_cu_ptr, int adq_num , unsigned int inst, unsigned int* inout_vector);
DLL_IMPORT  unsigned int ADQ_GetRecorderStatus(void* adq_cu_ptr, int adq_num , unsigned int inst, unsigned int* status);
DLL_IMPORT  unsigned int ADQ_GetRecorderDiskStatus(void* adq_cu_ptr, int adq_num , unsigned int inst, unsigned int diskno, unsigned int* status);
DLL_IMPORT  unsigned int ADQ_BreakRecorderCommand(void* adq_cu_ptr, int adq_num , unsigned int inst);
DLL_IMPORT  unsigned int ADQ_SendRecorderCommand(void* adq_cu_ptr, int adq_num , unsigned int inst, unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int* answer);
DLL_IMPORT  unsigned int ADQ_WriteDataToDSU(void* adq_cu_ptr, int adq_num , unsigned int inst, unsigned int start_address, unsigned int nofbytes, unsigned char* data);
DLL_IMPORT  unsigned int ADQ_ReadDataFromDSU(void* adq_cu_ptr, int adq_num , unsigned int inst, unsigned int start_address, unsigned int nofbytes, unsigned char* data);
DLL_IMPORT  unsigned int ADQ_SetupDSUAcquisition(void* adq_cu_ptr, int adq_num , unsigned int inst, unsigned int start_address, unsigned int end_address);
DLL_IMPORT  unsigned int ADQ_StartDSUAcquisition(void* adq_cu_ptr, int adq_num , unsigned int inst);
DLL_IMPORT  unsigned int ADQ_GetNextDSURecordingAddress(void* adq_cu_ptr, int adq_num , unsigned int inst, unsigned int* next_address);
DLL_IMPORT  unsigned int ADQ_GetPCIeLinkWidth(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetPCIeLinkRate(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetPCIeTLPSize(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetExtTrigThreshold(void* adq_cu_ptr, int adq_num , unsigned int trignum, double vthresh);
DLL_IMPORT  int ADQ_Blink(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr, unsigned int* freqcalflag);
DLL_IMPORT  unsigned int ADQ_SetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int adq_num , unsigned char IPInstanceAddr, unsigned int freqcalmode);
DLL_IMPORT  unsigned int ADQ_WriteUserRegister(void* adq_cu_ptr, int adq_num , unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT  unsigned int ADQ_ReadUserRegister(void* adq_cu_ptr, int adq_num , unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT  unsigned int ADQ_WriteBlockUserRegister(void* adq_cu_ptr, int adq_num , unsigned int ul_target, unsigned int start_addr, unsigned int *data, unsigned int num_bytes, unsigned int options);
DLL_IMPORT  unsigned int ADQ_ReadBlockUserRegister(void* adq_cu_ptr, int adq_num , unsigned int ul_target, unsigned int start_addr, unsigned int *data, unsigned int num_bytes, unsigned int options);
DLL_IMPORT  unsigned int ADQ_BypassUserLogic(void* adq_cu_ptr, int adq_num , unsigned int ul_target, unsigned int bypass);
DLL_IMPORT  unsigned int ADQ_GetProductFamily(void* adq_cu_ptr, int adq_num , unsigned int* family);
DLL_IMPORT  unsigned int ADQ_GetHardwareAssemblyPartNumber(void* adq_cu_ptr, int adq_num , char* partnum);
DLL_IMPORT  unsigned int ADQ_GetHardwareSubassemblyPartNumber(void* adq_cu_ptr, int adq_num , char* partnum);
DLL_IMPORT  unsigned int ADQ_GetPCBAssemblyPartNumber(void* adq_cu_ptr, int adq_num , char* partnum);
DLL_IMPORT  unsigned int ADQ_GetPCBPartNumber(void* adq_cu_ptr, int adq_num , char* partnum);
DLL_IMPORT  unsigned int ADQ_GetFPGApart(void* adq_cu_ptr, int adq_num , unsigned int fpganum, char* partstr);
DLL_IMPORT  unsigned int ADQ_GetFPGATempGrade(void* adq_cu_ptr, int adq_num , unsigned int fpganum, char* tgrade);
DLL_IMPORT  unsigned int ADQ_GetFPGASpeedGrade(void* adq_cu_ptr, int adq_num , unsigned int fpganum, unsigned int* sgrade);
DLL_IMPORT  unsigned int ADQ_SetBiasDACPercentage(void* adq_cu_ptr, int adq_num , unsigned int channel, float percent);
DLL_IMPORT  unsigned int ADQ_SetupLevelTrigger(void* adq_cu_ptr, int adq_num , int* level, int* edge, int* resetlevel, unsigned int channelsmask, unsigned int individual_mode);
DLL_IMPORT  unsigned int ADQ_GetRxFifoOverflow(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_HasTriggeredStreamingFunctionality(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingSetup(void* adq_cu_ptr, int adq_num , unsigned int NofRecords,unsigned int NofSamples,unsigned int NofPreTrigSamples,unsigned int NofHoldOffSamples,unsigned char ChannelsMask);
DLL_IMPORT  unsigned int ADQ_SetTriggeredStreamingTotalNofRecords(void* adq_cu_ptr, int adq_num , unsigned int MaxNofRecordsTotal);
DLL_IMPORT  unsigned int ADQ_GetTriggeredStreamingRecordSizeBytes(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetTriggeredStreamingHeaderSizeBytes(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetTriggeredStreamingRecords(void* adq_cu_ptr, int adq_num , unsigned int NofRecordsToRead, void** data_buf, void* header_buf, unsigned int* NofRecordsRead);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingGetNofRecordsCompleted(void* adq_cu_ptr, int adq_num , unsigned int ChannelsMask,  unsigned int* NofRecordsCompleted);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingGetStatus(void* adq_cu_ptr, int adq_num , unsigned int* InIdle, unsigned int* TriggerSkipped, unsigned int* Overflow);
DLL_IMPORT  unsigned int ADQ_SetTriggeredStreamingHeaderRegister(void* adq_cu_ptr, int adq_num , char RegValue);
DLL_IMPORT  unsigned int ADQ_SetTriggeredStreamingHeaderSerial(void* adq_cu_ptr, int adq_num , unsigned int SerialNumber);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingArm(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_TriggeredStreamingDisarm(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_ParseTriggeredStreamingHeader(void* adq_cu_ptr, int adq_num , void* HeaderPtr, unsigned long long* Timestamp, unsigned int* Channel, unsigned int* ExtraAccuracy,int* RegisterValue, unsigned int* SerialNumber, unsigned int* RecordCounter);
DLL_IMPORT  unsigned int ADQ_HasAdjustableInputRange(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetInputRange(void* adq_cu_ptr, int adq_num , unsigned int channel, float inputrangemVpp, float* result);
DLL_IMPORT  unsigned int ADQ_SetAdjustableBias(void* adq_cu_ptr, int adq_num , unsigned int channel, int ADCcodes);
DLL_IMPORT  unsigned int ADQ_WriteADQATTStateManual(void* adq_cu_ptr, int adq_num , unsigned int channel, unsigned int relay16, unsigned int relay8, unsigned int ptap8, unsigned int ptap4,unsigned int ptap2, unsigned int ptap1, unsigned int ptap05, unsigned int ptap025);
DLL_IMPORT  unsigned int ADQ_GetAdjustableBias(void* adq_cu_ptr, int adq_num , unsigned int channel, int *ADCcodes);
DLL_IMPORT  unsigned int ADQ_GetInputRange(void* adq_cu_ptr, int adq_num , unsigned int channel, float *InpRange);
DLL_IMPORT  unsigned int ADQ_GetRecorderBytesPerAddress(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetFanControl(void* adq_cu_ptr, int adq_num , unsigned int fan_control);
DLL_IMPORT  int ADQ_ADCReg(void* adq_cu_ptr, int adq_num , unsigned char addr, unsigned char adc, unsigned int val);
DLL_IMPORT  unsigned int ADQ_SetupDBS(void* adq_cu_ptr, int adq_num , unsigned char DBS_instance, unsigned int bypass, int dc_target, int lower_saturation_level, int upper_saturation_level);
DLL_IMPORT  unsigned int ADQ_EnableWFATriggerCounter(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_DisableWFATriggerCounter(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_StartWFATriggerCounter(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetWFANumberOfTriggers(void* adq_cu_ptr, int adq_num , unsigned int number_of_triggers);
DLL_IMPORT  unsigned int ADQ_MeasureInputOffset(void* adq_cu_ptr, int adq_num , unsigned int channel, int* value);
DLL_IMPORT  unsigned int ADQ_SetOffsetCompensationDAC(void* adq_cu_ptr, int adq_num , unsigned int channel, unsigned int daccode);
DLL_IMPORT  unsigned int ADQ_RunCalibrationADQ412DC(void* adq_cu_ptr, int adq_num , unsigned int calmode);
DLL_IMPORT  unsigned int ADQ_ResetCalibrationStateADQ412DC(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_DisconnectInputs(void* adq_cu_ptr, int adq_num , unsigned int channelmask);
DLL_IMPORT  unsigned int ADQ_RxSetFrequency(void* adq_cu_ptr, int adq_num , unsigned long long int frequency);
DLL_IMPORT  unsigned int ADQ_RxSetRfAmplifier(void* adq_cu_ptr, int adq_num , unsigned char amplifier, unsigned char mode);
DLL_IMPORT  unsigned int ADQ_RxSetRfPath(void* adq_cu_ptr, int adq_num , unsigned char mode);
DLL_IMPORT  unsigned int ADQ_RxSetLoOut(void* adq_cu_ptr, int adq_num , unsigned char mode);
DLL_IMPORT  unsigned int ADQ_RxSetRfAttenuation(void* adq_cu_ptr, int adq_num , unsigned char att_index, unsigned char att);
DLL_IMPORT  unsigned int ADQ_RxSetRfFilter(void* adq_cu_ptr, int adq_num , unsigned char filter);
DLL_IMPORT  unsigned int ADQ_RxSetLoFilter(void* adq_cu_ptr, int adq_num , unsigned char filter);
DLL_IMPORT  unsigned int ADQ_RxSetIfGainDac(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned char dacValue);
DLL_IMPORT  unsigned int ADQ_RxSetVcomDac(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned short dacValue);
DLL_IMPORT  unsigned int ADQ_RxSetDcOffsetDac(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned short dacValue);
DLL_IMPORT  unsigned int ADQ_RxSetLinearityDac(void* adq_cu_ptr, int adq_num , unsigned short dacValue);
DLL_IMPORT  unsigned int ADQ_TxSetFrequency(void* adq_cu_ptr, int adq_num , unsigned long long int frequency);
DLL_IMPORT  unsigned int ADQ_TxSetRfAmplifier(void* adq_cu_ptr, int adq_num , unsigned char amplifier, unsigned char mode);
DLL_IMPORT  unsigned int ADQ_TxSetRfPath(void* adq_cu_ptr, int adq_num , unsigned char mode);
DLL_IMPORT  unsigned int ADQ_TxSetLoOut(void* adq_cu_ptr, int adq_num , unsigned char mode);
DLL_IMPORT  unsigned int ADQ_TxSetRfAttenuation(void* adq_cu_ptr, int adq_num , unsigned char att_index, unsigned char att);
DLL_IMPORT  unsigned int ADQ_TxSetRfFilter(void* adq_cu_ptr, int adq_num , unsigned char filter);
DLL_IMPORT  unsigned int ADQ_TxSetLoFilter(void* adq_cu_ptr, int adq_num , unsigned char filter);
DLL_IMPORT  unsigned int ADQ_TxSetDcOffsetDac(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned short dacValue);
DLL_IMPORT  unsigned int ADQ_TxSetLinearityDac(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned short dacValue);
DLL_IMPORT  unsigned int ADQ_SynthSetDeviceStandby(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned char standbyStatus);
DLL_IMPORT  unsigned int ADQ_SynthSetFrequency(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned long long int frequency);
DLL_IMPORT  unsigned int ADQ_SynthSetPowerLevel(void* adq_cu_ptr, int adq_num , unsigned char channel, float powerLevel);
DLL_IMPORT  unsigned int ADQ_SynthSetRfOutput(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned char mode);
DLL_IMPORT  unsigned int ADQ_SynthSetAlcMode(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned char mode);
DLL_IMPORT  unsigned int ADQ_SynthDisableAutoLevel(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned char mode);
DLL_IMPORT  unsigned int ADQ_SynthSetClockReference(void* adq_cu_ptr, int adq_num , unsigned char lockExtEnable, unsigned char refOutEnable, unsigned char pxiClk);
DLL_IMPORT  unsigned int ADQ_SynthSetReferenceDac(void* adq_cu_ptr, int adq_num , unsigned int dacValue);
DLL_IMPORT  unsigned int ADQ_SynthSetAlcDac(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned int dacValue);
DLL_IMPORT  unsigned int ADQ_SynthGetAlcDac(void* adq_cu_ptr, int adq_num , unsigned char channel, unsigned int* dacValue);
DLL_IMPORT  unsigned int ADQ_GetDeviceStatus(void* adq_cu_ptr, int adq_num , unsigned int* status);
DLL_IMPORT  int ADQ_GetData(void* adq_cu_ptr, int adq_num , void** target_buffers,unsigned int target_buffer_size,unsigned char target_bytes_per_sample,unsigned int StartRecordNumber,unsigned int NumberOfRecords,unsigned char ChannelsMask,unsigned int StartSample,unsigned int nSamples,unsigned char TransferMode);
DLL_IMPORT  int ADQ_GetDataWH(void* adq_cu_ptr, int adq_num , void** target_buffers,void* target_headers,unsigned int target_buffer_size,unsigned char target_bytes_per_sample,unsigned int StartRecordNumber,unsigned int NumberOfRecords,unsigned char ChannelsMask,unsigned int StartSample,unsigned int nSamples,unsigned char TransferMode);
DLL_IMPORT  int ADQ_GetDataWHTS(void* adq_cu_ptr, int adq_num , void** target_buffers,void* target_headers,void* target_timestamps,unsigned int target_buffer_size,unsigned char target_bytes_per_sample,unsigned int StartRecordNumber,unsigned int NumberOfRecords,unsigned char ChannelsMask,unsigned int StartSample,unsigned int nSamples,unsigned char TransferMode);
DLL_IMPORT  int ADQ_GetDataStreaming(void* adq_cu_ptr, int adq_num , void** target_buffers,void** target_headers,unsigned char channels_mask,unsigned int* samples_added,unsigned int* headers_added,unsigned int* header_status);
DLL_IMPORT  unsigned int ADQ_GetLastError(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  void* ADQ_GetPtrStream(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetErrorVector(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_IsAlive(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_ReadEEPROMDB(void* adq_cu_ptr, int adq_num , unsigned int addr);
DLL_IMPORT  unsigned int ADQ_ReadEEPROM(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int i2c_addr);
DLL_IMPORT  unsigned int ADQ_ResetDevice(void* adq_cu_ptr, int adq_num , int resetlevel);
DLL_IMPORT  unsigned int ADQ_InvalidateCache(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SetCacheSize(void* adq_cu_ptr, int adq_num , unsigned int newSizeInBytes);
DLL_IMPORT  int ADQ_SetTransferBuffers(void* adq_cu_ptr, int adq_num , unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT  unsigned int ADQ_GetTransferBufferStatus(void* adq_cu_ptr, int adq_num , unsigned int* filled);
DLL_IMPORT  unsigned int ADQ_FlushDMA(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_WriteEEPROM(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT  unsigned int ADQ_WriteEEPROMDB(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT  int ADQ_ParseEEPROMBlock(void* adq_cu_ptr, int adq_num , char* blockname, char* map_version, unsigned int buffer_len, unsigned char* buffer, unsigned int i2c_addr);
DLL_IMPORT  int ADQ_SetDelayLineValues(void* adq_cu_ptr, int adq_num , int samplerate, unsigned int linear_interpolation);
DLL_IMPORT  int ADQ_SetDelayLineValuesDirect(void* adq_cu_ptr, int adq_num , unsigned int delay1, unsigned int delay2);
DLL_IMPORT  int ADQ_SetLvlTrigLevel(void* adq_cu_ptr, int adq_num , int level);
DLL_IMPORT  int ADQ_SetLvlTrigEdge(void* adq_cu_ptr, int adq_num , int edge);
DLL_IMPORT  int ADQ_SetClockSource(void* adq_cu_ptr, int adq_num , int source);
DLL_IMPORT  int ADQ_SetClockFrequencyMode(void* adq_cu_ptr, int adq_num , int clockmode);
DLL_IMPORT  int ADQ_SetPllFreqDivider(void* adq_cu_ptr, int adq_num , int divider);
DLL_IMPORT  int ADQ_SetPll(void* adq_cu_ptr, int adq_num , int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT  int ADQ_SetTriggerMode(void* adq_cu_ptr, int adq_num , int trig_mode);
DLL_IMPORT  int ADQ_SetAuxTriggerMode(void* adq_cu_ptr, int adq_num , int trig_mode);
DLL_IMPORT  int ADQ_SetPreTrigSamples(void* adq_cu_ptr, int adq_num , unsigned int PreTrigSamples);
DLL_IMPORT  int ADQ_SetTriggerHoldOffSamples(void* adq_cu_ptr, int adq_num , unsigned int TriggerHoldOffSamples);
DLL_IMPORT  int ADQ_SetBufferSizePages(void* adq_cu_ptr, int adq_num , unsigned int pages);
DLL_IMPORT  int ADQ_SetBufferSizeWords(void* adq_cu_ptr, int adq_num , unsigned int words);
DLL_IMPORT  int ADQ_SetBufferSize(void* adq_cu_ptr, int adq_num , unsigned int samples);
DLL_IMPORT  int ADQ_SetNofBits(void* adq_cu_ptr, int adq_num , unsigned int NofBits);
DLL_IMPORT  int ADQ_SetSampleWidth(void* adq_cu_ptr, int adq_num , unsigned int SampleWidth);
DLL_IMPORT  int ADQ_SetWordsPerPage(void* adq_cu_ptr, int adq_num , unsigned int WordsPerPage);
DLL_IMPORT  int ADQ_SetPreTrigWords(void* adq_cu_ptr, int adq_num , unsigned int PreTrigWords);
DLL_IMPORT  int ADQ_SetWordsAfterTrig(void* adq_cu_ptr, int adq_num , unsigned int WordsAfterTrig);
DLL_IMPORT  int ADQ_SetTrigLevelResetValue(void* adq_cu_ptr, int adq_num , int OffsetValue);
DLL_IMPORT  int ADQ_SetTrigMask1(void* adq_cu_ptr, int adq_num , unsigned int TrigMask);
DLL_IMPORT  int ADQ_SetTrigLevel1(void* adq_cu_ptr, int adq_num , int TrigLevel);
DLL_IMPORT  int ADQ_SetTrigPreLevel1(void* adq_cu_ptr, int adq_num , int TrigLevel);
DLL_IMPORT  int ADQ_SetTrigCompareMask1(void* adq_cu_ptr, int adq_num , unsigned int TrigCompareMask);
DLL_IMPORT  int ADQ_SetTrigMask2(void* adq_cu_ptr, int adq_num , unsigned int TrigMask);
DLL_IMPORT  int ADQ_SetTrigLevel2(void* adq_cu_ptr, int adq_num , int TrigLevel);
DLL_IMPORT  int ADQ_SetTrigPreLevel2(void* adq_cu_ptr, int adq_num , int TrigLevel);
DLL_IMPORT  int ADQ_SetTrigCompareMask2(void* adq_cu_ptr, int adq_num , unsigned int TrigCompareMask);
DLL_IMPORT  unsigned int ADQ_SetExternTrigEdge(void* adq_cu_ptr, int adq_num , unsigned int edge);
DLL_IMPORT  unsigned int ADQ_GetExternTrigEdge(void* adq_cu_ptr, int adq_num , unsigned int* edge);
DLL_IMPORT  unsigned int ADQ_SetExternalTriggerDelay(void* adq_cu_ptr, int adq_num , unsigned char delaycycles);
DLL_IMPORT  int ADQ_ArmTrigger(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_DisarmTrigger(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_StartStreaming(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_StopStreaming(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SWTrig(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_CollectDataNextPage(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_CollectDataNextPageWithPrefetch(void* adq_cu_ptr, int adq_num , unsigned int prefetch);
DLL_IMPORT  int ADQ_GetWaitingForTrigger(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetAcquired(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetTrigged(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetAcquiredRecords(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetAcquiredRecordsAndLoopCounter(void* adq_cu_ptr, int adq_num , unsigned int* acquired_records, unsigned int* loop_counter);
DLL_IMPORT  unsigned int ADQ_GetPageCount(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetLvlTrigLevel(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetLvlTrigFlank(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetLvlTrigEdge(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetOutputWidth(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetPllFreqDivider(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetClockSource(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetTriggerMode(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetBufferSizePages(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetBufferSize(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned long long ADQ_GetMaxBufferSize(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetMaxBufferSizePages(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetSamplesPerPage(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetUSBAddress(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned long ADQ_GetPCIeAddress(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetBcdDevice(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SetStreamConfig(void* adq_cu_ptr, int adq_num , unsigned int option, unsigned int value);
DLL_IMPORT  int ADQ_GetStreamConfig(void* adq_cu_ptr, int adq_num , unsigned int option, unsigned int *value);
DLL_IMPORT  int ADQ_GetStreamStatus(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SetStreamStatus(void* adq_cu_ptr, int adq_num , int status);
DLL_IMPORT  int ADQ_GetStreamOverflow(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  char* ADQ_GetBoardSerialNumber(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int* ADQ_GetRevision(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetTriggerInformation(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetTrigPoint(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetTrigType(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_GetOverflow(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetRecordSize(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetNofRecords(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_IsUSBDevice(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_IsUSB3Device(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_IsPCIeDevice(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_IsPCIeLiteDevice(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SendProcessorCommand(void* adq_cu_ptr, int adq_num , unsigned int command, unsigned int addr, unsigned int mask, unsigned int data);
DLL_IMPORT  unsigned int ADQ_SendLongProcessorCommand(void* adq_cu_ptr, int adq_num , unsigned int command, unsigned int addr, unsigned int mask, unsigned int data);
DLL_IMPORT  unsigned int ADQ_WriteRegister(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int mask, unsigned int data);
DLL_IMPORT  unsigned int ADQ_ReadRegister(void* adq_cu_ptr, int adq_num , unsigned int addr);
DLL_IMPORT  unsigned int ADQ_WriteAlgoRegister(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int mask, unsigned int data);
DLL_IMPORT  unsigned int ADQ_ReadAlgoRegister(void* adq_cu_ptr, int adq_num , unsigned int addr);
DLL_IMPORT  unsigned int ADQ_WriteI2C(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int nbytes, unsigned int data);
DLL_IMPORT  unsigned int ADQ_WriteDBI2C(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int nbytes, unsigned int data);
DLL_IMPORT  unsigned int ADQ_ReadI2C(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int nbytes);
DLL_IMPORT  unsigned int ADQ_ReadDBI2C(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int nbytes);
DLL_IMPORT  unsigned int ADQ_WriteReadI2C(void* adq_cu_ptr, int adq_num , unsigned int addr, unsigned int rbytes, unsigned int wbytes, unsigned int wrdata);
DLL_IMPORT  unsigned int ADQ_GetTemperature(void* adq_cu_ptr, int adq_num , unsigned int addr);
DLL_IMPORT  unsigned int ADQ_GetTemperatureFloat(void* adq_cu_ptr, int adq_num , unsigned int addr, float* temperature);
DLL_IMPORT  unsigned int ADQ_MultiRecordSetup(void* adq_cu_ptr, int adq_num , unsigned int NumberOfRecords, unsigned int SamplesPerRecord);
DLL_IMPORT  unsigned int ADQ_MultiRecordSetupGP(void* adq_cu_ptr, int adq_num , unsigned int NumberOfRecords, unsigned int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT  unsigned int ADQ_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int adq_num , unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT  unsigned int ADQ_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int adq_num , unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT  unsigned int ADQ_GetDataMultiRecordSetup(void* adq_cu_ptr, int adq_num , unsigned int NumberOfRecords, unsigned int SamplesPerRecord);
DLL_IMPORT  unsigned int ADQ_MultiRecordClose(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetAcquiredAll(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetTriggedAll(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_IsStartedOK(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_MemoryDump(void* adq_cu_ptr, int adq_num , unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT  unsigned int ADQ_MemoryDumpRecords(void* adq_cu_ptr, int adq_num , unsigned int StartRecord, unsigned int NofRecords, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT  unsigned int ADQ_MemoryShadow(void* adq_cu_ptr, int adq_num , void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT  unsigned int ADQ_GetDataFormat(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetTestPatternMode(void* adq_cu_ptr, int adq_num , int mode);
DLL_IMPORT  unsigned int ADQ_SetTestPatternConstant(void* adq_cu_ptr, int adq_num , int value);
DLL_IMPORT  int ADQ_SetDirectionTrig(void* adq_cu_ptr, int adq_num , int direction);
DLL_IMPORT  int ADQ_WriteTrig(void* adq_cu_ptr, int adq_num , int data);
DLL_IMPORT  unsigned int ADQ_SetConfigurationTrig(void* adq_cu_ptr, int adq_num , unsigned int mode, unsigned int pulselength, unsigned int invertoutput);
DLL_IMPORT  unsigned int ADQ_EnableInternalTriggerCounts(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_DisableInternalTriggerCounts(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_ClearInternalTriggerCounts(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetInternalTriggerCounts(void* adq_cu_ptr, int adq_num , unsigned int trigger_counts);
DLL_IMPORT  unsigned int ADQ_ConfigureDebugCounter(void* adq_cu_ptr, int adq_num , unsigned int direction, unsigned int output_mode, unsigned int counter_mode, int initial_value);
DLL_IMPORT  int ADQ_SetDirectionGPIO(void* adq_cu_ptr, int adq_num , unsigned int direction, unsigned int mask);
DLL_IMPORT  int ADQ_WriteGPIO(void* adq_cu_ptr, int adq_num , unsigned int data, unsigned int mask);
DLL_IMPORT  unsigned int ADQ_ReadGPIO(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int* ADQ_GetMultiRecordHeader(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned long long ADQ_GetTrigTime(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned long long ADQ_GetTrigTimeCycles(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetTrigTimeSyncs(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetTrigTimeStart(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SetTrigTimeMode(void* adq_cu_ptr, int adq_num , int TrigTimeMode);
DLL_IMPORT  int ADQ_ResetTrigTimer(void* adq_cu_ptr, int adq_num , int TrigTimeRestart);
DLL_IMPORT  unsigned int ADQ_ra(void* adq_cu_ptr, int adq_num , const char* regname);
DLL_IMPORT  unsigned int ADQ_RegisterNameLookup(void* adq_cu_ptr, int adq_num , const char* regname, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT  int ADQ_SpiSend(void* adq_cu_ptr, int adq_num , unsigned char addr, const char* data,unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT  unsigned int ADQ_GetExternalClockReferenceStatus(void* adq_cu_ptr, int adq_num , unsigned int *extrefstatus);
DLL_IMPORT  unsigned int ADQ_SetTransferTimeout(void* adq_cu_ptr, int adq_num , unsigned int value);
DLL_IMPORT  unsigned int ADQ_StorePCIeConfig(void* adq_cu_ptr, int adq_num , UINT32* pci_space);
DLL_IMPORT  unsigned int ADQ_ReloadPCIeConfig(void* adq_cu_ptr, int adq_num , UINT32* pci_space);
DLL_IMPORT  const char* ADQ_GetADQDSPOption(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_SetEthernetPllFreq(void* adq_cu_ptr, int adq_num , unsigned char eth10_freq, unsigned char eth1_freq);
DLL_IMPORT  unsigned int ADQ_SetPointToPointPllFreq(void* adq_cu_ptr, int adq_num , unsigned char pp_freq);
DLL_IMPORT  unsigned int ADQ_SetEthernetPll(void* adq_cu_ptr, int adq_num , unsigned short refdiv, unsigned char useref2, unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv, unsigned char eth10_outdiv, unsigned char eth1_outdiv);
DLL_IMPORT  unsigned int ADQ_SetPointToPointPll(void* adq_cu_ptr, int adq_num , unsigned short refdiv, unsigned char useref2,  unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv, unsigned char pp_outdiv, unsigned char ppsync_outdiv);
DLL_IMPORT  unsigned int ADQ_SetDirectionMLVDS(void* adq_cu_ptr, int adq_num , unsigned char direction);
DLL_IMPORT  const char* ADQ_GetNGCPartNumber(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  const char* ADQ_GetUserLogicPartNumber(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetProductVariant(void* adq_cu_ptr, int adq_num , unsigned int* ProductVariant);
DLL_IMPORT  unsigned int ADQ_SetInternalTriggerPeriod(void* adq_cu_ptr, int adq_num , unsigned int TriggerPeriodClockCycles);
DLL_IMPORT  unsigned int ADQ_FX2ReadRequest(void* adq_cu_ptr, int adq_num , unsigned int requestcode, unsigned int value, unsigned int index, long len, char *buf);
DLL_IMPORT  unsigned int ADQ_FX2WriteRequest(void* adq_cu_ptr, int adq_num , unsigned int requestcode, unsigned int value, unsigned int index, long len, char *data);
DLL_IMPORT  unsigned int ADQ_GetProductID(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_BootAdqFromFlash(void* adq_cu_ptr, int adq_num , unsigned int addr);
DLL_IMPORT  unsigned int ADQ_IsBootloader(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_TrigoutEnable(void* adq_cu_ptr, int adq_num , unsigned int bitflags);
DLL_IMPORT  unsigned int ADQ_HasTrigHardware(void* adq_cu_ptr, int adq_num , unsigned int trignum);
DLL_IMPORT  unsigned int ADQ_HasTrigoutHardware(void* adq_cu_ptr, int adq_num , unsigned int trignum);
DLL_IMPORT  unsigned int ADQ_HasVariableTrigThreshold(void* adq_cu_ptr, int adq_num , unsigned int trignum);
DLL_IMPORT  unsigned int ADQ_WriteToDataEP(void* adq_cu_ptr, int adq_num , unsigned int *pData, unsigned int length);
DLL_IMPORT  unsigned int ADQ_SendDataDev2Dev(void* adq_cu_ptr, int adq_num , unsigned long PhysicalAddress, unsigned int channel, unsigned int options);
DLL_IMPORT  unsigned int ADQ_GetP2pStatus(void* adq_cu_ptr, int adq_num , unsigned int* pending, unsigned int channel);
DLL_IMPORT  unsigned int ADQ_SetP2pSize(void* adq_cu_ptr, int adq_num , unsigned int bytes, unsigned int channel);
DLL_IMPORT  unsigned int ADQ_GetP2pSize(void* adq_cu_ptr, int adq_num , unsigned int channel);
DLL_IMPORT  unsigned int ADQ_HasAdjustableBias(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetUSB3Config(void* adq_cu_ptr, int adq_num , unsigned int option, unsigned int* value);
DLL_IMPORT  unsigned int ADQ_SetUSB3Config(void* adq_cu_ptr, int adq_num , unsigned int option, unsigned int value);
DLL_IMPORT  unsigned int ADQ_GetInputImpedance(void* adq_cu_ptr, int adq_num , unsigned int channel, float* impedance);
DLL_IMPORT  char* ADQ_GetBoardProductName(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  unsigned int ADQ_GetNofBytesPerSample(void* adq_cu_ptr, int adq_num , unsigned int* bytes_per_sample);
DLL_IMPORT  unsigned int ADQ_GetNofDBSInstances(void* adq_cu_ptr, int adq_num , unsigned int* nof_dbs_instances);
DLL_IMPORT  unsigned int ADQ_SetCalibrationModeADQ412DC(void* adq_cu_ptr, int adq_num , unsigned int calibmode);
DLL_IMPORT  unsigned int ADQ_SetFPGARebootMethod(void* adq_cu_ptr, int adq_num , unsigned int RebootMethod);
DLL_IMPORT  unsigned int ADQ_GetNofRecorderIP(void* adq_cu_ptr, int adq_num , unsigned int* answer);
DLL_IMPORT  unsigned int ADQ_SetOvervoltageProtection(void* adq_cu_ptr, int adq_num , unsigned int enabled);
DLL_IMPORT  unsigned int ADQ_SetADCClockDelay(void* adq_cu_ptr, int adq_num , unsigned int adcnum, float delayval);
DLL_IMPORT  unsigned int ADQ_SetAttenuators(void* adq_cu_ptr, int adq_num , unsigned int channel, unsigned int attmask);
DLL_IMPORT  int ADQ_ATDGetWFAStatus(void* adq_cu_ptr, int adq_num , unsigned int *status);
DLL_IMPORT  int ADQ_ATDSetupWFAAdvanced(void* adq_cu_ptr, int adq_num , unsigned int segment_length, unsigned int segments_per_record, unsigned int records_per_dram, unsigned int record_length, unsigned int nof_accumulations, unsigned int accumulator_offset, unsigned int acc_raw_ratio, unsigned int bypass);
DLL_IMPORT  int ADQ_ATDSetupWFA(void* adq_cu_ptr, int adq_num , unsigned int record_length, unsigned int nof_accumulations);
DLL_IMPORT  int ADQ_ATDSetupThreshold(void* adq_cu_ptr, int adq_num , unsigned int channel_id, unsigned int hysteresis, unsigned int threshold, unsigned int baseline, unsigned int negative_pulse_enable, unsigned int bypass);
DLL_IMPORT  int ADQ_ATDSetThresholdFilter(void* adq_cu_ptr, int adq_num , unsigned int channel_id, unsigned int* coefficients);
DLL_IMPORT  int ADQ_GetDNA(void* adq_cu_ptr, int adq_num , unsigned int* dna);
DLL_IMPORT  int ADQ_ResetDNA(void* adq_cu_ptr, int adq_num , unsigned int assert);
DLL_IMPORT  int ADQ_PDSetupStreaming(void* adq_cu_ptr, int adq_num , unsigned char channels_mask);
DLL_IMPORT  int ADQ_PDSetupLevelTrig(void* adq_cu_ptr, int adq_num , unsigned int channel, int trigger_level, int reset_hysteresis, int trigger_arm_hysteresis, int reset_arm_hysteresis, unsigned int trigger_polarity, unsigned int reset_polarity);
DLL_IMPORT  int ADQ_PDEnableLevelTrig(void* adq_cu_ptr, int adq_num , unsigned int enable);
DLL_IMPORT  int ADQ_PDGetLevelTrigStatus(void* adq_cu_ptr, int adq_num , unsigned int* status);
DLL_IMPORT  int ADQ_PDSetupTiming(void* adq_cu_ptr, int adq_num , unsigned int channel, unsigned int nof_pretrigger_samples, unsigned int nof_moving_average_samples, unsigned int moving_average_delay, unsigned int trailing_edge_window, unsigned int number_of_records, unsigned int record_variable_length);
DLL_IMPORT  int ADQ_PDSetupTriggerCoincidence(void* adq_cu_ptr, int adq_num , unsigned int channel, unsigned int window_length, unsigned int mask);
DLL_IMPORT  int ADQ_PDEnableTriggerCoincidence(void* adq_cu_ptr, int adq_num , unsigned int enable);
DLL_IMPORT  int ADQ_PDGetEventCounters(void* adq_cu_ptr, int adq_num , unsigned int* lt_tevent_counter, unsigned int* lt_revent_counter, unsigned int* coin_tevent_counter, unsigned int* coin_revent_counter, unsigned int* pt_tevent_counter, unsigned int* pt_revent_counter, unsigned int* acq_tevent_counter, unsigned int* acq_revent_counter, unsigned int* acq_revent_pt_counter);
DLL_IMPORT  int ADQ_PDAutoTrig(void* adq_cu_ptr, int adq_num , unsigned int channel, int* detected_trigger_level, unsigned int* detected_arm_hystersis);
DLL_IMPORT  int ADQ_HasFeature(void* adq_cu_ptr, int adq_num , char *featurename);
DLL_IMPORT  int ADQ_InitializeStreamingTransfer(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_SetSWTrigValue(void* adq_cu_ptr, int adq_num , float samples);
DLL_IMPORT  unsigned int ADQ_SetDACPercentage(void* adq_cu_ptr, int adq_num , unsigned int spi_addr, unsigned int output_num, float percent);
DLL_IMPORT  unsigned int ADQ_MultiRecordSetChannelMask(void* adq_cu_ptr, int adq_num , unsigned int channelmask);
DLL_IMPORT  unsigned int ADQ_USBReConnect(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_OCTDebug(void* adq_cu_ptr, int adq_num , unsigned int arg1, unsigned int arg2, unsigned int arg3);
DLL_IMPORT  int ADQ_OCTResetOVP(void* adq_cu_ptr, int adq_num);
DLL_IMPORT  int ADQ_OCTSetOVPLevel(void* adq_cu_ptr, int adq_num , unsigned int level);
DLL_IMPORT  int ADQ_OCTGetOVPLevel(void* adq_cu_ptr, int adq_num , unsigned int* level);
DLL_IMPORT  int ADQ_OCTGetOVPStatus(void* adq_cu_ptr, int adq_num , unsigned int* neg_ov, unsigned int* pos_ov);
DLL_IMPORT  int ADQ_OCTSetTriggerCount(void* adq_cu_ptr, int adq_num , unsigned int count);
DLL_IMPORT  int ADQ_OCTSetFFTLength(void* adq_cu_ptr, int adq_num , unsigned int length);
DLL_IMPORT  int ADQ_OCTSetFFTEnable(void* adq_cu_ptr, int adq_num , unsigned int enable);
DLL_IMPORT  int ADQ_OCTSetFFTEnableWin(void* adq_cu_ptr, int adq_num , unsigned int enable);
DLL_IMPORT  int ADQ_OCTSetFFTHoldOff(void* adq_cu_ptr, int adq_num , unsigned int holdoff);
DLL_IMPORT  int ADQ_EnableGPIOSupplyOutput(void* adq_cu_ptr, int adq_num , unsigned int enable);
DLL_IMPORT  int ADQ_USBLinkupTest(void* adq_cu_ptr, int adq_num , unsigned int retries);
DLL_IMPORT  int ADQ_SetClockInputImpedance(void* adq_cu_ptr, int adq_num , unsigned int input_num, unsigned int mode);
DLL_IMPORT  int ADQ_SetTriggerInputImpedance(void* adq_cu_ptr, int adq_num , unsigned int input_num, unsigned int mode);
DLL_IMPORT  int ADQ_MeasureSupplyVoltage(void* adq_cu_ptr, int adq_num , unsigned int sensor_num, float* value);
#ifdef __cplusplus
}
#endif


#define INCLUDE_ADQDSP
#ifdef __cplusplus
extern "C" {
#endif
DLL_IMPORT int*               ADQDSP_GetRevision(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT unsigned int        ADQDSP_WriteRegister(void* adq_cu_ptr, int adqdsp_num, int addr, int mask, int data);
DLL_IMPORT unsigned int        ADQDSP_ReadRegister(void* adq_cu_ptr, int adqdsp_num, int addr);
DLL_IMPORT int                ADQDSP_WaitForPCIeDMAFinish(void* adq_cu_ptr, int adqdsp_num, unsigned int length);
DLL_IMPORT int                ADQDSP_GetDSPDataNowait(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT int                ADQDSP_GetDSPData(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT int                ADQDSP_SetSendLength(void* adq_cu_ptr, int adqdsp_num, unsigned int length);
DLL_IMPORT unsigned int        ADQDSP_GetSendLength(void* adq_cu_ptr, int adqdsp_num);
DLL_EXPORT unsigned long       ADQDSP_GetPhysicalAddress(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT unsigned int*       ADQDSP_GetPtrData(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT unsigned int        ADQDSP_GetTemperature(void* adq_cu_ptr, int adqdsp_num, unsigned int addr);
DLL_IMPORT int                ADQDSP_InitTransfer(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT int                ADQDSP_WriteToDataEP(void* adq_cu_ptr, int adqdsp_num,unsigned int *pData, unsigned int length);
DLL_IMPORT unsigned int        ADQDSP_GetADQType(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT unsigned int        ADQDSP_TrigOutEn(void* adq_cu_ptr, int adqdsp_num, unsigned int en);
DLL_IMPORT unsigned int        ADQDSP_GetLastError(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT int                ADQDSP_SetTransferTimeout(void* adq_cu_ptr, int adqdsp_num, unsigned int TimeoutValue);
DLL_IMPORT int                ADQDSP_SetTransferBuffers(void* adq_cu_ptr, int adqdsp_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT unsigned int        ADQDSP_GetTransferBufferStatus(void* adq_cu_ptr, int adqdsp_num, unsigned int* filled);
DLL_IMPORT const char*        ADQDSP_GetNGCPartNumber(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT const char*        ADQDSP_GetUserLogicPartNumber(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT unsigned int        ADQDSP_GetPCIeLinkWidth(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT unsigned int        ADQDSP_GetPCIeLinkRate(void* adq_cu_ptr, int adqdsp_num);
DLL_IMPORT int                ADQDSP_Blink(void* adq_cu_ptr, int ADQDSP_num);
DLL_IMPORT unsigned int        ADQDSP_WriteUserRegister(void* adq_cu_ptr, int ADQDSP_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int        ADQDSP_ReadUserRegister(void* adq_cu_ptr, int ADQDSP_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT const char* ADQDSP_GetADQDSPOption(void* adq_cu_ptr, int ADQDSP_num);
DLL_IMPORT unsigned int ADQDSP_GetProductFamily(void* adq_cu_ptr, int ADQDSP_num, unsigned int* family);
DLL_IMPORT char* ADQDSP_GetBoardSerialNumber(void* adq_cu_ptr, int ADQDSP_num);


#ifdef __cplusplus
}
#endif


#define INCLUDE_ADQ108

#ifdef __cplusplus
extern "C" {
#endif

DLL_IMPORT int           ADQ108_ADCCalibrate(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int        ADQ108_ResetDevice(void* adq_cu_ptr, int adq108_num, int resetlevel);
DLL_IMPORT int                 ADQ108_GetData(void* adq_cu_ptr, int adq108_num, void** target_buffers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ108_GetDataWH(void* adq_cu_ptr, int adq108_num, void** target_buffers, void* target_headers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ108_GetDataWHTS(void* adq_cu_ptr, int adq108_num, void** target_buffers, void* target_headers, void* target_timestamps, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int            ADQ108_SetTransferTimeout(void* adq_cu_ptr, int adq108_num, unsigned int TimeoutValue);
DLL_IMPORT int            ADQ108_SetTransferBuffers(void* adq_cu_ptr, int adq108_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT unsigned int    ADQ108_GetTransferBufferStatus(void* adq_cu_ptr, int adq108_num, unsigned int* filled);
DLL_IMPORT int            ADQ108_SetDataFormat(void* adq_cu_ptr, int adq108_num, unsigned int format);
DLL_IMPORT unsigned int    ADQ108_GetDataFormat(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_GetStreamStatus(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_SetStreamStatus(void* adq_cu_ptr, int adq108_num, unsigned int status);
DLL_IMPORT unsigned int    ADQ108_GetStreamOverflow(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT void*          ADQ108_GetPtrStream(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_InvalidateCache(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_SetCacheSize(void* adq_cu_ptr, int adq108_num, unsigned int newCacheSize);
DLL_IMPORT int            ADQ108_SetLvlTrigLevel(void* adq_cu_ptr, int adq108_num, int level);
DLL_IMPORT int            ADQ108_SetLvlTrigFlank(void* adq_cu_ptr, int adq108_num, int flank);
DLL_IMPORT int            ADQ108_SetLvlTrigEdge(void* adq_cu_ptr, int adq108_num, int edge);
DLL_IMPORT unsigned int        ADQ108_SetTrigLevelResetValue(void* adq_cu_ptr, int adq108_num, int resetlevel);
DLL_IMPORT int            ADQ108_SetTriggerMode(void* adq_cu_ptr, int adq108_num, int trig_mode);
DLL_IMPORT int            ADQ108_SetSampleWidth(void* adq_cu_ptr, int adq108_num, unsigned int NofBits);
DLL_IMPORT int            ADQ108_SetNofBits(void* adq_cu_ptr, int adq108_num, unsigned int NofBits);
DLL_IMPORT int            ADQ108_SetBufferSizePages(void* adq_cu_ptr, int adq108_num, unsigned int pages);
DLL_IMPORT int            ADQ108_SetBufferSize(void* adq_cu_ptr, int adq108_num, unsigned int samples);
DLL_IMPORT unsigned int    ADQ108_SetExternTrigEdge(void* adq_cu_ptr, int adq108_num, unsigned int edge);
DLL_IMPORT unsigned int    ADQ108_GetExternTrigEdge(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_ArmTrigger(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_DisarmTrigger(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_USBTrig(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_SWTrig(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_CollectDataNextPage(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_CollectRecord(void* adq_cu_ptr, int adq108_num, unsigned int record_num);
DLL_IMPORT unsigned int     ADQ108_GetErrorVector(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT char*        ADQ108_GetPtrData(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_GetWaitingForTrigger(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_GetTrigged(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_GetTriggedAll(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int              ADQ108_GetAcquired(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int      ADQ108_GetAcquiredRecords(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int      ADQ108_GetAcquiredRecordsAndLoopCounter(void* adq_cu_ptr, int adq108_num, unsigned int* acquired_records, unsigned int* loop_counter);
DLL_IMPORT int              ADQ108_GetAcquiredAll(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_GetLvlTrigLevel(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_GetLvlTrigFlank(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_GetLvlTrigEdge(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_GetClockSource(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_GetTriggerMode(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned long long  ADQ108_GetMaxBufferSize(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_GetMaxBufferSizePages(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int*        ADQ108_GetRevision(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_WriteRegister(void* adq_cu_ptr, int ADQ108_num, int addr, int mask, int data);
DLL_IMPORT unsigned int    ADQ108_ReadRegister(void* adq_cu_ptr, int ADQ108_num, int addr);
DLL_IMPORT unsigned int    ADQ108_MemoryDump(void* adq_cu_ptr, int ADQ108_num,  unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT unsigned int    ADQ108_MemoryShadow(void* adq_cu_ptr, int ADQ108_num, void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT unsigned int    ADQ108_MultiRecordSetup(void* adq_cu_ptr, int adq108_num,int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int    ADQ108_MultiRecordSetupGP(void* adq_cu_ptr, int adq108_num,int NumberOfRecords, int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT unsigned int    ADQ108_MultiRecordClose(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_GetSamplesPerPage(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_GetUSBAddress(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_GetPCIeAddress(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_GetTemperature(void* adq_cu_ptr, int adq108_num, unsigned int addr);
DLL_IMPORT unsigned int    ADQ108_WriteEEPROM(void* adq_cu_ptr, int adq108_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int    ADQ108_ReadEEPROM(void* adq_cu_ptr, int adq108_num, unsigned int addr);
DLL_IMPORT unsigned int    ADQ108_WriteEEPROMDB(void* adq_cu_ptr, int adq108_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int    ADQ108_ReadEEPROMDB(void* adq_cu_ptr, int adq108_num, unsigned int addr);
DLL_IMPORT int            ADQ108_PllReg(void* adq_cu_ptr, int adq108_num, unsigned int reg_addr, unsigned char val, unsigned char mask);
DLL_IMPORT int            ADQ108_SetPll(void* adq_cu_ptr, int adq108_num, int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT int            ADQ108_ADCReg(void* adq_cu_ptr, int adq108_num, unsigned char addr, unsigned char adc, unsigned int val);
DLL_IMPORT int            ADQ108_SetPreTrigSamples(void* adq_cu_ptr, int adq108_num, unsigned int PreTrigSamples);
DLL_IMPORT int            ADQ108_SetTriggerHoldOffSamples(void* adq_cu_ptr, int adq108_num, unsigned int TriggerHoldOffSamples);
DLL_IMPORT int            ADQ108_ResetOverheat(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_SetClockSource(void* adq_cu_ptr, int adq108_num, int source);
DLL_IMPORT char*        ADQ108_GetBoardSerialNumber(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_GetLastError(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_EnableClockRefOut(void* adq_cu_ptr, int ADQ108_num, unsigned char enable);
DLL_IMPORT unsigned int    ADQ108_RegisterNameLookup(void* adq_cu_ptr, int adq108_num, const char* rn, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT unsigned int    ADQ108_ReadGPIO(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_WriteGPIO(void* adq_cu_ptr, int adq108_num, unsigned int data, unsigned int mask);
DLL_IMPORT int            ADQ108_SetDirectionGPIO(void* adq_cu_ptr, int adq108_num, unsigned int direction, unsigned int mask);
DLL_IMPORT unsigned int    ADQ108_GetOutputWidth(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_GetNofChannels(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT int            ADQ108_SetDirectionTrig(void* adq_cu_ptr, int adq108_num, int direction);
DLL_IMPORT int            ADQ108_WriteTrig(void* adq_cu_ptr, int adq108_num, int data);
DLL_IMPORT unsigned int       ADQ108_SetInternalTriggerPeriod(void* adq_cu_ptr, int ADQ108_num, unsigned int TriggerPeriodClockCycles);
DLL_IMPORT unsigned int        ADQ108_SetTestPatternMode(void* adq_cu_ptr, int adq108_num, int mode);
DLL_IMPORT unsigned int        ADQ108_SetTestPatternConstant(void* adq_cu_ptr, int adq108_num, int value);
DLL_IMPORT unsigned int    ADQ108_SetEthernetPllFreq(void* adq_cu_ptr, int ADQ108_num, unsigned char eth10_freq, unsigned char eth1_freq);
DLL_IMPORT unsigned int    ADQ108_SetPointToPointPllFreq(void* adq_cu_ptr, int ADQ108_num, unsigned char pp_freq);
DLL_IMPORT unsigned int    ADQ108_SetEthernetPll(void* adq_cu_ptr, int ADQ108_num, unsigned short refdiv, unsigned char useref2, unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv,
    unsigned char eth10_outdiv, unsigned char eth1_outdiv);
DLL_IMPORT unsigned int    ADQ108_SetPointToPointPll(void* adq_cu_ptr, int ADQ108_num, unsigned short refdiv, unsigned char useref2,  unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv,
    unsigned char pp_outdiv, unsigned char ppsync_outdiv);
DLL_IMPORT unsigned int    ADQ108_SetDirectionMLVDS(void* adq_cu_ptr, int ADQ108_num, unsigned char direction);
DLL_IMPORT unsigned int     ADQ108_StorePCIeConfig(void* adq_cu_ptr, int ADQ108_num, UINT32* pci_space);
DLL_IMPORT unsigned int     ADQ108_ReloadPCIeConfig(void* adq_cu_ptr, int ADQ108_num, UINT32* pci_space);
DLL_IMPORT int            ADQ108_IsUSBDevice(void* adq_cu_ptr, int ADQ108_num);
DLL_IMPORT int            ADQ108_IsPCIeDevice(void* adq_cu_ptr, int ADQ108_num);
DLL_IMPORT const char*        ADQ108_GetNGCPartNumber(void* adq_cu_ptr, int ADQ108_num);
DLL_IMPORT const char*        ADQ108_GetUserLogicPartNumber(void* adq_cu_ptr, int ADQ108_num);
DLL_IMPORT unsigned int        ADQ108_GetPCIeLinkWidth(void* adq_cu_ptr, int ADQ108_num);
DLL_IMPORT unsigned int        ADQ108_GetPCIeLinkRate(void* adq_cu_ptr, int ADQ108_num);
DLL_IMPORT int                 ADQ108_Blink(void* adq_cu_ptr, int ADQ108_num);
DLL_IMPORT unsigned int        ADQ108_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int ADQ108_num, unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT unsigned int        ADQ108_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int ADQ108_num, unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT unsigned int    ADQ108_SPISend(void* adq_cu_ptr, int adq108_num, unsigned char addr, const char* data, unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT int            ADQ108_SetDelayLineValues(void* adq_cu_ptr, int adq108_num, int samplerate, unsigned int linear_interpolation);
DLL_IMPORT unsigned int    ADQ108_GetPPTStatus(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_InitPPT(void* adq_cu_ptr, int adq108_num);
DLL_IMPORT unsigned int    ADQ108_SetPPTActive(void* adq_cu_ptr, int adq108_num, unsigned int active);
DLL_IMPORT unsigned int    ADQ108_SetPPTInitOffset(void* adq_cu_ptr, int adq108_num, unsigned int init_offset);
DLL_IMPORT unsigned int    ADQ108_SetPPTPeriod(void* adq_cu_ptr, int adq108_num, unsigned int period);
DLL_IMPORT unsigned int    ADQ108_SetPPTBurstMode(void* adq_cu_ptr, int adq108_num, unsigned int burst_mode);
DLL_IMPORT unsigned int   ADQ108_WriteUserRegister(void* adq_cu_ptr, int ADQ108_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int   ADQ108_ReadUserRegister(void* adq_cu_ptr, int ADQ108_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT const char* ADQ108_GetADQDSPOption(void* adq_cu_ptr, int ADQ108_num);
DLL_IMPORT unsigned int ADQ108_GetProductFamily(void* adq_cu_ptr, int ADQ108_num, unsigned int* family);
DLL_IMPORT unsigned int ADQ108_SetupLevelTrigger(void* adq_cu_ptr, int ADQ108_num, int* level, int* edge, int* resetlevel, unsigned int channel, unsigned int individual_mode);
DLL_IMPORT unsigned int ADQ108_SetConfigurationTrig(void* adq_cu_ptr, int ADQ108_num, unsigned int mode, unsigned int pulselength, unsigned int invertoutput);
DLL_IMPORT unsigned int ADQ108_GetPCIeTLPSize(void* adq_cu_ptr, int ADQ108_num);

#ifdef __cplusplus
}
#endif


#define ADQ112_DATA_FORMAT_PACKED_12BIT   0
#define ADQ112_DATA_FORMAT_UNPACKED_12BIT 1
#define ADQ112_DATA_FORMAT_UNPACKED_16BIT 2
#define ADQ112_DATA_FORMAT_UNPACKED_32BIT 3

#define ADQ112_STREAM_DISABLED     0
#define ADQ112_STREAM_ENABLED      7

#ifdef __cplusplus
extern "C" {
#endif

DLL_IMPORT unsigned int        ADQ112_SetConfigurationTrig(void* adq_cu_ptr, int adq112_num, unsigned int mode, unsigned int pulselength, unsigned int invertoutput);
DLL_IMPORT unsigned int        ADQ112_SetExternalTriggerDelay(void* adq_cu_ptr, int adq112_num, unsigned char delaycycles);
DLL_IMPORT int                 ADQ112_GetData(void* adq_cu_ptr, int adq112_num, void** target_buffers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ112_GetDataWH(void* adq_cu_ptr, int adq112_num, void** target_buffers, void* target_headers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ112_GetDataWHTS(void* adq_cu_ptr, int adq112_num, void** target_buffers, void* target_headers, void* target_timestamps, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT unsigned int        ADQ112_SetCacheSize(void* adq_cu_ptr, int adq112_num, unsigned int newCacheSize);
DLL_IMPORT int              ADQ112_SetTransferTimeout(void* adq_cu_ptr, int adq112_num, unsigned int TimeoutValue);
DLL_IMPORT int                 ADQ112_SetTransferBuffers(void* adq_cu_ptr, int adq112_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT unsigned int        ADQ112_GetTransferBufferStatus(void* adq_cu_ptr, int adq112_num, unsigned int* filled);
DLL_IMPORT unsigned int        ADQ112_SetLvlTrigResetLevel(void* adq_cu_ptr, int adq112_num, int resetlevel);
DLL_IMPORT unsigned int        ADQ112_SetTrigLevelResetValue(void* adq_cu_ptr, int adq112_num, int resetlevel);
DLL_IMPORT unsigned int        ADQ112_SetSampleSkip(void* adq_cu_ptr, int adq112_num, unsigned int skipfactor);
DLL_IMPORT int              ADQ112_SetLvlTrigLevel(void* adq_cu_ptr, int adq112_num, int level);
DLL_IMPORT int              ADQ112_SetLvlTrigFlank(void* adq_cu_ptr, int adq112_num, int flank);
DLL_IMPORT int              ADQ112_SetLvlTrigEdge(void* adq_cu_ptr, int adq112_num, int edge);
DLL_IMPORT int              ADQ112_SetClockSource(void* adq_cu_ptr, int adq112_num, int source);
DLL_IMPORT int              ADQ112_SetClockFrequencyMode(void* adq_cu_ptr, int adq112_num, int clockmode);
DLL_IMPORT int              ADQ112_SetPllFreqDivider(void* adq_cu_ptr, int adq112_num, int divider);
DLL_IMPORT int              ADQ112_SetPll(void* adq_cu_ptr, int adq112_num, int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT int              ADQ112_SetTriggerMode(void* adq_cu_ptr, int adq112_num, int trig_mode);
DLL_IMPORT int                 ADQ112_SetSampleWidth(void* adq_cu_ptr, int adq112_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ112_SetNofBits(void* adq_cu_ptr, int adq112_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ112_SetPreTrigSamples(void* adq_cu_ptr, int adq112_num, unsigned int PreTrigSamples);
DLL_IMPORT int                 ADQ112_SetTriggerHoldOffSamples(void* adq_cu_ptr, int adq112_num, unsigned int TriggerHoldOffSamples);
DLL_IMPORT int              ADQ112_SetBufferSizePages(void* adq_cu_ptr, int adq112_num, unsigned int pages);
DLL_IMPORT int                 ADQ112_SetBufferSize(void* adq_cu_ptr, int adq112_num, unsigned int samples);
DLL_IMPORT unsigned int        ADQ112_SetExternTrigEdge(void* adq_cu_ptr, int adq112_num, unsigned int edge);
DLL_IMPORT unsigned int        ADQ112_GetExternTrigEdge(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_ArmTrigger(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_DisarmTrigger(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_USBTrig(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_SWTrig(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_CollectDataNextPage(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_CollectRecord(void* adq_cu_ptr, int adq112_num, unsigned int record_num);
DLL_IMPORT unsigned int      ADQ112_GetErrorVector(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int      ADQ112_GetStreamStatus(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int      ADQ112_SetStreamStatus(void* adq_cu_ptr, int adq112_num, unsigned int status);
DLL_IMPORT unsigned int      ADQ112_GetStreamOverflow(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int*            ADQ112_GetPtrData(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT void*               ADQ112_GetPtrStream(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_GetWaitingForTrigger(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_GetTrigged(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_GetTriggedAll(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_GetAcquired(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_GetAcquiredAll(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_GetSampleSkip(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetPageCount(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_GetLvlTrigLevel(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_GetLvlTrigFlank(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_GetLvlTrigEdge(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_GetPllFreqDivider(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_GetClockSource(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_GetTriggerMode(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetBufferSizePages(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetBufferSize(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned long long  ADQ112_GetMaxBufferSize(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetMaxBufferSizePages(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetSamplesPerPage(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetUSBAddress(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetPCIeAddress(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetBcdDevice(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT char*            ADQ112_GetBoardSerialNumber(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int*            ADQ112_GetRevision(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_GetTriggerInformation(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_GetTrigPoint(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetTrigType(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_GetOverflow(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                ADQ112_GetRecordSize(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int               ADQ112_GetNofRecords(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_ResetDevice(void* adq_cu_ptr, int adq112_num, int resetlevel);
DLL_IMPORT int              ADQ112_IsUSBDevice(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int              ADQ112_IsPCIeDevice(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_IsAlive(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_SendProcessorCommand(void* adq_cu_ptr, int adq112_num, int command, int argument);
DLL_IMPORT unsigned int        ADQ112_SendLongProcessorCommand(void* adq_cu_ptr, int adq112_num, int command, int addr, int mask, int data);
DLL_IMPORT unsigned int        ADQ112_WriteRegister(void* adq_cu_ptr, int adq112_num, int addr, int mask, int data);
DLL_IMPORT unsigned int        ADQ112_ReadRegister(void* adq_cu_ptr, int adq112_num, int addr);
DLL_IMPORT unsigned int        ADQ112_WriteAlgoRegister(void* adq_cu_ptr, int adq112_num, int addr, int data);
DLL_IMPORT unsigned int        ADQ112_ReadAlgoRegister(void* adq_cu_ptr, int adq112_num, int addr);
DLL_IMPORT unsigned int        ADQ112_GetTemperature(void* adq_cu_ptr, int adq112_num, unsigned int addr);
DLL_IMPORT unsigned int        ADQ112_WriteEEPROM(void* adq_cu_ptr, int adq112_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int        ADQ112_ReadEEPROM(void* adq_cu_ptr, int adq112_num, unsigned int addr);
DLL_IMPORT int                ADQ112_SetDataFormat(void* adq_cu_ptr, int adq112_num, unsigned int format);
DLL_IMPORT unsigned int        ADQ112_GetDataFormat(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_MultiRecordSetup(void* adq_cu_ptr, int adq112_num,int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int        ADQ112_MultiRecordSetupGP(void* adq_cu_ptr, int adq112_num,int NumberOfRecords, int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT unsigned int        ADQ112_MultiRecordClose(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_MemoryDump(void* adq_cu_ptr, int adq112_num,  unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT unsigned int        ADQ112_MemoryShadow(void* adq_cu_ptr, int adq112_num, void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT int                 ADQ112_WriteTrig(void* adq_cu_ptr, int adq112_num, int data);
DLL_IMPORT unsigned int        ADQ112_SetTestPatternMode(void* adq_cu_ptr, int adq112_num, int mode);
DLL_IMPORT unsigned int        ADQ112_SetTestPatternConstant(void* adq_cu_ptr, int adq112_num, int value);
DLL_IMPORT int                 ADQ112_SetDirectionTrig(void* adq_cu_ptr, int adq112_num, int direction);
DLL_IMPORT unsigned int        ADQ112_ReadGPIO(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_WriteGPIO(void* adq_cu_ptr, int adq112_num, unsigned int data, unsigned int mask);
DLL_IMPORT int                 ADQ112_SetDirectionGPIO(void* adq_cu_ptr, int adq112_num, unsigned int direction, unsigned int mask);
DLL_IMPORT unsigned int        ADQ112_SetAlgoStatus(void* adq_cu_ptr, int adq112_num, int status);
DLL_IMPORT unsigned int        ADQ112_SetAlgoNyquistBand(void* adq_cu_ptr, int adq112_num, unsigned int band);
DLL_IMPORT unsigned int*       ADQ112_GetMultiRecordHeader(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned long long  ADQ112_GetTrigTimeCycles(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned long long  ADQ112_GetTrigTime(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetTrigTimeSyncs(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetTrigTimeStart(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_SetTrigTimeMode(void* adq_cu_ptr, int adq112_num, int TrigTimeMode);
DLL_IMPORT int                 ADQ112_ResetTrigTimer(void* adq_cu_ptr, int adq112_num, int TrigTimeRestart);
DLL_IMPORT unsigned int        ADQ112_SetGainAndOffset(void* adq_cu_ptr, int adq112_num, unsigned char channel, int Gain, int Offset);
DLL_IMPORT unsigned int        ADQ112_GetGainAndOffset(void* adq_cu_ptr, int adq112_num, unsigned char channel, int* Gain, int* Offset);
DLL_IMPORT unsigned int        ADQ112_SetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int adq112_num, unsigned char ipinst, unsigned int freqcalmode);
DLL_IMPORT unsigned int        ADQ112_GetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int adq112_num, unsigned char ipinst, unsigned int* freqcalmode);
DLL_IMPORT unsigned int        ADQ112_SetInterleavingIPEstimationMode(void* adq_cu_ptr, int adq112_num, unsigned char ipinst, unsigned int updateflag);
DLL_IMPORT unsigned int        ADQ112_GetInterleavingIPEstimationMode(void* adq_cu_ptr, int adq112_num, unsigned char ipinst, unsigned int* updateflag);
DLL_IMPORT unsigned int        ADQ112_SetInterleavingIPBypassMode(void* adq_cu_ptr, int adq112_num, unsigned char ipinst, unsigned int bypassflag);
DLL_IMPORT unsigned int        ADQ112_GetInterleavingIPBypassMode(void* adq_cu_ptr, int adq112_num, unsigned char ipinst, unsigned int* bypassflag);
DLL_IMPORT unsigned int        ADQ112_SetInterleavingIPCalibration(void* adq_cu_ptr, int adq112_num, unsigned char ipinst, unsigned int* calibration);
DLL_IMPORT unsigned int        ADQ112_GetInterleavingIPCalibration(void* adq_cu_ptr, int adq112_num, unsigned char ipinst, unsigned int* calibration);
DLL_IMPORT unsigned int        ADQ112_ResetInterleavingIP(void* adq_cu_ptr, int adq112_num, unsigned char ipinst);
DLL_IMPORT unsigned int        ADQ112_SendIPCommand(void* adq_cu_ptr, int adq112_num, unsigned char ipinst, unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int* answer);
DLL_IMPORT unsigned int        ADQ112_GetLastError(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetExternalClockReferenceStatus(void* adq_cu_ptr, int adq112_num, unsigned int* extrefstatus);
DLL_IMPORT unsigned int        ADQ112_RegisterNameLookup(void* adq_cu_ptr, int adq112_num, const char* rn, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT unsigned int        ADQ112_GetOutputWidth(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_GetNofChannels(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_SetInternalTriggerPeriod(void* adq_cu_ptr, int adq112_num, unsigned int TriggerPeriodClockCycles);
DLL_IMPORT unsigned int        ADQ112_WaveformAveragingSetup(void* adq_cu_ptr, int adq112_num, unsigned int NofWaveforms, unsigned int NofSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int WaveformAveragingFlags);
DLL_IMPORT unsigned int        ADQ112_WaveformAveragingArm(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_WaveformAveragingDisarm(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_WaveformAveragingStartReadout(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT unsigned int        ADQ112_WaveformAveragingGetWaveform(void* adq_cu_ptr, int adq112_num, int* waveform_data);
DLL_IMPORT unsigned int        ADQ112_WaveformAveragingGetStatus(void* adq_cu_ptr, int adq112_num, unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle);
DLL_IMPORT unsigned int        ADQ112_WaveformAveragingShutdown(void* adq_cu_ptr, int adq112_num);
DLL_IMPORT int                 ADQ112_Blink(void* adq_cu_ptr, int ADQ112_num);
DLL_IMPORT unsigned int        ADQ112_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int adq112_num, unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT unsigned int        ADQ112_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int adq112_num, unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT unsigned int        ADQ112_SPISend(void* adq_cu_ptr, int adq112_num, unsigned char addr, const char* data, unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT unsigned int        ADQ112_WriteUserRegister(void* adq_cu_ptr, int ADQ112_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int        ADQ112_ReadUserRegister(void* adq_cu_ptr, int ADQ112_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT unsigned int ADQ112_WaveformAveragingParseDataStream(void* adq_cu_ptr, int ADQ112_num, unsigned int samples_per_record, int* data_stream, int** data_target);
DLL_IMPORT unsigned int ADQ112_GetProductFamily(void* adq_cu_ptr, int ADQ112_num, unsigned int* family);

#ifdef __cplusplus
}
#endif


#define ADQ114_DATA_FORMAT_PACKED_14BIT   0
#define ADQ114_DATA_FORMAT_UNPACKED_14BIT 1
#define ADQ114_DATA_FORMAT_UNPACKED_16BIT 2
#define ADQ114_DATA_FORMAT_UNPACKED_32BIT 3

#define ADQ114_STREAM_DISABLED     0
#define ADQ114_STREAM_ENABLED      7

#ifdef __cplusplus
extern "C" {
#endif

DLL_IMPORT unsigned int        ADQ114_SetConfigurationTrig(void* adq_cu_ptr, int adq114_num, unsigned int mode, unsigned int pulselength, unsigned int invertoutput);
DLL_IMPORT unsigned int        ADQ114_SetExternalTriggerDelay(void* adq_cu_ptr, int adq114_num, unsigned char delaycycles);
DLL_IMPORT int                 ADQ114_GetData(void* adq_cu_ptr, int adq114_num, void** target_buffers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ114_GetDataWH(void* adq_cu_ptr, int adq114_num, void** target_buffers, void* target_headers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ114_GetDataWHTS(void* adq_cu_ptr, int adq114_num, void** target_buffers, void* target_headers, void* target_timestamps, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT unsigned int        ADQ114_SetCacheSize(void* adq_cu_ptr, int adq114_num, unsigned int newCacheSize);
DLL_IMPORT int                ADQ114_SetTransferTimeout(void* adq_cu_ptr, int adq114_num, unsigned int TimeoutValue);
DLL_IMPORT int                ADQ114_SetTransferBuffers(void* adq_cu_ptr, int adq114_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT unsigned int        ADQ114_GetTransferBufferStatus(void* adq_cu_ptr, int adq114_num, unsigned int* filled);
DLL_IMPORT unsigned int        ADQ114_SetLvlTrigResetLevel(void* adq_cu_ptr, int adq114_num, int resetlevel);
DLL_IMPORT unsigned int        ADQ114_SetTrigLevelResetValue(void* adq_cu_ptr, int adq114_num, int resetlevel);
DLL_IMPORT unsigned int        ADQ114_SetSampleSkip(void* adq_cu_ptr, int adq114_num, unsigned int skipfactor);
DLL_IMPORT int              ADQ114_SetLvlTrigLevel(void* adq_cu_ptr, int adq114_num, int level);
DLL_IMPORT int              ADQ114_SetLvlTrigFlank(void* adq_cu_ptr, int adq114_num, int flank);
DLL_IMPORT int              ADQ114_SetLvlTrigEdge(void* adq_cu_ptr, int adq114_num, int edge);
DLL_IMPORT int              ADQ114_SetClockSource(void* adq_cu_ptr, int adq114_num, int source);
DLL_IMPORT int              ADQ114_SetClockFrequencyMode(void* adq_cu_ptr, int adq114_num, int clockmode);
DLL_IMPORT int              ADQ114_SetPllFreqDivider(void* adq_cu_ptr, int adq114_num, int divider);
DLL_IMPORT int              ADQ114_SetPll(void* adq_cu_ptr, int adq114_num, int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT int              ADQ114_SetTriggerMode(void* adq_cu_ptr, int adq114_num, int trig_mode);
DLL_IMPORT int                 ADQ114_SetSampleWidth(void* adq_cu_ptr, int adq114_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ114_SetNofBits(void* adq_cu_ptr, int adq114_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ114_SetPreTrigSamples(void* adq_cu_ptr, int adq114_num, unsigned int PreTrigSamples);
DLL_IMPORT int                 ADQ114_SetTriggerHoldOffSamples(void* adq_cu_ptr, int adq114_num, unsigned int TriggerHoldOffSamples);
DLL_IMPORT int              ADQ114_SetBufferSizePages(void* adq_cu_ptr, int adq114_num, unsigned int pages);
DLL_IMPORT int                 ADQ114_SetBufferSize(void* adq_cu_ptr, int adq114_num, unsigned int samples);
DLL_IMPORT unsigned int        ADQ114_SetExternTrigEdge(void* adq_cu_ptr, int adq114_num, unsigned int edge);
DLL_IMPORT unsigned int        ADQ114_GetExternTrigEdge(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_ArmTrigger(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_DisarmTrigger(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_USBTrig(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_SWTrig(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_CollectDataNextPage(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_CollectRecord(void* adq_cu_ptr, int adq114_num, unsigned int record_num);
DLL_IMPORT unsigned int      ADQ114_GetErrorVector(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_GetStreamStatus(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_SetStreamStatus(void* adq_cu_ptr, int adq114_num, unsigned int status);
DLL_IMPORT unsigned int      ADQ114_GetStreamOverflow(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int*          ADQ114_GetPtrData(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT void*          ADQ114_GetPtrStream(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetWaitingForTrigger(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetTrigged(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetTriggedAll(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetAcquired(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetAcquiredAll(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetSampleSkip(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_GetPageCount(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetLvlTrigLevel(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetLvlTrigFlank(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetLvlTrigEdge(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetPllFreqDivider(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetClockSource(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetTriggerMode(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_GetBufferSizePages(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_GetBufferSize(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned long long  ADQ114_GetMaxBufferSize(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_GetMaxBufferSizePages(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_GetSamplesPerPage(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_GetUSBAddress(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_GetPCIeAddress(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_GetBcdDevice(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT char*          ADQ114_GetBoardSerialNumber(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int*          ADQ114_GetRevision(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetTriggerInformation(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetTrigPoint(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_GetTrigType(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetOverflow(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetRecordSize(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_GetNofRecords(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_ResetDevice(void* adq_cu_ptr, int adq114_num, int resetlevel);
DLL_IMPORT int              ADQ114_IsUSBDevice(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_IsPCIeDevice(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_IsAlive(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_SendProcessorCommand(void* adq_cu_ptr, int adq114_num, int command, int argument);
DLL_IMPORT unsigned int      ADQ114_SendLongProcessorCommand(void* adq_cu_ptr, int adq114_num, int command, int addr, int mask, int data);
DLL_IMPORT unsigned int      ADQ114_WriteRegister(void* adq_cu_ptr, int adq114_num, int addr, int mask, int data);
DLL_IMPORT unsigned int      ADQ114_ReadRegister(void* adq_cu_ptr, int adq114_num, int addr);
DLL_IMPORT unsigned int      ADQ114_WriteAlgoRegister(void* adq_cu_ptr, int adq114_num, int addr, int data);
DLL_IMPORT unsigned int      ADQ114_ReadAlgoRegister(void* adq_cu_ptr, int adq114_num, int addr);
DLL_IMPORT unsigned int      ADQ114_GetTemperature(void* adq_cu_ptr, int adq114_num, unsigned int addr);
DLL_IMPORT unsigned int      ADQ114_WriteEEPROM(void* adq_cu_ptr, int adq114_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int      ADQ114_ReadEEPROM(void* adq_cu_ptr, int adq114_num, unsigned int addr);
DLL_IMPORT int              ADQ114_SetDataFormat(void* adq_cu_ptr, int adq114_num, unsigned int format);
DLL_IMPORT unsigned int      ADQ114_GetDataFormat(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_MultiRecordSetup(void* adq_cu_ptr, int adq114_num,int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int        ADQ114_MultiRecordSetupGP(void* adq_cu_ptr, int adq114_num,int NumberOfRecords, int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT unsigned int        ADQ114_MultiRecordClose(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_MemoryDump(void* adq_cu_ptr, int adq114_num,  unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT unsigned int        ADQ114_MemoryShadow(void* adq_cu_ptr, int adq114_num,  void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT int              ADQ114_WriteTrig(void* adq_cu_ptr, int adq114_num, int data);
DLL_IMPORT unsigned int        ADQ114_SetTestPatternMode(void* adq_cu_ptr, int adq114_num, int mode);
DLL_IMPORT unsigned int        ADQ114_SetTestPatternConstant(void* adq_cu_ptr, int adq114_num, int value);
DLL_IMPORT int              ADQ114_SetDirectionTrig(void* adq_cu_ptr, int adq114_num, int direction);
DLL_IMPORT unsigned int      ADQ114_ReadGPIO(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int              ADQ114_WriteGPIO(void* adq_cu_ptr, int adq114_num, unsigned int data, unsigned int mask);
DLL_IMPORT int              ADQ114_SetDirectionGPIO(void* adq_cu_ptr, int adq114_num, unsigned int direction, unsigned int mask);
DLL_IMPORT unsigned int        ADQ114_SetAlgoStatus(void* adq_cu_ptr, int adq114_num, int status);
DLL_IMPORT unsigned int        ADQ114_SetAlgoNyquistBand(void* adq_cu_ptr, int adq114_num, unsigned int band);
DLL_IMPORT unsigned int*      ADQ114_GetMultiRecordHeader(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned long long  ADQ114_GetTrigTime(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned long long  ADQ114_GetTrigTimeCycles(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_GetTrigTimeSyncs(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_GetTrigTimeStart(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int                 ADQ114_SetTrigTimeMode(void* adq_cu_ptr, int adq114_num, int TrigTimeMode);
DLL_IMPORT int                 ADQ114_ResetTrigTimer(void* adq_cu_ptr, int adq114_num,int TrigTimeRestart);
DLL_IMPORT unsigned int      ADQ114_SetGainAndOffset(void* adq_cu_ptr, int adq114_num, unsigned char channel, int Gain, int Offset);
DLL_IMPORT unsigned int      ADQ114_GetGainAndOffset(void* adq_cu_ptr, int adq114_num, unsigned char channel, int* Gain, int* Offset);
DLL_IMPORT unsigned int        ADQ114_SetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int adq114_num, unsigned char ipinst, unsigned int freqcalmode);
DLL_IMPORT unsigned int        ADQ114_GetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int adq114_num, unsigned char ipinst, unsigned int* freqcalmode);
DLL_IMPORT unsigned int        ADQ114_SetInterleavingIPEstimationMode(void* adq_cu_ptr, int adq114_num, unsigned char ipinst, unsigned int updateflag);
DLL_IMPORT unsigned int        ADQ114_GetInterleavingIPEstimationMode(void* adq_cu_ptr, int adq114_num, unsigned char ipinst, unsigned int* updateflag);
DLL_IMPORT unsigned int        ADQ114_SetInterleavingIPBypassMode(void* adq_cu_ptr, int adq114_num, unsigned char ipinst, unsigned int bypassflag);
DLL_IMPORT unsigned int        ADQ114_GetInterleavingIPBypassMode(void* adq_cu_ptr, int adq114_num, unsigned char ipinst, unsigned int* bypassflag);
DLL_IMPORT unsigned int        ADQ114_SetInterleavingIPCalibration(void* adq_cu_ptr, int adq114_num, unsigned char ipinst, unsigned int* calibration);
DLL_IMPORT unsigned int        ADQ114_GetInterleavingIPCalibration(void* adq_cu_ptr, int adq114_num, unsigned char ipinst, unsigned int* calibration);
DLL_IMPORT unsigned int        ADQ114_SendIPCommand(void* adq_cu_ptr, int adq114_num, unsigned char ipinst, unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int* answer);
DLL_IMPORT unsigned int        ADQ114_ResetInterleavingIP(void* adq_cu_ptr, int adq114_num, unsigned char ipinst);
DLL_IMPORT unsigned int        ADQ114_GetLastError(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_GetExternalClockReferenceStatus(void* adq_cu_ptr, int adq114_num, unsigned int* extrefstatus);
DLL_IMPORT unsigned int        ADQ114_RegisterNameLookup(void* adq_cu_ptr, int adq114_num, const char* rn, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT unsigned int      ADQ114_GetOutputWidth(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_GetNofChannels(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int      ADQ114_SetInternalTriggerPeriod(void* adq_cu_ptr, int adq114_num, unsigned int TriggerPeriodClockCycles);
DLL_IMPORT unsigned int        ADQ114_WaveformAveragingSetup(void* adq_cu_ptr, int adq114_num, unsigned int NofWaveforms, unsigned int NofSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int WaveformAveragingFlags);
DLL_IMPORT unsigned int        ADQ114_WaveformAveragingArm(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_WaveformAveragingDisarm(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_WaveformAveragingStartReadout(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT unsigned int        ADQ114_WaveformAveragingGetWaveform(void* adq_cu_ptr, int adq114_num, int* waveform_data);
DLL_IMPORT unsigned int        ADQ114_WaveformAveragingGetStatus(void* adq_cu_ptr, int adq114_num, unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle);
DLL_IMPORT unsigned int        ADQ114_WaveformAveragingShutdown(void* adq_cu_ptr, int adq114_num);
DLL_IMPORT int                ADQ114_Blink(void* adq_cu_ptr, int ADQ114_num);
DLL_IMPORT unsigned int        ADQ114_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int adq114_num, unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT unsigned int        ADQ114_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int adq114_num, unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT unsigned int        ADQ114_SPISend(void* adq_cu_ptr, int adq114_num, unsigned char addr, const char* data, unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT unsigned int        ADQ114_WriteUserRegister(void* adq_cu_ptr, int ADQ114_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int        ADQ114_ReadUserRegister(void* adq_cu_ptr, int ADQ114_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT unsigned int ADQ114_WaveformAveragingParseDataStream(void* adq_cu_ptr, int ADQ114_num, unsigned int samples_per_record, int* data_stream, int** data_target);
DLL_IMPORT unsigned int ADQ114_GetProductFamily(void* adq_cu_ptr, int ADQ114_num, unsigned int* family);

#ifdef __cplusplus
}
#endif


#define ADQ212_DATA_FORMAT_PACKED_14BIT   0
#define ADQ212_DATA_FORMAT_UNPACKED_14BIT 1
#define ADQ212_DATA_FORMAT_UNPACKED_16BIT 2
#define ADQ212_DATA_FORMAT_UNPACKED_32BIT 3

#define ADQ212_STREAM_DISABLED     0
#define ADQ212_STREAM_ENABLED_BOTH 7
#define ADQ212_STREAM_ENABLED_A    3
#define ADQ212_STREAM_ENABLED_B    5

#ifdef __cplusplus
extern "C" {
#endif

DLL_IMPORT unsigned int        ADQ212_SetConfigurationTrig(void* adq_cu_ptr, int adq212_num, unsigned int mode, unsigned int pulselength, unsigned int invertoutput);
DLL_IMPORT unsigned int        ADQ212_SetExternalTriggerDelay(void* adq_cu_ptr, int adq212_num, unsigned char delaycycles);
DLL_IMPORT int                 ADQ212_GetData(void* adq_cu_ptr, int adq212_num, void** target_buffers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ212_GetDataWH(void* adq_cu_ptr, int adq212_num, void** target_buffers, void* target_headers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ212_GetDataWHTS(void* adq_cu_ptr, int adq212_num, void** target_buffers, void* target_headers, void* target_timestamps, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT unsigned int        ADQ212_SetCacheSize(void* adq_cu_ptr, int adq212_num, unsigned int newCacheSize);
DLL_IMPORT int               ADQ212_SetTransferTimeout(void* adq_cu_ptr, int adq212_num, unsigned int TimeoutValue);
DLL_IMPORT int                 ADQ212_SetTransferBuffers(void* adq_cu_ptr, int adq212_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT unsigned int        ADQ212_GetTransferBufferStatus(void* adq_cu_ptr, int adq212_num, unsigned int* filled);
DLL_IMPORT unsigned int        ADQ212_SetLvlTrigResetLevel(void* adq_cu_ptr, int adq212_num, int resetlevel);
DLL_IMPORT unsigned int        ADQ212_SetTrigLevelResetValue(void* adq_cu_ptr, int adq212_num, int resetlevel);
DLL_IMPORT unsigned int      ADQ212_SetSampleSkip(void* adq_cu_ptr, int adq212_num, unsigned int skipfactor);
DLL_IMPORT int              ADQ212_SetLvlTrigLevel(void* adq_cu_ptr, int adq212_num, int level);
DLL_IMPORT int              ADQ212_SetLvlTrigFlank(void* adq_cu_ptr, int adq212_num, int flank);
DLL_IMPORT int              ADQ212_SetLvlTrigEdge(void* adq_cu_ptr, int adq212_num, int edge);
DLL_IMPORT int              ADQ212_SetLvlTrigChannel(void* adq_cu_ptr, int adq212_num,int channel);
DLL_IMPORT int              ADQ212_SetClockSource(void* adq_cu_ptr, int adq212_num, int source);
DLL_IMPORT int              ADQ212_SetClockFrequencyMode(void* adq_cu_ptr, int adq212_num, int clockmode);
DLL_IMPORT int              ADQ212_SetPllFreqDivider(void* adq_cu_ptr, int adq212_num, int divider);
DLL_IMPORT int              ADQ212_SetPll(void* adq_cu_ptr, int adq212_num, int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT int              ADQ212_SetTriggerMode(void* adq_cu_ptr, int adq212_num, int trig_mode);
DLL_IMPORT int                 ADQ212_SetSampleWidth(void* adq_cu_ptr, int adq212_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ212_SetNofBits(void* adq_cu_ptr, int adq212_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ212_SetPreTrigSamples(void* adq_cu_ptr, int adq212_num, unsigned int PreTrigSamples);
DLL_IMPORT int                 ADQ212_SetTriggerHoldOffSamples(void* adq_cu_ptr, int adq212_num, unsigned int TriggerHoldOffSamples);
DLL_IMPORT int              ADQ212_SetBufferSizePages(void* adq_cu_ptr, int adq212_num, unsigned int pages);
DLL_IMPORT int                 ADQ212_SetBufferSize(void* adq_cu_ptr, int adq212_num, unsigned int samples);
DLL_IMPORT unsigned int      ADQ212_SetExternTrigEdge(void* adq_cu_ptr, int adq212_num, unsigned int edge);
DLL_IMPORT unsigned int      ADQ212_GetExternTrigEdge(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_ArmTrigger(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_DisarmTrigger(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_USBTrig(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_SWTrig(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_CollectDataNextPage(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_CollectRecord(void* adq_cu_ptr, int adq212_num, unsigned int record_num);
DLL_IMPORT unsigned int      ADQ212_GetErrorVector(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetStreamStatus(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_SetStreamStatus(void* adq_cu_ptr, int adq212_num, unsigned int status);
DLL_IMPORT int              ADQ212_GetStreamOverflow(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int*          ADQ212_GetPtrDataChA(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int*          ADQ212_GetPtrDataChB(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT void*          ADQ212_GetPtrStream(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetWaitingForTrigger(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetTrigged(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetTriggedAll(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetAcquired(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetAcquiredAll(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetSampleSkip(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetPageCount(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetLvlTrigLevel(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetLvlTrigFlank(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetLvlTrigEdge(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetLvlTrigChannel(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetPllFreqDivider(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetClockSource(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetTriggerMode(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetBufferSizePages(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int        ADQ212_GetBufferSize(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned long long  ADQ212_GetMaxBufferSize(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int        ADQ212_GetMaxBufferSizePages(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int        ADQ212_GetSamplesPerPage(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetUSBAddress(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetPCIeAddress(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetBcdDevice(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT char*          ADQ212_GetBoardSerialNumber(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int*          ADQ212_GetRevision(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetTriggerInformation(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetTrigPoint(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetTrigType(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetTriggedCh(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_GetOverflow(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetRecordSize(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetNofRecords(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_ResetDevice(void* adq_cu_ptr, int adq212_num, int resetlevel);
DLL_IMPORT int              ADQ212_IsUSBDevice(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_IsPCIeDevice(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_IsAlive(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_SendProcessorCommand(void* adq_cu_ptr, int adq212_num, int command, int argument);
DLL_IMPORT unsigned int      ADQ212_SendLongProcessorCommand(void* adq_cu_ptr, int adq212_num, int command, int addr, int mask, int data);
DLL_IMPORT unsigned int      ADQ212_WriteRegister(void* adq_cu_ptr, int ADQ212_num, int addr, int mask, int data);
DLL_IMPORT unsigned int      ADQ212_ReadRegister(void* adq_cu_ptr, int ADQ212_num, int addr);
DLL_IMPORT unsigned int      ADQ212_WriteAlgoRegister(void* adq_cu_ptr, int adq212_num, int addr, int data);
DLL_IMPORT unsigned int      ADQ212_ReadAlgoRegister(void* adq_cu_ptr, int adq212_num, int addr);
DLL_IMPORT unsigned int      ADQ212_GetTemperature(void* adq_cu_ptr, int adq212_num, unsigned int addr);
DLL_IMPORT unsigned int      ADQ212_WriteEEPROM(void* adq_cu_ptr, int adq212_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int      ADQ212_ReadEEPROM(void* adq_cu_ptr, int adq212_num, unsigned int addr);
DLL_IMPORT int              ADQ212_SetDataFormat(void* adq_cu_ptr, int adq212_num, unsigned int format);
DLL_IMPORT unsigned int      ADQ212_GetDataFormat(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int        ADQ212_MultiRecordSetup(void* adq_cu_ptr, int adq212_num,int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int        ADQ212_MultiRecordSetupGP(void* adq_cu_ptr, int adq212_num,int NumberOfRecords, int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT unsigned int        ADQ212_MultiRecordClose(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int        ADQ212_MemoryDump(void* adq_cu_ptr, int adq212_num,  unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT unsigned int        ADQ212_MemoryShadow(void* adq_cu_ptr, int adq212_num,  void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT int              ADQ212_WriteTrig(void* adq_cu_ptr, int adq212_num, int data);
DLL_IMPORT unsigned int        ADQ212_SetTestPatternMode(void* adq_cu_ptr, int adq212_num, int mode);
DLL_IMPORT unsigned int        ADQ212_SetTestPatternConstant(void* adq_cu_ptr, int adq212_num, int value);
DLL_IMPORT int                ADQ212_SetDirectionTrig(void* adq_cu_ptr, int adq212_num, int direction);
DLL_IMPORT unsigned int      ADQ212_ReadGPIO(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_WriteGPIO(void* adq_cu_ptr, int adq212_num, unsigned int data, unsigned int mask);
DLL_IMPORT int              ADQ212_SetDirectionGPIO(void* adq_cu_ptr, int adq212_num, unsigned int direction, unsigned int mask);
DLL_IMPORT unsigned int*      ADQ212_GetMultiRecordHeader(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned long long  ADQ212_GetTrigTime(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned long long  ADQ212_GetTrigTimeCycles(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetTrigTimeSyncs(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int      ADQ212_GetTrigTimeStart(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int              ADQ212_SetTrigTimeMode(void* adq_cu_ptr, int adq212_num, int TrigTimeMode);
DLL_IMPORT int              ADQ212_ResetTrigTimer(void* adq_cu_ptr, int adq212_num,int TrigTimeRestart);
DLL_IMPORT int              ADQ212_SetAfeSwitch(void* adq_cu_ptr, int adq212_num, unsigned int afemask);
DLL_IMPORT unsigned int        ADQ212_GetAfeSwitch(void* adq_cu_ptr, int adq214_num, unsigned char channel, unsigned char* afe);
DLL_IMPORT unsigned int      ADQ212_SetGainAndOffset(void* adq_cu_ptr, int adq212_num, unsigned char channel, int Gain, int Offset);
DLL_IMPORT unsigned int      ADQ212_GetGainAndOffset(void* adq_cu_ptr, int adq212_num, unsigned char channel, int* Gain, int* Offset);
DLL_IMPORT unsigned int        ADQ212_GetLastError(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT unsigned int        ADQ212_GetExternalClockReferenceStatus(void* adq_cu_ptr, int adq212_num, unsigned int* extrefstatus);
DLL_IMPORT unsigned int        ADQ212_RegisterNameLookup(void* adq_cu_ptr, int adq212_num, const char* rn, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT unsigned int      ADQ212_GetOutputWidth(void* adq_cu_ptr, int ADQ212_num);
DLL_IMPORT unsigned int      ADQ212_GetNofChannels(void* adq_cu_ptr, int ADQ212_num);
DLL_IMPORT unsigned int      ADQ212_SetInternalTriggerPeriod(void* adq_cu_ptr, int ADQ212_num, unsigned int TriggerPeriodClockCycles);
DLL_IMPORT unsigned int        ADQ212_WaveformAveragingSetup(void* adq_cu_ptr, int ADQ212_num, unsigned int NofWaveforms, unsigned int NofSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int WaveformAveragingFlags);
DLL_IMPORT unsigned int        ADQ212_WaveformAveragingArm(void* adq_cu_ptr, int ADQ212_num);
DLL_IMPORT unsigned int        ADQ212_WaveformAveragingDisarm(void* adq_cu_ptr, int ADQ212_num);
DLL_IMPORT unsigned int        ADQ212_WaveformAveragingStartReadout(void* adq_cu_ptr, int ADQ212_num);
DLL_IMPORT unsigned int        ADQ212_WaveformAveragingGetWaveform(void* adq_cu_ptr, int ADQ212_num, int* waveform_data);
DLL_IMPORT unsigned int        ADQ212_WaveformAveragingGetStatus(void* adq_cu_ptr, int ADQ212_num, unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle);
DLL_IMPORT unsigned int        ADQ212_WaveformAveragingShutdown(void* adq_cu_ptr, int adq212_num);
DLL_IMPORT int                 ADQ212_Blink(void* adq_cu_ptr, int ADQ212_num);
DLL_IMPORT unsigned int        ADQ212_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int adq212_num, unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT unsigned int        ADQ212_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int adq212_num, unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT unsigned int        ADQ212_SPISend(void* adq_cu_ptr, int adq212_num, unsigned char addr, const char* data, unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT unsigned int        ADQ212_WriteUserRegister(void* adq_cu_ptr, int ADQ212_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int        ADQ212_ReadUserRegister(void* adq_cu_ptr, int ADQ212_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT unsigned int ADQ212_WaveformAveragingParseDataStream(void* adq_cu_ptr, int ADQ212_num, unsigned int samples_per_record, int* data_stream, int** data_target);
DLL_IMPORT unsigned int ADQ212_GetProductFamily(void* adq_cu_ptr, int ADQ212_num, unsigned int* family);

#ifdef __cplusplus
}
#endif


#define ADQ214_DATA_FORMAT_PACKED_14BIT   0
#define ADQ214_DATA_FORMAT_UNPACKED_14BIT 1
#define ADQ214_DATA_FORMAT_UNPACKED_16BIT 2
#define ADQ214_DATA_FORMAT_UNPACKED_32BIT 3

#define ADQ214_STREAM_DISABLED     0
#define ADQ214_STREAM_ENABLED_BOTH 7
#define ADQ214_STREAM_ENABLED_A    3
#define ADQ214_STREAM_ENABLED_B    5

#ifdef __cplusplus
extern "C" {
#endif

DLL_IMPORT unsigned int        ADQ214_SetConfigurationTrig(void* adq_cu_ptr, int adq214_num, unsigned int mode, unsigned int pulselength, unsigned int invertoutput);
DLL_IMPORT unsigned int        ADQ214_SetExternalTriggerDelay(void* adq_cu_ptr, int adq214_num, unsigned char delaycycles);
DLL_IMPORT int                 ADQ214_GetData(void* adq_cu_ptr, int adq214_num, void** target_buffers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ214_GetDataWH(void* adq_cu_ptr, int adq214_num, void** target_buffers, void* target_headers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ214_GetDataWHTS(void* adq_cu_ptr, int adq214_num, void** target_buffers, void* target_headers, void* target_timestamps, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT unsigned int        ADQ214_SetCacheSize(void* adq_cu_ptr, int adq214_num, unsigned int newCacheSize);
DLL_IMPORT int                ADQ214_SetTransferTimeout(void* adq_cu_ptr, int adq214_num, unsigned int TimeoutValue);
DLL_IMPORT int                 ADQ214_SetTransferBuffers(void* adq_cu_ptr, int adq214_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT unsigned int        ADQ214_GetTransferBufferStatus(void* adq_cu_ptr, int adq214_num, unsigned int* filled);
DLL_IMPORT unsigned int        ADQ214_SetLvlTrigResetLevel(void* adq_cu_ptr, int adq214_num, int resetlevel);
DLL_IMPORT unsigned int        ADQ214_SetTrigLevelResetValue(void* adq_cu_ptr, int adq214_num, int resetlevel);
DLL_IMPORT unsigned int      ADQ214_SetSampleSkip(void* adq_cu_ptr, int adq214_num, unsigned int skipfactor);
DLL_IMPORT unsigned int      ADQ214_SetSampleDecimation(void* adq_cu_ptr, int adq214_num, unsigned int decimationfactor);
DLL_IMPORT int              ADQ214_SetLvlTrigLevel(void* adq_cu_ptr, int adq214_num, int level);
DLL_IMPORT int              ADQ214_SetLvlTrigFlank(void* adq_cu_ptr, int adq214_num, int flank);
DLL_IMPORT int              ADQ214_SetLvlTrigEdge(void* adq_cu_ptr, int adq214_num, int edge);
DLL_IMPORT int              ADQ214_SetLvlTrigChannel(void* adq_cu_ptr, int adq214_num,int channel);
DLL_IMPORT int              ADQ214_SetClockSource(void* adq_cu_ptr, int adq214_num, int source);
DLL_IMPORT int              ADQ214_SetClockFrequencyMode(void* adq_cu_ptr, int adq214_num, int clockmode);
DLL_IMPORT int              ADQ214_SetPllFreqDivider(void* adq_cu_ptr, int adq214_num, int divider);
DLL_IMPORT int              ADQ214_SetPll(void* adq_cu_ptr, int adq214_num, int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT int              ADQ214_SetTriggerMode(void* adq_cu_ptr, int adq214_num, int trig_mode);
DLL_IMPORT int                 ADQ214_SetSampleWidth(void* adq_cu_ptr, int adq214_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ214_SetNofBits(void* adq_cu_ptr, int adq214_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ214_SetPreTrigSamples(void* adq_cu_ptr, int adq214_num, unsigned int PreTrigSamples);
DLL_IMPORT int                 ADQ214_SetTriggerHoldOffSamples(void* adq_cu_ptr, int adq214_num, unsigned int TriggerHoldOffSamples);
DLL_IMPORT int              ADQ214_SetBufferSizePages(void* adq_cu_ptr, int adq214_num, unsigned int pages);
DLL_IMPORT int                 ADQ214_SetBufferSize(void* adq_cu_ptr, int adq214_num, unsigned int samples);
DLL_IMPORT unsigned int      ADQ214_SetExternTrigEdge(void* adq_cu_ptr, int adq214_num, unsigned int edge);
DLL_IMPORT unsigned int      ADQ214_GetExternTrigEdge(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_ArmTrigger(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_DisarmTrigger(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_USBTrig(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_SWTrig(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_CollectDataNextPage(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_CollectRecord(void* adq_cu_ptr, int adq214_num, unsigned int record_num);
DLL_IMPORT unsigned int      ADQ214_GetErrorVector(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetStreamStatus(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_SetStreamStatus(void* adq_cu_ptr, int adq214_num, unsigned int status);
DLL_IMPORT int              ADQ214_GetStreamOverflow(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int*          ADQ214_GetPtrDataChA(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int*          ADQ214_GetPtrDataChB(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT void*          ADQ214_GetPtrStream(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetWaitingForTrigger(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetTrigged(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetTriggedAll(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetAcquired(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetAcquiredAll(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetSampleSkip(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetSampleDecimation(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetPageCount(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetLvlTrigLevel(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetLvlTrigFlank(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetLvlTrigEdge(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetLvlTrigChannel(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetPllFreqDivider(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetClockSource(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetTriggerMode(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetBufferSizePages(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_GetBufferSize(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned long long  ADQ214_GetMaxBufferSize(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_GetMaxBufferSizePages(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_GetSamplesPerPage(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetUSBAddress(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetPCIeAddress(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetBcdDevice(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT char*          ADQ214_GetBoardSerialNumber(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int*          ADQ214_GetRevision(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetTriggerInformation(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetTrigPoint(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetTrigType(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetTriggedCh(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_GetOverflow(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetRecordSize(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetNofRecords(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_ResetDevice(void* adq_cu_ptr, int adq214_num, int resetlevel);
DLL_IMPORT int              ADQ214_IsUSBDevice(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_IsPCIeDevice(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_IsAlive(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_SendProcessorCommand(void* adq_cu_ptr, int adq214_num, int command, int argument);
DLL_IMPORT unsigned int      ADQ214_SendLongProcessorCommand(void* adq_cu_ptr, int adq214_num, int command, int addr, int mask, int data);
DLL_IMPORT unsigned int      ADQ214_WriteRegister(void* adq_cu_ptr, int ADQ214_num, int addr, int mask, int data);
DLL_IMPORT unsigned int      ADQ214_ReadRegister(void* adq_cu_ptr, int ADQ214_num, int addr);
DLL_IMPORT unsigned int      ADQ214_WriteAlgoRegister(void* adq_cu_ptr, int adq214_num, int addr, int mask, int data);
DLL_IMPORT unsigned int      ADQ214_ReadAlgoRegister(void* adq_cu_ptr, int adq214_num, int addr);
DLL_IMPORT unsigned int      ADQ214_GetTemperature(void* adq_cu_ptr, int adq214_num, unsigned int addr);
DLL_IMPORT unsigned int      ADQ214_WriteEEPROM(void* adq_cu_ptr, int adq214_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int      ADQ214_ReadEEPROM(void* adq_cu_ptr, int adq214_num, unsigned int addr);
DLL_IMPORT int              ADQ214_SetDataFormat(void* adq_cu_ptr, int adq214_num, unsigned int format);
DLL_IMPORT unsigned int      ADQ214_GetDataFormat(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_MultiRecordSetup(void* adq_cu_ptr, int adq214_num,int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int        ADQ214_MultiRecordSetupGP(void* adq_cu_ptr, int adq214_num,int NumberOfRecords, int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT unsigned int        ADQ214_GetDataMultiRecordSetup(void* adq_cu_ptr, int adq214_num,int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int        ADQ214_MultiRecordClose(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_MemoryDump(void* adq_cu_ptr, int adq214_num,  unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT unsigned int        ADQ214_MemoryShadow(void* adq_cu_ptr, int adq214_num,  void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT int              ADQ214_WriteTrig(void* adq_cu_ptr, int adq214_num, int data);
DLL_IMPORT unsigned int        ADQ214_SetTestPatternMode(void* adq_cu_ptr, int adq214_num, int mode);
DLL_IMPORT unsigned int        ADQ214_SetTestPatternConstant(void* adq_cu_ptr, int adq214_num, int value);
DLL_IMPORT int              ADQ214_SetDirectionTrig(void* adq_cu_ptr, int adq214_num, int direction);
DLL_IMPORT unsigned int      ADQ214_ReadGPIO(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int              ADQ214_WriteGPIO(void* adq_cu_ptr, int adq214_num, unsigned int data, unsigned int mask);
DLL_IMPORT int              ADQ214_SetDirectionGPIO(void* adq_cu_ptr, int adq214_num, unsigned int direction, unsigned int mask);
DLL_IMPORT unsigned int*      ADQ214_GetMultiRecordHeader(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned long long  ADQ214_GetTrigTime(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned long long  ADQ214_GetTrigTimeCycles(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetTrigTimeSyncs(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetTrigTimeStart(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int            ADQ214_SetTrigTimeMode(void* adq_cu_ptr, int adq214_num, int TrigTimeMode);
DLL_IMPORT int            ADQ214_ResetTrigTimer(void* adq_cu_ptr, int adq214_num,int TrigTimeRestart);
DLL_IMPORT int            ADQ214_SetAfeSwitch(void* adq_cu_ptr, int adq214_num, unsigned int afemask);
DLL_IMPORT unsigned int      ADQ214_GetAfeSwitch(void* adq_cu_ptr, int adq214_num, unsigned char channel, unsigned char* afe);
DLL_IMPORT unsigned int      ADQ214_SetGainAndOffset(void* adq_cu_ptr, int adq212_num, unsigned char channel, int Gain, int Offset);
DLL_IMPORT unsigned int      ADQ214_GetGainAndOffset(void* adq_cu_ptr, int adq212_num, unsigned char channel, int* Gain, int* Offset);
DLL_IMPORT unsigned int      ADQ214_GetLastError(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetExternalClockReferenceStatus(void* adq_cu_ptr, int adq214_num, unsigned int* extrefstatus);
DLL_IMPORT unsigned int      ADQ214_RegisterNameLookup(void* adq_cu_ptr, int adq214_num, const char* rn, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT unsigned int      ADQ214_GetOutputWidth(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetNofChannels(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_GetComFlashEnableBit(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_RebootCOMFPGAFromSecondaryImage(void* adq_cu_ptr, int adq214_num, unsigned int PCIeAddress, unsigned int PromAddress);
DLL_IMPORT unsigned int      ADQ214_RebootALGFPGAFromPrimaryImage(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int      ADQ214_ProcessorFlashControl(void* adq_cu_ptr, int adq214_num, unsigned char cmd, unsigned int data);
DLL_IMPORT unsigned int      ADQ214_SetInternalTriggerPeriod(void* adq_cu_ptr, int ADQ214_num, unsigned int TriggerPeriodClockCycles);
DLL_IMPORT unsigned int      ADQ214_SetInternalTriggerFrequency(void* adq_cu_ptr, int ADQ214_num, unsigned int Int_Trig_Freq);
DLL_IMPORT unsigned int      ADQ214_EnableInternalTriggerCounts(void* adq_cu_ptr, int ADQ214_num);
DLL_IMPORT unsigned int      ADQ214_DisableInternalTriggerCounts(void* adq_cu_ptr, int ADQ214_num);
DLL_IMPORT unsigned int      ADQ214_ClearInternalTriggerCounts(void* adq_cu_ptr, int ADQ214_num);
DLL_IMPORT unsigned int      ADQ214_SetInternalTriggerCounts(void* adq_cu_ptr, int ADQ214_num, unsigned int trigger_counts);
DLL_IMPORT unsigned int      ADQ214_ConfigureDebugCounter(void* adq_cu_ptr, int ADQ214_num, unsigned int direction, unsigned int output_mode, unsigned int counter_mode, int initial_value);
DLL_IMPORT unsigned int        ADQ214_WaveformAveragingSetup(void* adq_cu_ptr, int adq214_num, unsigned int NofWaveforms, unsigned int NofSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int WaveformAveragingFlags);
DLL_IMPORT unsigned int        ADQ214_WaveformAveragingArm(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_WaveformAveragingDisarm(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_WaveformAveragingStartReadout(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_WaveformAveragingGetWaveform(void* adq_cu_ptr, int adq214_num, int* waveform_data);
DLL_IMPORT unsigned int        ADQ214_WaveformAveragingGetStatus(void* adq_cu_ptr, int adq214_num, unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle);
DLL_IMPORT unsigned int        ADQ214_WaveformAveragingShutdown(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_TriggeredStreamingOneChannelSetup(void* adq_cu_ptr, int ADQ214_num, unsigned int SamplePerRecord, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int ArmMode, unsigned int ReadOutSpeed, unsigned int Channel);
DLL_IMPORT unsigned int        ADQ214_TriggeredStreamingSetupV5(void* adq_cu_ptr, int ADQ214_num, unsigned int SamplePerRecord, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int TriggeredStreamingFlags);
DLL_IMPORT unsigned int        ADQ214_TriggeredStreamingArmV5(void* adq_cu_ptr, int ADQ214_num);
DLL_IMPORT unsigned int        ADQ214_TriggeredStreamingGetStatusV5(void* adq_cu_ptr, int ADQ214_num, unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle);
DLL_IMPORT unsigned int        ADQ214_TriggeredStreamingShutdownV5(void* adq_cu_ptr, int ADQ214_num);
DLL_IMPORT unsigned int        ADQ214_TriggeredStreamingGetWaveformV5(void* adq_cu_ptr, int ADQ214_num, short* waveform_data_short);
DLL_IMPORT unsigned int        ADQ214_TriggeredStreamingDisarmV5(void* adq_cu_ptr, int ADQ214_num);
DLL_IMPORT unsigned int        ADQ214_PacketStreamingSetup(void* adq_cu_ptr, int adq214_num, unsigned int PacketSizeSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples);
DLL_IMPORT unsigned int        ADQ214_PacketStreamingArm(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_PacketStreamingDisarm(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_IsBootloader(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT unsigned int        ADQ214_Open(void* adq_cu_ptr, int adq214_num);
DLL_IMPORT int                 ADQ214_Blink(void* adq_cu_ptr, int ADQ214_num);
DLL_IMPORT unsigned int        ADQ214_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int adq214_num, unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT unsigned int        ADQ214_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int adq214_num, unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT unsigned int        ADQ214_SPISend(void* adq_cu_ptr, int adq214_num, unsigned char addr, const char* data, unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT unsigned int        ADQ214_WriteUserRegister(void* adq_cu_ptr, int ADQ214_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int        ADQ214_ReadUserRegister(void* adq_cu_ptr, int ADQ214_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT unsigned int ADQ214_WaveformAveragingParseDataStream(void* adq_cu_ptr, int ADQ214_num, unsigned int samples_per_record, int* data_stream, int** data_target);
DLL_IMPORT unsigned int ADQ214_GetProductFamily(void* adq_cu_ptr, int ADQ214_num, unsigned int* family);

#ifdef __cplusplus
}
#endif



#define INCLUDE_ADQ412

#define ADQ412_DATA_FORMAT_PACKED_12BIT   0
#define ADQ412_DATA_FORMAT_UNPACKED_16BIT 2
#define ADQ412_STREAM_DISABLED     0
#define ADQ412_STREAM_ENABLED      1

#ifdef __cplusplus
extern "C" {
#endif

DLL_IMPORT unsigned int        ADQ412_ResetDevice(void* adq_cu_ptr, int adq412_num, int resetlevel);
DLL_IMPORT int                 ADQ412_GetData(void* adq_cu_ptr, int adq412_num, void** target_buffers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ412_GetDataWH(void* adq_cu_ptr, int adq412_num, void** target_buffers, void* target_headers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ412_GetDataWHTS(void* adq_cu_ptr, int adq412_num, void** target_buffers, void* target_headers, void* target_timestamps, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT unsigned int        ADQ412_SetTransferTimeout(void* adq_cu_ptr, int adq412_num, unsigned int TimeoutValue);
DLL_IMPORT int                 ADQ412_SetTransferBuffers(void* adq_cu_ptr, int adq412_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT int                ADQ412_SetDataFormat(void* adq_cu_ptr, int adq412_num, unsigned int format);
DLL_IMPORT unsigned int        ADQ412_GetDataFormat(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int                 ADQ412_GetStreamStatus(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_SetStreamStatus(void* adq_cu_ptr, int adq412_num, unsigned int status);
DLL_IMPORT unsigned int      ADQ412_GetStreamOverflow(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT void*          ADQ412_GetPtrStream(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_GetTransferBufferStatus(void* adq_cu_ptr, int adq412_num, unsigned int* filled);
DLL_IMPORT unsigned int        ADQ412_SetCacheSize(void* adq_cu_ptr, int adq412_num, unsigned int newCacheSize);
DLL_IMPORT int              ADQ412_SetLvlTrigLevel(void* adq_cu_ptr, int adq412_num, int level);
DLL_IMPORT int              ADQ412_SetLvlTrigFlank(void* adq_cu_ptr, int adq412_num, int flank);
DLL_IMPORT int              ADQ412_SetLvlTrigEdge(void* adq_cu_ptr, int adq412_num, int edge);
DLL_IMPORT int              ADQ412_SetLvlTrigChannel(void* adq_cu_ptr, int adq412_num,int channel);
DLL_IMPORT int              ADQ412_GetLvlTrigChannel(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetTriggedCh(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_SetTriggerMode(void* adq_cu_ptr, int adq412_num, int trig_mode);
DLL_IMPORT int                 ADQ412_SetSampleWidth(void* adq_cu_ptr, int adq412_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ412_SetNofBits(void* adq_cu_ptr, int adq412_num, unsigned int NofBits);
DLL_IMPORT int              ADQ412_SetBufferSizePages(void* adq_cu_ptr, int adq412_num, unsigned int pages);
DLL_IMPORT int                 ADQ412_SetBufferSize(void* adq_cu_ptr, int adq412_num, unsigned int samples);
DLL_IMPORT unsigned int      ADQ412_SetExternTrigEdge(void* adq_cu_ptr, int adq412_num, unsigned int edge);
DLL_IMPORT unsigned int      ADQ412_GetExternTrigEdge(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_ArmTrigger(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_DisarmTrigger(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_USBTrig(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_SWTrig(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_CollectDataNextPage(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_CollectRecord(void* adq_cu_ptr, int adq412_num, unsigned int record_num);
DLL_IMPORT unsigned int      ADQ412_GetErrorVector(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT short*          ADQ412_GetPtrData(void* adq_cu_ptr, int adq412_num, int channel);
DLL_IMPORT int              ADQ412_GetWaitingForTrigger(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetTrigged(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetTriggedAll(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetAcquired(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetAcquiredAll(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_GetAcquiredRecords(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetLvlTrigLevel(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetLvlTrigFlank(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetLvlTrigEdge(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetClockSource(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetTriggerMode(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned long long  ADQ412_GetMaxBufferSize(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_GetMaxBufferSizePages(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int*          ADQ412_GetRevision(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int      ADQ412_WriteRegister(void* adq_cu_ptr, int       adq412_num, int addr, int mask, int data);
DLL_IMPORT unsigned int      ADQ412_ReadRegister(void* adq_cu_ptr, int       adq412_num, int addr);
DLL_IMPORT unsigned int        ADQ412_MemoryDump(void* adq_cu_ptr, int adq412_num,  unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT unsigned int        ADQ412_MemoryShadow(void* adq_cu_ptr, int adq412_num,  void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT unsigned int        ADQ412_MultiRecordSetup(void* adq_cu_ptr, int adq412_num,int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int        ADQ412_MultiRecordSetupGP(void* adq_cu_ptr, int adq412_num,int NumberOfRecords, int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT unsigned int        ADQ412_MultiRecordClose(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_GetSamplesPerPage(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int      ADQ412_GetUSBAddress(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int      ADQ412_GetPCIeAddress(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int      ADQ412_GetTemperature(void* adq_cu_ptr, int adq412_num, unsigned int addr);
DLL_IMPORT unsigned int      ADQ412_WriteEEPROM(void* adq_cu_ptr, int adq412_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int      ADQ412_ReadEEPROM(void* adq_cu_ptr, int adq412_num, unsigned int addr);
DLL_IMPORT unsigned int      ADQ412_WriteEEPROMDB(void* adq_cu_ptr, int adq412_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int      ADQ412_ReadEEPROMDB(void* adq_cu_ptr, int adq412_num, unsigned int addr);
DLL_IMPORT int              ADQ412_PllReg(void* adq_cu_ptr, int adq412_num, unsigned int reg_addr, unsigned char val, unsigned char mask);
DLL_IMPORT int              ADQ412_SetPll(void* adq_cu_ptr, int adq412_num, int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT int                 ADQ412_SetPreTrigSamples(void* adq_cu_ptr, int adq412_num, unsigned int PreTrigSamples);
DLL_IMPORT int                 ADQ412_SetTriggerHoldOffSamples(void* adq_cu_ptr, int adq412_num, unsigned int TriggerHoldOffSamples);
DLL_IMPORT int              ADQ412_ResetOverheat(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_SetClockSource(void* adq_cu_ptr, int adq412_num, int source);
DLL_IMPORT char*          ADQ412_GetBoardSerialNumber(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_RegisterNameLookup(void* adq_cu_ptr, int adq412_num, const char* rn, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT int              ADQ412_SetInterleavingMode(void* adq_cu_ptr, int adq412_num, char interleaving);
DLL_IMPORT int              ADQ412_ADCCalibrate(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_GetADQType(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT const char*          ADQ412_GetCardOption(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_GetLastError(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_SetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int adq412_num, unsigned char IPInstanceAddr, unsigned int freqcalmode);
DLL_IMPORT unsigned int        ADQ412_GetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int adq412_num, unsigned char IPInstanceAddr, unsigned int* freqcalmode);
DLL_IMPORT unsigned int        ADQ412_SetInterleavingIPEstimationMode(void* adq_cu_ptr, int adq412_num, unsigned char IPInstanceAddr, unsigned int updateflag);
DLL_IMPORT unsigned int        ADQ412_GetInterleavingIPEstimationMode(void* adq_cu_ptr, int adq412_num, unsigned char IPInstanceAddr, unsigned int* updateflag);
DLL_IMPORT unsigned int        ADQ412_SetInterleavingIPBypassMode(void* adq_cu_ptr, int adq412_num, unsigned char IPInstanceAddr, unsigned int bypassflag);
DLL_IMPORT unsigned int        ADQ412_GetInterleavingIPBypassMode(void* adq_cu_ptr, int adq412_num, unsigned char IPInstanceAddr, unsigned int* bypassflag);
DLL_IMPORT unsigned int        ADQ412_SetInterleavingIPCalibration(void* adq_cu_ptr, int       adq412_num, unsigned char IPInstanceAddr, unsigned int* calibration);
DLL_IMPORT unsigned int        ADQ412_GetInterleavingIPCalibration(void* adq_cu_ptr, int adq412_num, unsigned char IPInstanceAddr, unsigned int* calibration);
DLL_IMPORT unsigned int        ADQ412_ResetInterleavingIP(void* adq_cu_ptr, int adq412_num, unsigned char IPInstanceAddr);
DLL_IMPORT unsigned int        ADQ412_ReadADCCalibration(void* adq_cu_ptr, int adq412_num, unsigned char ADCNo, unsigned short* calibration);
DLL_IMPORT unsigned int        ADQ412_WriteADCCalibration(void* adq_cu_ptr, int adq412_num, unsigned char ADCNo, unsigned short* calibration);
DLL_IMPORT unsigned int        ADQ412_EnableClockRefOut(void* adq_cu_ptr, int adq412_num, unsigned char enable);
DLL_IMPORT unsigned int      ADQ412_ReadGPIO(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_WriteGPIO(void* adq_cu_ptr, int adq412_num, unsigned int data, unsigned int mask);
DLL_IMPORT int              ADQ412_SetDirectionGPIO(void* adq_cu_ptr, int adq412_num, unsigned int direction, unsigned int mask);
DLL_IMPORT unsigned long long  ADQ412_GetTrigTime(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned long long  ADQ412_GetTrigTimeCycles(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int      ADQ412_GetTrigTimeSyncs(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int      ADQ412_GetTrigTimeStart(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int              ADQ412_SetTrigTimeMode(void* adq_cu_ptr, int adq412_num, int TrigTimeMode);
DLL_IMPORT int              ADQ412_ResetTrigTimer(void* adq_cu_ptr, int adq412_num,int TrigTimeRestart);
DLL_IMPORT unsigned int        ADQ412_SetLvlTrigResetLevel(void* adq_cu_ptr, int adq412_num, int resetlevel);
DLL_IMPORT unsigned int        ADQ412_SetTrigLevelResetValue(void* adq_cu_ptr, int adq412_num, int resetlevel);
DLL_IMPORT unsigned int      ADQ412_GetOutputWidth(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int      ADQ412_GetNofChannels(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT int            ADQ412_SetDirectionTrig(void* adq_cu_ptr, int adq412_num, int direction);
DLL_IMPORT int            ADQ412_WriteTrig(void* adq_cu_ptr, int adq412_num, int data);
DLL_IMPORT unsigned int        ADQ412_WaveformAveragingSetup(void* adq_cu_ptr, int adq412_num, unsigned int NofWaveforms, unsigned int NofSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int WaveformAveragingFlags);
DLL_IMPORT unsigned int        ADQ412_WaveformAveragingArm(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_WaveformAveragingDisarm(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_WaveformAveragingStartReadout(void* adq_cu_ptr, int adq412_num);
DLL_IMPORT unsigned int        ADQ412_WaveformAveragingGetWaveform(void* adq_cu_ptr, int adq412_num, int* waveform_data);
DLL_IMPORT unsigned int        ADQ412_WaveformAveragingGetStatus(void* adq_cu_ptr, int adq412_num, unsigned char* ready, unsigned int* nofrecordscompleted, unsigned char* in_idle);
DLL_IMPORT unsigned int        ADQ412_SetInternalTriggerFrequency(void* adq_cu_ptr, int ADQ412_num, unsigned int Int_Trig_Freq);
DLL_IMPORT unsigned int      ADQ412_SetInternalTriggerPeriod(void* adq_cu_ptr, int adq412_num, unsigned int TriggerPeriodClockCycles);
DLL_IMPORT unsigned int        ADQ412_SetConfigurationTrig(void* adq_cu_ptr, int ADQ412_num, unsigned int mode, unsigned int pulselength, unsigned int invertoutput);
DLL_IMPORT unsigned int    ADQ412_SetEthernetPllFreq(void* adq_cu_ptr, int ADQ412_num, unsigned char eth10_freq, unsigned char eth1_freq);
DLL_IMPORT unsigned int    ADQ412_SetPointToPointPllFreq(void* adq_cu_ptr, int ADQ412_num, unsigned char pp_freq);
DLL_IMPORT unsigned int    ADQ412_SetEthernetPll(void* adq_cu_ptr, int ADQ412_num, unsigned short refdiv, unsigned char useref2, unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv,
    unsigned char eth10_outdiv, unsigned char eth1_outdiv);
DLL_IMPORT unsigned int    ADQ412_SetPointToPointPll(void* adq_cu_ptr, int ADQ412_num, unsigned short refdiv, unsigned char useref2,  unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv,
    unsigned char pp_outdiv, unsigned char ppsync_outdiv);
DLL_IMPORT unsigned int    ADQ412_SetDirectionMLVDS(void* adq_cu_ptr, int ADQ412_num, unsigned char direction);
DLL_IMPORT unsigned int    ADQ412_StorePCIeConfig(void* adq_cu_ptr, int ADQ412_num, UINT32* pci_space);
DLL_IMPORT unsigned int    ADQ412_ReloadPCIeConfig(void* adq_cu_ptr, int ADQ412_num, UINT32* pci_space);
DLL_IMPORT int            ADQ412_IsUSBDevice(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT int            ADQ412_IsPCIeDevice(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int    ADQ412_ReBootADQFromFlash(void* adq_cu_ptr, int ADQ412_num, unsigned int partition);
DLL_IMPORT unsigned int       ADQ412_SendIPCommand(void* adq_cu_ptr, int ADQ412_num, unsigned char ipinst, unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int* answer);
DLL_IMPORT const char*        ADQ412_GetNGCPartNumber(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT const char*        ADQ412_GetUserLogicPartNumber(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int       ADQ412_GetPCIeLinkWidth(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int       ADQ412_GetPCIeLinkRate(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int       ADQ412_SetSampleSkip(void* adq_cu_ptr, int ADQ412_num, unsigned int SkipFactor);
DLL_IMPORT unsigned int       ADQ412_WriteToDataEP(void* adq_cu_ptr, int ADQ412_num, unsigned int *pData, unsigned int length);
DLL_IMPORT unsigned int       ADQ412_SendDataDev2Dev(void* adq_cu_ptr, int ADQ412_num, unsigned long PhysicalAddress, unsigned int channel, unsigned int options);
DLL_IMPORT unsigned int       ADQ412_GetP2pStatus(void* adq_cu_ptr, int ADQ412_num, unsigned int *pending, unsigned int channel);
DLL_IMPORT unsigned int       ADQ412_SetP2pSize(void* adq_cu_ptr, int ADQ412_num, unsigned int bytes, unsigned int channel);
DLL_IMPORT unsigned int       ADQ412_GetP2pSize(void* adq_cu_ptr, int ADQ412_num, unsigned int channel);
DLL_IMPORT int                ADQ412_InitTransfer(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned long      ADQ412_GetPhysicalAddress(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT int                ADQ412_Blink(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int       ADQ412_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int ADQ412_num, unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT unsigned int       ADQ412_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int ADQ412_num, unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT unsigned int       ADQ412_SPISend(void* adq_cu_ptr, int adq412_num, unsigned char addr, const char* data, unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT unsigned int       ADQ412_WriteUserRegister(void* adq_cu_ptr, int ADQ412_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int       ADQ412_ReadUserRegister(void* adq_cu_ptr, int ADQ412_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT unsigned int ADQ412_WaveformAveragingParseDataStream(void* adq_cu_ptr, int ADQ412_num, unsigned int samples_per_record, int* data_stream, int** data_target);
DLL_IMPORT unsigned int ADQ412_WaveformAveragingSoftwareTrigger(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_WaveformAveragingShutdown(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_HasTriggeredStreamingFunctionality(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_TriggeredStreamingSetup(void* adq_cu_ptr, int ADQ412_num, unsigned int NofRecords, unsigned int NofSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned char ChannelsMask);
DLL_IMPORT unsigned int ADQ412_GetTriggeredStreamingRecordSizeBytes(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_GetTriggeredStreamingHeaderSizeBytes(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_GetTriggeredStreamingRecords(void* adq_cu_ptr, int ADQ412_num, unsigned int NofRecordsToRead, void** data_buf, void* header_buf, unsigned int* NofRecordsRead);
DLL_IMPORT unsigned int ADQ412_TriggeredStreamingGetNofRecordsCompleted(void* adq_cu_ptr, int ADQ412_num, unsigned int ChannelsMask,  unsigned int* NofRecordsCompleted);
DLL_IMPORT unsigned int ADQ412_TriggeredStreamingGetStatus(void* adq_cu_ptr, int ADQ412_num, unsigned int* InIdle, unsigned int* TriggerSkipped, unsigned int* Overflow);
DLL_IMPORT unsigned int ADQ412_SetTriggeredStreamingHeaderRegister(void* adq_cu_ptr, int ADQ412_num, char RegValue);
DLL_IMPORT unsigned int ADQ412_SetTriggeredStreamingHeaderSerial(void* adq_cu_ptr, int ADQ412_num, unsigned int SerialNumber);
DLL_IMPORT unsigned int ADQ412_TriggeredStreamingArm(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_TriggeredStreamingDisarm(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_ParseTriggeredStreamingHeader(void* adq_cu_ptr, int ADQ412_num, void* HeaderPtr, unsigned long long* Timestamp, unsigned int* Channel, unsigned int* ExtraAccuracy, int* RegisterValue, unsigned int* SerialNumber, unsigned int* RecordCounter);
DLL_IMPORT const char* ADQ412_GetADQDSPOption(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_GetProductFamily(void* adq_cu_ptr, int ADQ412_num, unsigned int* family);
DLL_IMPORT unsigned int ADQ412_SetupLevelTrigger(void* adq_cu_ptr, int ADQ412_num, int* level, int* edge, int* resetlevel, unsigned int channel, unsigned int individual_mode);
DLL_IMPORT unsigned int ADQ412_HasAdjustableInputRange(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_HasAdjustableBias(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_SetAdjustableBias(void* adq_cu_ptr, int ADQ412_num, unsigned int channel, int ADCcodes);
DLL_IMPORT unsigned int ADQ412_SetInputRange(void* adq_cu_ptr, int ADQ412_num, unsigned int channel, float inputrangemVpp, float* result);
DLL_IMPORT unsigned int ADQ412_WriteADQATTStateManual(void* adq_cu_ptr, int ADQ412_num, unsigned int channel, unsigned int relay16, unsigned int relay8, unsigned int ptap8, unsigned int ptap4, unsigned int ptap2, unsigned int ptap1, unsigned int ptap05, unsigned int ptap025);
DLL_IMPORT unsigned int ADQ412_GetAdjustableBias(void* adq_cu_ptr, int ADQ412_num, unsigned int channel, int *ADCcodes);
DLL_IMPORT unsigned int ADQ412_GetInputRange(void* adq_cu_ptr, int ADQ412_num, unsigned int channel, float *InpRange);
DLL_IMPORT unsigned int ADQ412_EnableWFATriggerCounter(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_DisableWFATriggerCounter(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_StartWFATriggerCounter(void* adq_cu_ptr, int ADQ412_num);
DLL_IMPORT unsigned int ADQ412_SetWFANumberOfTriggers(void* adq_cu_ptr, int ADQ412_num, unsigned int number_of_triggers);
DLL_IMPORT unsigned int ADQ412_SetTriggeredStreamingTotalNofRecords(void* adq_cu_ptr, int ADQ412_num, unsigned int MaxNofRecordsTotal);
DLL_IMPORT unsigned int ADQ412_RunCalibrationADQ412DC(void* adq_cu_ptr, int ADQ412_num, unsigned int calmode);
DLL_IMPORT unsigned int ADQ412_SetCalibrationModeADQ412DC(void* adq_cu_ptr, int ADQ412_num, unsigned int calmode);
DLL_IMPORT unsigned int ADQ412_GetInputImpedance(void* adq_cu_ptr, int ADQ412_num, unsigned int channel, float* impedance);
DLL_IMPORT unsigned int ADQ412_SetupDBS(void* adq_cu_ptr, int ADQ412_num, unsigned char DBS_instance, unsigned int bypass, int dc_target, int lower_saturation_level, int upper_saturation_level);
DLL_IMPORT unsigned int ADQ412_SetDBSSaturationLevels(void* adq_cu_ptr, int ADQ412_num, unsigned char DBS_instance, int lower_saturation_level, int upper_saturation_level);
DLL_IMPORT unsigned int ADQ412_GetPCIeTLPSize(void* adq_cu_ptr, int ADQ412_num);

#ifdef __cplusplus
}
#endif

#define INCLUDE_ADQ1600

#ifdef __cplusplus
extern "C"
{
#endif
DLL_IMPORT unsigned int        ADQ1600_SetConfigurationTrig(void* adq_cu_ptr, int ADQ1600_num, unsigned int mode, unsigned int pulselength, unsigned int invertoutput);
DLL_IMPORT unsigned int        ADQ1600_ResetDevice(void* adq_cu_ptr, int ADQ1600_num, int resetlevel);
DLL_IMPORT unsigned int     ADQ1600_ReBootADQFromFlash(void* adq_cu_ptr, int ADQ1600_num, unsigned int partition);
DLL_IMPORT int                 ADQ1600_IsUSBDevice(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT int                 ADQ1600_IsPCIeDevice(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT int                 ADQ1600_GetData(void* adq_cu_ptr, int ADQ1600_num, void** target_buffers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample,
    unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample,
    unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ1600_GetDataWH(void* adq_cu_ptr, int ADQ1600_num, void** target_buffers, void* target_headers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample,
    unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample,
    unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ1600_GetDataWHTS(void* adq_cu_ptr, int ADQ1600_num, void** target_buffers, void* target_headers, void* target_timestamps, unsigned int target_buffer_size, unsigned char target_bytes_per_sample,
    unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample,
    unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT unsigned int        ADQ1600_SetTransferTimeout(void* adq_cu_ptr, int ADQ1600_num, unsigned int TimeoutValue);
DLL_IMPORT int                 ADQ1600_SetTransferBuffers(void* adq_cu_ptr, int ADQ1600_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT int                ADQ1600_SetDataFormat(void* adq_cu_ptr, int ADQ1600_num, unsigned int format);
DLL_IMPORT unsigned int        ADQ1600_GetDataFormat(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetStreamStatus(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_SetStreamStatus(void* adq_cu_ptr, int ADQ1600_num, unsigned int status);
DLL_IMPORT int                 ADQ1600_GetStreamOverflow(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT void*               ADQ1600_GetPtrStream(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetTransferBufferStatus(void* adq_cu_ptr, int ADQ1600_num, unsigned int* filled);
DLL_IMPORT unsigned int        ADQ1600_SetCacheSize(void* adq_cu_ptr, int ADQ1600_num, unsigned int newCacheSize);
DLL_IMPORT int                 ADQ1600_SetLvlTrigLevel(void* adq_cu_ptr, int ADQ1600_num, int level);
DLL_IMPORT int                 ADQ1600_SetLvlTrigFlank(void* adq_cu_ptr, int ADQ1600_num, int flank);
DLL_IMPORT int                 ADQ1600_SetLvlTrigEdge(void* adq_cu_ptr, int ADQ1600_num, int edge);
DLL_IMPORT int                 ADQ1600_SetLvlTrigChannel(void* adq_cu_ptr, int ADQ1600_num, int channel);
DLL_IMPORT int                 ADQ1600_SetTriggerMode(void* adq_cu_ptr, int ADQ1600_num, int trig_mode);
DLL_IMPORT int                 ADQ1600_SetSampleWidth(void* adq_cu_ptr, int ADQ1600_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ1600_SetNofBits(void* adq_cu_ptr, int ADQ1600_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ1600_SetBufferSizePages(void* adq_cu_ptr, int ADQ1600_num, unsigned int pages);
DLL_IMPORT int                 ADQ1600_SetBufferSize(void* adq_cu_ptr, int ADQ1600_num, unsigned int samples);
DLL_IMPORT unsigned int        ADQ1600_SetExternTrigEdge(void* adq_cu_ptr, int ADQ1600_num, unsigned int edge);
DLL_IMPORT unsigned int        ADQ1600_GetExternTrigEdge(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_ArmTrigger(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_DisarmTrigger(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_USBTrig(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_SWTrig(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_CollectDataNextPage(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_CollectRecord(void* adq_cu_ptr, int ADQ1600_num, unsigned int record_num);
DLL_IMPORT unsigned int         ADQ1600_GetErrorVector(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT short*              ADQ1600_GetPtrData(void* adq_cu_ptr, int ADQ1600_num, int channel);
DLL_IMPORT int                 ADQ1600_GetWaitingForTrigger(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetTrigged(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetTriggedAll(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetAcquired(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetAcquiredAll(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetLvlTrigLevel(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetLvlTrigFlank(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetLvlTrigEdge(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetClockSource(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_GetTriggerMode(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned long long  ADQ1600_GetMaxBufferSize(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetMaxBufferSizePages(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int*                ADQ1600_GetRevision(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_WriteRegister(void* adq_cu_ptr, int ADQ1600_num, int addr, int mask, int data);
DLL_IMPORT unsigned int        ADQ1600_ReadRegister(void* adq_cu_ptr, int ADQ1600_num, int addr);
DLL_IMPORT unsigned int        ADQ1600_MemoryDump(void* adq_cu_ptr, int ADQ1600_num, unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT unsigned int        ADQ1600_MemoryShadow(void* adq_cu_ptr, int adq1600_num,  void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT unsigned int        ADQ1600_MultiRecordSetup(void* adq_cu_ptr, int ADQ1600_num, int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int        ADQ1600_MultiRecordSetupGP(void* adq_cu_ptr, int ADQ1600_num, int NumberOfRecords, int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT unsigned int        ADQ1600_MultiRecordClose(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetSamplesPerPage(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetUSBAddress(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetPCIeAddress(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetTemperature(void* adq_cu_ptr, int ADQ1600_num, unsigned int addr);
DLL_IMPORT unsigned int        ADQ1600_WriteEEPROM(void* adq_cu_ptr, int ADQ1600_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int        ADQ1600_ReadEEPROM(void* adq_cu_ptr, int ADQ1600_num, unsigned int addr);
DLL_IMPORT unsigned int        ADQ1600_WriteEEPROMDB(void* adq_cu_ptr, int ADQ1600_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int        ADQ1600_ReadEEPROMDB(void* adq_cu_ptr, int ADQ1600_num, unsigned int addr);
DLL_IMPORT int                 ADQ1600_PllReg(void* adq_cu_ptr, int ADQ1600_num, unsigned int reg_addr, unsigned char val, unsigned char mask);
DLL_IMPORT int                 ADQ1600_SetPll(void* adq_cu_ptr, int ADQ1600_num, int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT int                 ADQ1600_SetPreTrigSamples(void* adq_cu_ptr, int ADQ1600_num, unsigned int PreTrigSamples);
DLL_IMPORT int                 ADQ1600_SetTriggerHoldOffSamples(void* adq_cu_ptr, int ADQ1600_num, unsigned int TriggerHoldOffSamples);
DLL_IMPORT int                 ADQ1600_ResetOverheat(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_SetClockSource(void* adq_cu_ptr, int ADQ1600_num, int source);
DLL_IMPORT char*               ADQ1600_GetBoardSerialNumber(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_RegisterNameLookup(void* adq_cu_ptr, int ADQ1600_num, const char* rn, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT int                 ADQ1600_SetInterleavingMode(void* adq_cu_ptr, int ADQ1600_num, char interleaving);
DLL_IMPORT int                 ADQ1600_GetADQType(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT const char*         ADQ1600_GetCardOption(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetLastError(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_SetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int ADQ1600_num, unsigned char IPInstanceAddr, unsigned int freqcalmode);
DLL_IMPORT unsigned int        ADQ1600_GetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int ADQ1600_num, unsigned char IPInstanceAddr, unsigned int* freqcalmode);
DLL_IMPORT unsigned int        ADQ1600_SetInterleavingIPEstimationMode(void* adq_cu_ptr, int ADQ1600_num, unsigned char IPInstanceAddr, unsigned int updateflag);
DLL_IMPORT unsigned int        ADQ1600_GetInterleavingIPEstimationMode(void* adq_cu_ptr, int ADQ1600_num, unsigned char IPInstanceAddr, unsigned int* updateflag);
DLL_IMPORT unsigned int        ADQ1600_SetInterleavingIPBypassMode(void* adq_cu_ptr, int ADQ1600_num, unsigned char IPInstanceAddr, unsigned int bypassflag);
DLL_IMPORT unsigned int        ADQ1600_GetInterleavingIPBypassMode(void* adq_cu_ptr, int ADQ1600_num, unsigned char IPInstanceAddr, unsigned int* bypassflag);
DLL_IMPORT unsigned int        ADQ1600_SetInterleavingIPCalibration(void* adq_cu_ptr, int ADQ1600_num, unsigned char IPInstanceAddr, unsigned int* calibration);
DLL_IMPORT unsigned int        ADQ1600_GetInterleavingIPCalibration(void* adq_cu_ptr, int ADQ1600_num, unsigned char IPInstanceAddr, unsigned int* calibration);
DLL_IMPORT unsigned int        ADQ1600_SendIPCommand(void* adq_cu_ptr, int adq1600_num, unsigned char ipinst, unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int* answer);
DLL_IMPORT unsigned int        ADQ1600_ResetInterleavingIP(void* adq_cu_ptr, int ADQ1600_num, unsigned char IPInstanceAddr);
DLL_IMPORT unsigned int        ADQ1600_SetGainAndOffset(void* adq_cu_ptr, int ADQ1600_num, unsigned char Channel, int Gain, int Offset);
DLL_IMPORT unsigned int        ADQ1600_GetGainAndOffset(void* adq_cu_ptr, int ADQ1600_num, unsigned char Channel, int* Gain, int* Offset);
DLL_IMPORT unsigned int        ADQ1600_EnableClockRefOut(void* adq_cu_ptr, int ADQ1600_num, unsigned char enable);
DLL_IMPORT unsigned int        ADQ1600_ReadGPIO(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_WriteGPIO(void* adq_cu_ptr, int ADQ1600_num, unsigned int data, unsigned int mask);
DLL_IMPORT int                 ADQ1600_SetDirectionGPIO(void* adq_cu_ptr, int ADQ1600_num, unsigned int direction, unsigned int mask);
DLL_IMPORT unsigned long long  ADQ1600_GetTrigTime(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned long long  ADQ1600_GetTrigTimeCycles(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetTrigTimeSyncs(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetTrigTimeStart(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_SetTrigTimeMode(void* adq_cu_ptr, int ADQ1600_num, int TrigTimeMode);
DLL_IMPORT int                 ADQ1600_ResetTrigTimer(void* adq_cu_ptr, int ADQ1600_num, int TrigTimeRestart);
DLL_IMPORT unsigned int        ADQ1600_SetLvlTrigResetLevel(void* adq_cu_ptr, int ADQ1600_num, int resetlevel);
DLL_IMPORT unsigned int        ADQ1600_SetTrigLevelResetValue(void* adq_cu_ptr, int ADQ1600_num, int resetlevel);
DLL_IMPORT unsigned int        ADQ1600_GetOutputWidth(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetNofChannels(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT int                 ADQ1600_SetDirectionTrig(void* adq_cu_ptr, int ADQ1600_num, int direction);
DLL_IMPORT int                 ADQ1600_WriteTrig(void* adq_cu_ptr, int ADQ1600_num, int data);
DLL_IMPORT unsigned int       ADQ1600_SetInternalTriggerPeriod(void* adq_cu_ptr, int adq1600_num, unsigned int TriggerPeriodClockCycles);
DLL_IMPORT unsigned int        ADQ1600_WaveformAveragingSetup(void* adq_cu_ptr, int adq1600_num, unsigned int NofWaveforms, unsigned int NofSamples, unsigned int NofPreTrigSamples, unsigned int NofHoldOffSamples, unsigned int WaveformAveragingFlags);
DLL_IMPORT unsigned int        ADQ1600_WaveformAveragingArm(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT unsigned int        ADQ1600_WaveformAveragingDisarm(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT unsigned int        ADQ1600_WaveformAveragingStartReadout(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT unsigned int        ADQ1600_WaveformAveragingGetWaveform(void* adq_cu_ptr, int adq1600_num, int* waveform_data);
DLL_IMPORT unsigned int        ADQ1600_WaveformAveragingGetStatus(void* adq_cu_ptr, int adq1600_num, unsigned char* ready, unsigned int* nofrecordscompleted);
DLL_IMPORT const char*         ADQ1600_GetNGCPartNumber(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT const char*         ADQ1600_GetUserLogicPartNumber(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetPCIeLinkWidth(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetPCIeLinkRate(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT unsigned int     ADQ1600_SetEthernetPllFreq(void* adq_cu_ptr, int adq1600_num, unsigned char eth10_freq, unsigned char eth1_freq);
DLL_IMPORT unsigned int     ADQ1600_SetPointToPointPllFreq(void* adq_cu_ptr, int adq1600_num, unsigned char pp_freq);
DLL_IMPORT unsigned int     ADQ1600_SetEthernetPll(void* adq_cu_ptr, int adq1600_num, unsigned short refdiv, unsigned char useref2, unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv,
    unsigned char eth10_outdiv, unsigned char eth1_outdiv);
DLL_IMPORT unsigned int     ADQ1600_SetPointToPointPll(void* adq_cu_ptr, int adq1600_num, unsigned short refdiv, unsigned char useref2,  unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv,
    unsigned char pp_outdiv, unsigned char ppsync_outdiv);
DLL_IMPORT unsigned int     ADQ1600_SetDirectionMLVDS(void* adq_cu_ptr, int adq1600_num, unsigned char direction);
DLL_IMPORT unsigned int        ADQ1600_SetExtTrigThreshold(void* adq_cu_ptr, int adq1600_num, unsigned int trignum, double vthresh);
DLL_IMPORT unsigned int     ADQ1600_TrigoutEnable(void* adq_cu_ptr, int adq1600_num, unsigned int bitflags);
DLL_IMPORT unsigned int     ADQ1600_HasTrigHardware(void* adq_cu_ptr, int adq1600_num, unsigned int trignum);
DLL_IMPORT unsigned int     ADQ1600_HasTrigoutHardware(void* adq_cu_ptr, int adq1600_num, unsigned int trignum);
DLL_IMPORT unsigned int     ADQ1600_HasVariableTrigThreshold(void* adq_cu_ptr, int adq1600_num, unsigned int trignum);
DLL_IMPORT int                 ADQ1600_Blink(void* adq_cu_ptr, int adq1600_num);
DLL_IMPORT unsigned int        ADQ1600_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int adq1600_num, unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT unsigned int        ADQ1600_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int adq1600_num, unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT unsigned int        ADQ1600_SPISend(void* adq_cu_ptr, int ADQ1600_num, unsigned char addr, const char* data, unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT unsigned int        ADQ1600_SetInternalTriggerFrequency(void* adq_cu_ptr, int ADQ1600_num, unsigned int Int_Trig_Freq);
DLL_IMPORT unsigned int        ADQ1600_WriteUserRegister(void* adq_cu_ptr, int ADQ1600_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int        ADQ1600_ReadUserRegister(void* adq_cu_ptr, int ADQ1600_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT unsigned int ADQ1600_WaveformAveragingParseDataStream(void* adq_cu_ptr, int ADQ1600_num, unsigned int samples_per_record, int* data_stream, int** data_target);
DLL_IMPORT unsigned int ADQ1600_WaveformAveragingSoftwareTrigger(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT const char* ADQ1600_GetADQDSPOption(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int ADQ1600_GetProductFamily(void* adq_cu_ptr, int ADQ1600_num, unsigned int* family);
DLL_IMPORT unsigned int ADQ1600_SetupLevelTrigger(void* adq_cu_ptr, int ADQ1600_num, int* level, int* edge, int* resetlevel, unsigned int channel, unsigned int individual_mode);
DLL_IMPORT unsigned int ADQ1600_SetupDBS(void* adq_cu_ptr, int ADQ1600_num, unsigned char DBS_instance, unsigned int bypass, int dc_target, int lower_saturation_level, int upper_saturation_level);
DLL_IMPORT unsigned int ADQ1600_SetDBSSaturationLevels(void* adq_cu_ptr, int ADQ1600_num, unsigned char DBS_instance, int lower_saturation_level, int upper_saturation_level);
DLL_IMPORT unsigned int ADQ1600_SetSampleSkip(void* adq_cu_ptr, int ADQ1600_num, unsigned int SkipFactor);
DLL_IMPORT unsigned int ADQ1600_HasAdjustableBias(void* adq_cu_ptr, int ADQ1600_num);
DLL_IMPORT unsigned int ADQ1600_SetAdjustableBias(void* adq_cu_ptr, int ADQ1600_num, unsigned int channel, int ADCcodes);
DLL_IMPORT unsigned int ADQ1600_GetPCIeTLPSize(void* adq_cu_ptr, int ADQ1600_num);

#ifdef __cplusplus
}
#endif



#define INCLUDE_SDR14
#ifdef __cplusplus
extern "C" {
#endif

DLL_IMPORT unsigned int        SDR14_SetDACOffsetVoltage(void* adq_cu_ptr, int sdr14_num, unsigned char channel, float v);
DLL_IMPORT unsigned int        SDR14_ResetDevice(void* adq_cu_ptr, int sdr14_num, int resetlevel);
DLL_IMPORT unsigned int        SDR14_ReBootADQFromFlash(void* adq_cu_ptr, int sdr14_num, unsigned int partition);
DLL_IMPORT int                 SDR14_GetData(void* adq_cu_ptr, int sdr14_num, void** target_buffers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 SDR14_GetDataWH(void* adq_cu_ptr, int sdr14_num, void** target_buffers, void* target_headers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 SDR14_GetDataWHTS(void* adq_cu_ptr, int sdr14_num, void** target_buffers, void* target_headers, void* target_timestamps, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 SDR14_WriteToDataEP(void* adq_cu_ptr, int adqdsp_num, unsigned int *pData, unsigned int length);
DLL_IMPORT unsigned int        SDR14_SetTransferTimeout(void* adq_cu_ptr, int sdr14_num, unsigned int TimeoutValue);
DLL_IMPORT int                 SDR14_SetTransferBuffers(void* adq_cu_ptr, int sdr14_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT int                 SDR14_SetDataFormat(void* adq_cu_ptr, int sdr14_num, unsigned int format);
DLL_IMPORT unsigned int        SDR14_GetDataFormat(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetStreamStatus(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_SetStreamStatus(void* adq_cu_ptr, int sdr14_num, unsigned int status);
DLL_IMPORT int                 SDR14_GetStreamOverflow(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT void*               SDR14_GetPtrStream(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetTransferBufferStatus(void* adq_cu_ptr, int sdr14_num, unsigned int* filled);
DLL_IMPORT unsigned int        SDR14_SetCacheSize(void* adq_cu_ptr, int sdr14_num, unsigned int newCacheSize);
DLL_IMPORT int                 SDR14_SetLvlTrigLevel(void* adq_cu_ptr, int sdr14_num, int level);
DLL_IMPORT int                 SDR14_SetLvlTrigFlank(void* adq_cu_ptr, int sdr14_num, int flank);
DLL_IMPORT int                 SDR14_SetLvlTrigEdge(void* adq_cu_ptr, int sdr14_num, int edge);
DLL_IMPORT int                 SDR14_SetLvlTrigChannel(void* adq_cu_ptr, int sdr14_num,int channel);
DLL_IMPORT int                 SDR14_GetLvlTrigChannel(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetTriggedCh(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_SetTriggerMode(void* adq_cu_ptr, int sdr14_num, int trig_mode);
DLL_IMPORT int                 SDR14_SetSampleWidth(void* adq_cu_ptr, int sdr14_num, unsigned int NofBits);
DLL_IMPORT int                 SDR14_SetNofBits(void* adq_cu_ptr, int sdr14_num, unsigned int NofBits);
DLL_IMPORT int                 SDR14_SetBufferSizePages(void* adq_cu_ptr, int sdr14_num, unsigned int pages);
DLL_IMPORT int                 SDR14_SetBufferSize(void* adq_cu_ptr, int sdr14_num, unsigned int samples);
DLL_IMPORT unsigned int        SDR14_SetExternTrigEdge(void* adq_cu_ptr, int sdr14_num, unsigned int edge);
DLL_IMPORT unsigned int        SDR14_GetExternTrigEdge(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_ArmTrigger(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_DisarmTrigger(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_USBTrig(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_SWTrig(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_CollectDataNextPage(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_CollectRecord(void* adq_cu_ptr, int sdr14_num, unsigned int record_num);
DLL_IMPORT unsigned int         SDR14_GetErrorVector(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT short*              SDR14_GetPtrData(void* adq_cu_ptr, int sdr14_num, int channel);
DLL_IMPORT int                 SDR14_GetWaitingForTrigger(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetTrigged(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetTriggedAll(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetAcquired(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetAcquiredAll(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetLvlTrigLevel(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetLvlTrigFlank(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetLvlTrigEdge(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetClockSource(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_GetTriggerMode(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned long long  SDR14_GetMaxBufferSize(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetMaxBufferSizePages(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int*                SDR14_GetRevision(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_WriteRegister(void* adq_cu_ptr, int       sdr14_num, int addr, int mask, int data);
DLL_IMPORT unsigned int        SDR14_ReadRegister(void* adq_cu_ptr, int       sdr14_num, int addr);
DLL_IMPORT unsigned int        SDR14_MemoryDump(void* adq_cu_ptr, int sdr14_num,  unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT unsigned int        SDR14_MemoryShadow(void* adq_cu_ptr, int sdr14_num,  void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT unsigned int        SDR14_MultiRecordSetup(void* adq_cu_ptr, int sdr14_num,int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int        SDR14_MultiRecordSetupGP(void* adq_cu_ptr, int sdr14_num,int NumberOfRecords, int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT unsigned int        SDR14_MultiRecordClose(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetSamplesPerPage(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetUSBAddress(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetPCIeAddress(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetTemperature(void* adq_cu_ptr, int sdr14_num, unsigned int addr);
DLL_IMPORT unsigned int        SDR14_WriteEEPROM(void* adq_cu_ptr, int sdr14_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int        SDR14_ReadEEPROM(void* adq_cu_ptr, int sdr14_num, unsigned int addr);
DLL_IMPORT unsigned int        SDR14_WriteEEPROMDB(void* adq_cu_ptr, int sdr14_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int        SDR14_ReadEEPROMDB(void* adq_cu_ptr, int sdr14_num, unsigned int addr);
DLL_IMPORT int                 SDR14_PllReg(void* adq_cu_ptr, int sdr14_num, unsigned int reg_addr, unsigned char val, unsigned char mask);
DLL_IMPORT int                 SDR14_SetPll(void* adq_cu_ptr, int sdr14_num, int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT int                 SDR14_SetPreTrigSamples(void* adq_cu_ptr, int sdr14_num, unsigned int PreTrigSamples);
DLL_IMPORT int                 SDR14_SetTriggerHoldOffSamples(void* adq_cu_ptr, int sdr14_num, unsigned int TriggerHoldOffSamples);
DLL_IMPORT int                 SDR14_ResetOverheat(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_SetClockSource(void* adq_cu_ptr, int sdr14_num, int source);
DLL_IMPORT char*               SDR14_GetBoardSerialNumber(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_RegisterNameLookup(void* adq_cu_ptr, int sdr14_num, const char* rn, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT int                 SDR14_GetADQType(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT const char*         SDR14_GetCardOption(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetLastError(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_SetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int sdr14_num, unsigned char IPInstanceAddr, unsigned int freqcalmode);
DLL_IMPORT unsigned int        SDR14_GetInterleavingIPFrequencyCalibrationMode(void* adq_cu_ptr, int sdr14_num, unsigned char IPInstanceAddr, unsigned int* freqcalmode);
DLL_IMPORT unsigned int        SDR14_SetInterleavingIPEstimationMode(void* adq_cu_ptr, int sdr14_num, unsigned char IPInstanceAddr, unsigned int updateflag);
DLL_IMPORT unsigned int        SDR14_GetInterleavingIPEstimationMode(void* adq_cu_ptr, int sdr14_num, unsigned char IPInstanceAddr, unsigned int* updateflag);
DLL_IMPORT unsigned int        SDR14_SetInterleavingIPBypassMode(void* adq_cu_ptr, int sdr14_num, unsigned char IPInstanceAddr, unsigned int bypassflag);
DLL_IMPORT unsigned int        SDR14_GetInterleavingIPBypassMode(void* adq_cu_ptr, int sdr14_num, unsigned char IPInstanceAddr, unsigned int* bypassflag);
DLL_IMPORT unsigned int        SDR14_SetInterleavingIPCalibration(void* adq_cu_ptr, int       sdr14_num, unsigned char IPInstanceAddr, unsigned int* calibration);
DLL_IMPORT unsigned int        SDR14_GetInterleavingIPCalibration(void* adq_cu_ptr, int sdr14_num, unsigned char IPInstanceAddr, unsigned int* calibration);
DLL_IMPORT unsigned int        SDR14_ResetInterleavingIP(void* adq_cu_ptr, int sdr14_num, unsigned char IPInstanceAddr);
DLL_IMPORT unsigned int        SDR14_EnableClockRefOut(void* adq_cu_ptr, int sdr14_num, unsigned char enable);
DLL_IMPORT unsigned int        SDR14_ReadGPIO(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_WriteGPIO(void* adq_cu_ptr, int sdr14_num, unsigned int data, unsigned int mask);
DLL_IMPORT int                 SDR14_SetDirectionGPIO(void* adq_cu_ptr, int sdr14_num, unsigned int direction, unsigned int mask);
DLL_IMPORT unsigned long long  SDR14_GetTrigTime(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned long long  SDR14_GetTrigTimeCycles(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetTrigTimeSyncs(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetTrigTimeStart(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT int                 SDR14_SetTrigTimeMode(void* adq_cu_ptr, int sdr14_num, int TrigTimeMode);
DLL_IMPORT int                 SDR14_ResetTrigTimer(void* adq_cu_ptr, int sdr14_num,int TrigTimeRestart);
DLL_IMPORT unsigned int        SDR14_SetLvlTrigResetLevel(void* adq_cu_ptr, int sdr14_num, int resetlevel);
DLL_IMPORT unsigned int        SDR14_SetTrigLevelResetValue(void* adq_cu_ptr, int sdr14_num, int resetlevel);
DLL_IMPORT unsigned int        SDR14_GetOutputWidth(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_GetNofChannels(void* adq_cu_ptr, int sdr14_num);
DLL_IMPORT unsigned int        SDR14_AWGReset(void* adq_cu_ptr, int sdr14_num, unsigned int dacId);
DLL_IMPORT unsigned int        SDR14_AWGmalloc(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int LengthSeg1, unsigned int LengthSeg2, unsigned int LengthSeg3, unsigned int LengthSeg4);
DLL_IMPORT unsigned int        SDR14_AWGSegmentMalloc(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int segId, unsigned int length, unsigned char reallocate);
DLL_IMPORT unsigned int        SDR14_AWGWriteSegment(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int segId, unsigned int enable, unsigned int NofLaps, unsigned int length, int *data);
DLL_IMPORT unsigned int        SDR14_AWGWriteSegments(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int NofSegs, unsigned int* segId, unsigned int* NofLaps, unsigned int* length, short int** data);
DLL_IMPORT unsigned int        SDR14_AWGEnableSegments(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int enableSeg);
DLL_IMPORT unsigned int        SDR14_AWGAutoRearm(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int enable);
DLL_IMPORT unsigned int        SDR14_AWGContinuous(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int enable);
DLL_IMPORT unsigned int        SDR14_AWGTrigMode(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int trigmode);
DLL_IMPORT unsigned int        SDR14_AWGArm(void* adq_cu_ptr, int sdr14_num, unsigned int dacId);
DLL_IMPORT unsigned int        SDR14_AWGDisarm(void* adq_cu_ptr, int sdr14_num, unsigned int dacId);
DLL_IMPORT unsigned int        SDR14_AWGSetupTrigout(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int trigoutmode, unsigned int pulselength, unsigned int enableflags, unsigned int autorearm);
DLL_IMPORT unsigned int        SDR14_AWGTrigoutArm(void* adq_cu_ptr, int sdr14_num, unsigned int dacId);
DLL_IMPORT unsigned int        SDR14_AWGTrigoutDisarm(void* adq_cu_ptr, int sdr14_num, unsigned int dacId);
DLL_IMPORT unsigned int        SDR14_AWGSetTriggerEnable(void* adq_cu_ptr, int sdr14_num, unsigned int dacId, unsigned int bitflags);
DLL_IMPORT unsigned int        SDR14_SetGainAndOffset(void* adq_cu_ptr, int SDR14_num, unsigned char Channel, int Gain, int Offset);
DLL_IMPORT unsigned int        SDR14_GetGainAndOffset(void* adq_cu_ptr, int SDR14_num, unsigned char Channel, int* Gain, int* Offset);
DLL_IMPORT unsigned int        SDR14_SetMinAndMaxSaturation(void* adq_cu_ptr, int SDR14_num, unsigned char Channel, int max_code_control, int min_code_control);
DLL_IMPORT unsigned int        SDR14_StorePCIeConfig(void* adq_cu_ptr, int SDR14_num, UINT32* pci_space);
DLL_IMPORT unsigned int        SDR14_ReloadPCIeConfig(void* adq_cu_ptr, int SDR14_num, UINT32* pci_space);
DLL_IMPORT int                 SDR14_IsUSBDevice(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT int                 SDR14_IsPCIeDevice(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT unsigned int        SDR14_SendIPCommand(void* adq_cu_ptr, int SDR14_num, unsigned char ipinst, unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int* answer);
DLL_IMPORT unsigned int        SDR14_OffsetDACSpiWrite(void* adq_cu_ptr, int SDR14_num, unsigned char spi_address, unsigned int data);
DLL_IMPORT unsigned int        SDR14_DACSpiWrite(void* adq_cu_ptr, int SDR14_num, unsigned char spi_address, const unsigned char address, const unsigned char data);
DLL_IMPORT unsigned int        SDR14_DACSpiRead(void* adq_cu_ptr, int SDR14_num, unsigned char spi_address, unsigned char address, unsigned char *data);
DLL_IMPORT const char*         SDR14_GetNGCPartNumber(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT const char*         SDR14_GetUserLogicPartNumber(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT unsigned int        SDR14_SetInternalTriggerPeriod(void* adq_cu_ptr, int SDR14_num, unsigned int TriggerPeriodClockCycles);
DLL_IMPORT unsigned int        SDR14_WriteSTARBDelay(void* adq_cu_ptr, int SDR14_num, unsigned int starbdelay, unsigned int writetoeeprom);
DLL_IMPORT unsigned int        SDR14_EnablePXIeTriggers(void* adq_cu_ptr, int SDR14_num, unsigned int port, unsigned int bitflags);
DLL_IMPORT unsigned int        SDR14_EnablePXIeTrigout(void* adq_cu_ptr, int SDR14_num, unsigned int trigsource, unsigned int bitflags);
DLL_IMPORT unsigned int        SDR14_PXIeSoftwareTrigger(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT unsigned int        SDR14_SetPXIeTrigDirection(void* adq_cu_ptr, int SDR14_num, unsigned int trig0output, unsigned int trig1output);
DLL_IMPORT unsigned int        SDR14_GetPCIeLinkWidth(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT unsigned int        SDR14_GetPCIeLinkRate(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT unsigned int        SDR14_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int sdr14_num, unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT unsigned int        SDR14_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int sdr14_num, unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT unsigned int        SDR14_SPISend(void* adq_cu_ptr, int sdr14_num, unsigned char addr, const char* data, unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT unsigned int        SDR14_WriteUserRegister(void* adq_cu_ptr, int SDR14_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int        SDR14_ReadUserRegister(void* adq_cu_ptr, int SDR14_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT int                 SDR14_Blink(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT unsigned int        SDR14_WaveformAveragingParseDataStream(void* adq_cu_ptr, int SDR14_num, unsigned int samples_per_record, int* data_stream, int** data_target);
DLL_IMPORT unsigned int        SDR14_WaveformAveragingSoftwareTrigger(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT const char*         SDR14_GetADQDSPOption(void* adq_cu_ptr, int SDR14_num);
DLL_IMPORT unsigned int        SDR14_GetProductFamily(void* adq_cu_ptr, int SDR14_num, unsigned int* family);
DLL_IMPORT unsigned int        SDR14_SetupLevelTrigger(void* adq_cu_ptr, int SDR14_num, int* level, int* edge, int* resetlevel, unsigned int channel, unsigned int individual_mode);
DLL_IMPORT unsigned int SDR14_SetSampleSkip(void* adq_cu_ptr, int SDR14_num, unsigned int SkipFactor);
DLL_IMPORT unsigned int SDR14_GetPCIeTLPSize(void* adq_cu_ptr, int SDR14_num);

#ifdef __cplusplus
}
#endif

#define INCLUDE_ADQSPHINX

#ifdef __cplusplus
extern "C"
{
#endif
DLL_IMPORT int SphinxAA_SetClockSource(void* adq_cu_ptr, int SphinxAA_num, int source);
DLL_IMPORT int SphinxAA_SetPllFreqDivider(void* adq_cu_ptr, int SphinxAA_num, int divider);
DLL_IMPORT int SphinxAA_SetTriggerMode(void* adq_cu_ptr, int SphinxAA_num, int trig_mode);
DLL_IMPORT int SphinxAA_SetTriginDirection(void* adq_cu_ptr, int SphinxAA_num, int direction);
DLL_IMPORT int SphinxAA_SetTrigRateDivider(void* adq_cu_ptr, int SphinxAA_num, int divider);
DLL_IMPORT int SphinxAA_SetWordsPerBatch(void* adq_cu_ptr, int SphinxAA_num, unsigned int samples);
DLL_IMPORT int SphinxAA_SetRefclkDirection(void* adq_cu_ptr, int SphinxAA_num, int direction);
DLL_IMPORT int SphinxAA_SetNofBatches(void* adq_cu_ptr, int SphinxAA_num, unsigned int batches);
DLL_IMPORT int SphinxAA_SetPreTrigSamples(void* adq_cu_ptr, int SphinxAA_num, unsigned int samples);
DLL_IMPORT int SphinxAA_ArmTrigger(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_DisarmTrigger(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SWTrig(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_GetData(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned long SphinxAA_GetPXIeAddress(void* adq_cu_ptr, int SphinxProc_num);
DLL_IMPORT int SphinxAA_SendData2Proc(void* adq_cu_ptr, int SphinxAA_num, unsigned long PhysicalAddress);
DLL_IMPORT short* SphinxAA_GetPtrData(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetDataReady(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetFifoOverflow(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_GetPllFreqDivider(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_GetClockSource(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_GetTriggerMode(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_GetTrigRateDivider(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetWordsPerBatch(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetPreTrigSamples(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetNofBatches(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetNofBatchesPerGetData(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetUSBAddress(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetBcdDevice(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int* SphinxAA_GetRevision(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_GetOverflow(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_IsUSBDevice(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_IsPXIeDevice(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_SendProcessorCommand(void* adq_cu_ptr, int SphinxAA_num, unsigned int command, unsigned int addr, unsigned int mask,
    unsigned int data);
DLL_IMPORT unsigned int SphinxAA_WriteRegister(void* adq_cu_ptr, int SphinxAA_num, unsigned int addr, unsigned int mask, unsigned int data);
DLL_IMPORT unsigned int SphinxAA_ReadRegister(void* adq_cu_ptr, int SphinxAA_num, unsigned int addr);
DLL_IMPORT int SphinxAA_SetMode(void* adq_cu_ptr, int SphinxAA_num, unsigned int mode);
DLL_IMPORT unsigned int SphinxAA_GetMode(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_NormEnable(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_NormDisable(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_FilterEnable(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_FilterDisable(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_PostDecEnable(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_PostDecDisable(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetPostDecimationFactor(void* adq_cu_ptr, int SphinxAA_num, int decimation_factor);
DLL_IMPORT int SphinxAA_SetOffsetLength(void* adq_cu_ptr, int SphinxAA_num, unsigned int offset_length);
DLL_IMPORT unsigned int SphinxAA_GetOffsetLength(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_ResetIntegration(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_EnableIntegration(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetIntegrationCoeff(void* adq_cu_ptr, int SphinxAA_num, unsigned int coeff);
DLL_IMPORT int SphinxAA_GetIntegrationCoeff(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetOutputType(void* adq_cu_ptr, int SphinxAA_num, unsigned int type);
DLL_IMPORT unsigned int SphinxAA_GetOutputType(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetSendStart(void* adq_cu_ptr, int SphinxAA_num, unsigned int pos);
DLL_IMPORT unsigned int SphinxAA_GetSendStart(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetSendLength(void* adq_cu_ptr, int SphinxAA_num, unsigned int pos);
DLL_IMPORT unsigned int SphinxAA_GetSendLength(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetReflev(void* adq_cu_ptr, int SphinxAA_num, unsigned int channel, int reflev);
DLL_IMPORT int SphinxAA_GetReflev(void* adq_cu_ptr, int SphinxAA_num, unsigned int channel);
DLL_IMPORT unsigned int SphinxAA_GetMaxXferLength(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetTimeout(void* adq_cu_ptr, int SphinxAA_num, int timeout);
DLL_IMPORT int SphinxAA_SetMaxNofBatchesPerGetData(void* adq_cu_ptr, int SphinxAA_num, unsigned int batches);
DLL_IMPORT int SphinxAA_SetOffsetRange(void* adq_cu_ptr, int SphinxAA_num, unsigned int channel, int lower_bound, int upper_bound);
DLL_IMPORT int SphinxAA_GetOffsetRange(void* adq_cu_ptr, int SphinxAA_num, unsigned int channel, unsigned int bound);
DLL_IMPORT int SphinxAA_SetOutputControlTestPattern(void* adq_cu_ptr, int SphinxAA_num, unsigned int state);
DLL_IMPORT int SphinxAA_SetDeltaphiRange(void* adq_cu_ptr, int SphinxAA_num, int lower_bound, int upper_bound);
DLL_IMPORT int SphinxAA_GetDeltaphiRange(void* adq_cu_ptr, int SphinxAA_num, unsigned int bound);
DLL_IMPORT int SphinxAA_SetZ(void* adq_cu_ptr, int SphinxAA_num, unsigned int z);
DLL_IMPORT unsigned int SphinxAA_GetZ(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetPhiGain(void* adq_cu_ptr, int SphinxAA_num, unsigned int phi_gain);
DLL_IMPORT unsigned int SphinxAA_GetPhiGain(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetNormGain(void* adq_cu_ptr, int SphinxAA_num, unsigned int norm_gain);
DLL_IMPORT int SphinxAA_SetNormalizationFactor(void* adq_cu_ptr, int SphinxAA_num, unsigned int channel, unsigned int normalization_factor);
DLL_IMPORT int SphinxAA_SetNormalizationReferenceLevel(void* adq_cu_ptr, int SphinxAA_num, signed int reference_level);
DLL_IMPORT unsigned int SphinxAA_SetNormalizationMode(void* adq_cu_ptr, int SphinxAA_num, unsigned int Mode, unsigned int TimeInterval, unsigned int L1, unsigned int L2);
DLL_IMPORT unsigned int SphinxAA_GetNormalizationMode(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetLiveNFactors(void* adq_cu_ptr, int SphinxAA_num, unsigned short* N_factors);


DLL_IMPORT unsigned int SphinxAA_SetDiffAmplifyFactor(void* adq_cu_ptr, int SphinxAA_num, unsigned int diff_amp);
DLL_IMPORT unsigned int SphinxAA_SetDPhiFilterFactors(void* adq_cu_ptr, int SphinxAA_num, unsigned int z, unsigned int a);
DLL_IMPORT unsigned int SphinxAA_SetChannelDelay(void* adq_cu_ptr, int SphinxAA_num, unsigned int channel, unsigned int delay);
DLL_IMPORT unsigned int SphinxAA_GetNormGain(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetOOR(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetDeltaphiDenominator(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetDeltaphiDenominatorZ(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetDecimationFactor(void* adq_cu_ptr, int SphinxAA_num, unsigned int decimation_factor);
DLL_IMPORT unsigned int SphinxAA_GetDecimationFactor(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_SetCICDecimationFactor(void* adq_cu_ptr, int SphinxAA_num, int decimation_factor);
DLL_IMPORT unsigned int SphinxAA_GetCICDecimationFactor(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_SetCICScaleFactor(void* adq_cu_ptr, int SphinxAA_num, long scale_factor);
DLL_IMPORT unsigned int SphinxAA_ConfigurePulseGenerator(void* adq_cu_ptr, int SphinxAA_num, char enable, unsigned int period, unsigned int pulse1_width,
    float pulse1_peak, unsigned int pulse2_width, float pulse2_peak, unsigned int pulse2_delay);
DLL_IMPORT unsigned int SphinxAA_SetAdcGain(void* adq_cu_ptr, int SphinxAA_num, unsigned int gain);
DLL_IMPORT unsigned int SphinxAA_WriteToDataEP(void* adq_cu_ptr, int SphinxAA_num, unsigned int *pData, unsigned int length);
DLL_IMPORT unsigned int SphinxAA_SetGpioDir(void* adq_cu_ptr, int SphinxAA_num, unsigned int dir);
DLL_IMPORT unsigned int SphinxAA_SetGpioVal(void* adq_cu_ptr, int SphinxAA_num, unsigned int val);
DLL_IMPORT unsigned int SphinxAA_GetGpioVal(void* adq_cu_ptr, int SphinxAA_num, unsigned int *val);
DLL_IMPORT unsigned int SphinxAA_SetAnalogOut(void* adq_cu_ptr, int SphinxAA_num, unsigned int port, float val);
DLL_IMPORT unsigned int SphinxAA_ReadEEPROMDB(void* adq_cu_ptr, int SphinxAA_num, unsigned int addr);
DLL_IMPORT unsigned int SphinxAA_WriteEEPROMDB(void* adq_cu_ptr, int SphinxAA_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int SphinxAA_ReadEEPROM(void* adq_cu_ptr, int SphinxAA_num, unsigned int addr);
DLL_IMPORT unsigned int SphinxAA_WriteEEPROM(void* adq_cu_ptr, int SphinxAA_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int SphinxAA_GetTemperature(void* adq_cu_ptr, int SphinxAA_num, unsigned int addr);
DLL_IMPORT unsigned int SphinxAA_GetType(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_SetTransferBuffers(void* adq_cu_ptr, int SphinxAA_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT unsigned int SphinxAA_SetTrigSync(void* adq_cu_ptr, int SphinxAA_num, unsigned int setting);
DLL_IMPORT unsigned int SphinxAA_SetTrigSyncRiseFall(void* adq_cu_ptr, int SphinxAA_num, unsigned int trf);
DLL_IMPORT unsigned int SphinxAA_SetTrigSyncDelay(void* adq_cu_ptr, int SphinxAA_num, unsigned int delay);
DLL_IMPORT unsigned int SphinxAA_GetTrigSyncDetect(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_InitTransfer(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned long SphinxAA_GetPhysicalAddress(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_PCIeDebugEnable(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT int SphinxAA_PCIeDebugDisable(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetRawData(void* adq_cu_ptr, int SphinxAA_num, unsigned int nofbytes, unsigned int* prefetch, unsigned int* nofbytesreturned);
DLL_IMPORT unsigned int SphinxAA_SendRawData2Proc(void* adq_cu_ptr, int SphinxAA_num, unsigned long PhysicalAddress, unsigned int* nofbytes, unsigned int* prefetch);
DLL_IMPORT char* SphinxAA_GetBoardSerialNumber(void* adq_cu_ptr, int SphinxAA_num);
DLL_IMPORT unsigned int SphinxAA_GetPCIeTLPSize(void* adq_cu_ptr, int SphinxAA_num);
#ifdef __cplusplus
}
#endif


#define INCLUDE_DSU

#define REC_IP_STATUS_SETUP_OK          0x00000001
#define REC_IP_STATUS_WAITINGFORDATA    0x00000002
#define REC_IP_STATUS_CMD_ANSWERED      0x00000004
#define REC_IP_STATUS_CMD_SUCCESS       0x00000008
#define REC_IP_STATUS_DISK_INIT_OK      0x00004000
#define REC_IP_STATUS_ERROR             0xF0000000
#define REC_IP_STATUS_READ_STUCK_ERROR  0x08000000
#define REC_IP_STATUS_WRITE_STUCK_ERROR 0x04000000
#define REC_IP_STATUS_DISK_SECTOR_ERROR 0x02000000
#define REC_IP_STATUS_DISK_ERASE_ERROR  0x01000000
#define REC_IP_STATUS_IN_TEST           0x00000010
#define REC_IP_STATUS_IN_WRITE          0x00000020
#define REC_IP_STATUS_IN_READ           0x00000040

#define REC_IP_DISK_STATUS_INIT_OK     0x00000001
#define REC_IP_DISK_STATUS_ERASE_ERROR 0x00000010

#define REC_IP_CMD_SET_NOFDISKS                 0x00000001
#define REC_IP_CMD_DISK_INIT                   0x00000002
#define REC_IP_CMD_SET_PARAMETER               0x00000003
#define REC_IP_CMD_ERASE_DISK                  0x00000004
#define REC_IP_CMD_TEST_PERFORMANCE            0x00000005
#define REC_IP_CMD_START_DATA_RECORDING        0x00000006
#define REC_IP_CMD_END_DATA_RECORDING          0x00000007
#define REC_IP_CMD_START_DATA_READING          0x00000008
#define REC_IP_CMD_RESTART_DATA_RECORDING      0x00000009
#define REC_IP_CMD_DEBUG                       0x0000000A
#define REC_IP_CMD_READ_INTERNAL_STATUS        0x0000000B
#define REC_IP_CMD_GET_PARAMETER               0x0000000C
#define REC_IP_CMD_RESET                       0x0000000D

#define REC_IP_PAR_SECTOR_SIZE                   1
#define REC_IP_PAR_REC_STRIP_SIZE                2
#define REC_IP_PAR_REC_START_ADDRESS           100
#define REC_IP_PAR_REC_END_ADDRESS             101
#define REC_IP_PAR_SECTORS_PER_ATA             105

#ifdef __cplusplus
extern "C" {
#endif
DLL_IMPORT int*                DSU_GetRevision(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned int        DSU_WriteRegister(void* adq_cu_ptr, int DSU_num, int addr, int mask, int data);
DLL_IMPORT unsigned int        DSU_ReadRegister(void* adq_cu_ptr, int DSU_num, int addr);
DLL_IMPORT int                 DSU_WaitForPCIeDMAFinish(void* adq_cu_ptr, int DSU_num, unsigned int length);
DLL_IMPORT int                 DSU_GetDSPDataNowait(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT int                 DSU_GetDSPData(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT int                 DSU_SetSendLength(void* adq_cu_ptr, int DSU_num, unsigned int length);
DLL_IMPORT unsigned int        DSU_GetSendLength(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned int       DSU_GetRecorderBytesPerAddress(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned long       DSU_GetPhysicalAddress(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned int*       DSU_GetPtrData(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned int        DSU_GetTemperature(void* adq_cu_ptr, int DSU_num, unsigned int addr);
DLL_IMPORT int                 DSU_InitTransfer(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT int                 DSU_WriteToDataEP(void* adq_cu_ptr, int DSU_num,unsigned int *pData, unsigned int length);
DLL_IMPORT unsigned int        DSU_GetADQType(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned int        DSU_TrigOutEn(void* adq_cu_ptr, int DSU_num, unsigned int en);
DLL_IMPORT unsigned int        DSU_GetLastError(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT int                DSU_SetTransferTimeout(void* adq_cu_ptr, int DSU_num, unsigned int TimeoutValue);
DLL_IMPORT int                 DSU_SetTransferBuffers(void* adq_cu_ptr, int DSU_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT unsigned int        DSU_GetTransferBufferStatus(void* adq_cu_ptr, int DSU_num, unsigned int* filled);
DLL_IMPORT const char*         DSU_GetNGCPartNumber(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT const char*         DSU_GetUserLogicPartNumber(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned int        DSU_GetPCIeLinkWidth(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned int        DSU_GetPCIeLinkRate(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned int        DSU_WriteDataToDSU(void* adq_cu_ptr, int DSU_num, unsigned int inst, unsigned int start_address, unsigned int nofbytes, unsigned char* data);
DLL_IMPORT unsigned int        DSU_ReadDataFromDSU(void* adq_cu_ptr, int DSU_num, unsigned int inst, unsigned int start_address, unsigned int nofbytes, unsigned char* data);
DLL_IMPORT unsigned int        DSU_StartDSUAcquisition(void* adq_cu_ptr, int DSU_num, unsigned int inst);
DLL_IMPORT unsigned int        DSU_SetupDSUAcquisition(void* adq_cu_ptr, int DSU_num, unsigned int inst, unsigned int start_address, unsigned int end_address);
DLL_IMPORT unsigned int        DSU_ResetRecorder(void* adq_cu_ptr, int DSU_num, unsigned int inst);
DLL_IMPORT unsigned int        DSU_ResetFIFOPaths(void* adq_cu_ptr, int DSU_num, unsigned int inst);
DLL_IMPORT unsigned int        DSU_RunRecorderSelfTest(void* adq_cu_ptr, int DSU_num, unsigned int inst, unsigned int* inout_vector);
DLL_IMPORT int                 DSU_Blink(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT const char* DSU_GetADQDSPOption(void* adq_cu_ptr, int DSU_num);
DLL_IMPORT unsigned int DSU_GetProductFamily(void* adq_cu_ptr, int DSU_num, unsigned int* family);
DLL_IMPORT char* DSU_GetBoardSerialNumber(void* adq_cu_ptr, int DSU_num);
#ifdef __cplusplus
}
#endif


#define INCLUDE_ADQ208

#ifdef __cplusplus
extern "C" {
#endif

DLL_IMPORT int           ADQ208_ADCCalibrate(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_ResetDevice(void* adq_cu_ptr, int adq208_num, int resetlevel);
DLL_IMPORT int                 ADQ208_GetData(void* adq_cu_ptr, int adq208_num, void** target_buffers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ208_GetDataWH(void* adq_cu_ptr, int adq208_num, void** target_buffers, void* target_headers, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT int                 ADQ208_GetDataWHTS(void* adq_cu_ptr, int adq208_num, void** target_buffers, void* target_headers, void* target_timestamps, unsigned int target_buffer_size, unsigned char target_bytes_per_sample, unsigned int StartRecordNumber, unsigned int NumberOfRecords, unsigned char ChannelsMask, unsigned int StartSample, unsigned int nSamples, unsigned char TransferMode);
DLL_IMPORT unsigned int        ADQ208_SetTransferTimeout(void* adq_cu_ptr, int adq208_num, unsigned int TimeoutValue);
DLL_IMPORT int                 ADQ208_SetTransferBuffers(void* adq_cu_ptr, int adq208_num, unsigned int nOfBuffers, unsigned int bufferSize);
DLL_IMPORT unsigned int        ADQ208_GetTransferBufferStatus(void* adq_cu_ptr, int adq208_num, unsigned int* filled);
DLL_IMPORT int                ADQ208_SetDataFormat(void* adq_cu_ptr, int adq208_num, unsigned int format);
DLL_IMPORT unsigned int        ADQ208_GetDataFormat(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetStreamStatus(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_SetStreamStatus(void* adq_cu_ptr, int adq208_num, unsigned int status);
DLL_IMPORT int                 ADQ208_GetStreamOverflow(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT void*               ADQ208_GetPtrStream(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_SetCacheSize(void* adq_cu_ptr, int adq208_num, unsigned int newCacheSize);
DLL_IMPORT int                 ADQ208_SetLvlTrigLevel(void* adq_cu_ptr, int adq208_num, int level);
DLL_IMPORT int                 ADQ208_SetLvlTrigFlank(void* adq_cu_ptr, int adq208_num, int flank);
DLL_IMPORT int                 ADQ208_SetLvlTrigEdge(void* adq_cu_ptr, int adq208_num, int edge);
DLL_IMPORT int                 ADQ208_SetLvlTrigChannel(void* adq_cu_ptr, int adq208_num, int channel);
DLL_IMPORT unsigned int        ADQ208_SetTrigLevelResetValue(void* adq_cu_ptr, int adq208_num, int resetlevel);
DLL_IMPORT int                 ADQ208_SetTriggerMode(void* adq_cu_ptr, int adq208_num, int trig_mode);
DLL_IMPORT int                 ADQ208_SetSampleWidth(void* adq_cu_ptr, int adq208_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ208_SetNofBits(void* adq_cu_ptr, int adq208_num, unsigned int NofBits);
DLL_IMPORT int                 ADQ208_SetBufferSizePages(void* adq_cu_ptr, int adq208_num, unsigned int pages);
DLL_IMPORT int                 ADQ208_SetBufferSize(void* adq_cu_ptr, int adq208_num, unsigned int samples);
DLL_IMPORT unsigned int        ADQ208_SetExternTrigEdge(void* adq_cu_ptr, int adq208_num, unsigned int edge);
DLL_IMPORT unsigned int         ADQ208_GetExternTrigEdge(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_ArmTrigger(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_DisarmTrigger(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_USBTrig(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_SWTrig(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_CollectDataNextPage(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_CollectRecord(void* adq_cu_ptr, int adq208_num, unsigned int record_num);
DLL_IMPORT unsigned int         ADQ208_GetErrorVector(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT char*               ADQ208_GetPtrData(void* adq_cu_ptr, int adq208_num, int channel);
DLL_IMPORT int                 ADQ208_GetWaitingForTrigger(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetTrigged(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetTriggedAll(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetAcquired(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_GetAcquiredRecords(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int         ADQ208_GetAcquiredRecordsAndLoopCounter(void* adq_cu_ptr, int adq208_num, unsigned int* acquired_records, unsigned int* loop_counter);
DLL_IMPORT int                 ADQ208_GetAcquiredAll(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetLvlTrigLevel(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetLvlTrigFlank(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetLvlTrigEdge(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetLvlTrigChannel(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetTriggedCh(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetClockSource(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_GetTriggerMode(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned long long  ADQ208_GetMaxBufferSize(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_GetMaxBufferSizePages(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int*                ADQ208_GetRevision(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_WriteRegister(void* adq_cu_ptr, int ADQ208_num, int addr, int mask, int data);
DLL_IMPORT unsigned int        ADQ208_ReadRegister(void* adq_cu_ptr, int ADQ208_num, int addr);
DLL_IMPORT unsigned int        ADQ208_MemoryDump(void* adq_cu_ptr, int ADQ208_num,  unsigned int StartAddress, unsigned int EndAddress, unsigned char* buffer, unsigned int *bufctr, unsigned int transfersize);
DLL_IMPORT unsigned int        ADQ208_MemoryShadow(void* adq_cu_ptr, int adq208_num,  void* MemoryArea, unsigned int ByteSize);
DLL_IMPORT unsigned int        ADQ208_MultiRecordSetup(void* adq_cu_ptr, int adq208_num,int NumberOfRecords, int SamplesPerRecord);
DLL_IMPORT unsigned int        ADQ208_MultiRecordSetupGP(void* adq_cu_ptr, int adq208_num,int NumberOfRecords, int SamplesPerRecord, unsigned int* mrinfo);
DLL_IMPORT unsigned int        ADQ208_MultiRecordClose(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_GetSamplesPerPage(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_GetUSBAddress(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_GetPCIeAddress(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_GetTemperature(void* adq_cu_ptr, int adq208_num, unsigned int addr);
DLL_IMPORT unsigned int        ADQ208_WriteEEPROM(void* adq_cu_ptr, int adq208_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int        ADQ208_ReadEEPROM(void* adq_cu_ptr, int adq208_num, unsigned int addr);
DLL_IMPORT unsigned int        ADQ208_WriteEEPROMDB(void* adq_cu_ptr, int adq208_num, unsigned int addr, unsigned int data, unsigned int accesscode);
DLL_IMPORT unsigned int        ADQ208_ReadEEPROMDB(void* adq_cu_ptr, int adq208_num, unsigned int addr);
DLL_IMPORT int                 ADQ208_PllReg(void* adq_cu_ptr, int adq208_num, unsigned int reg_addr, unsigned char val, unsigned char mask);
DLL_IMPORT int                 ADQ208_SetPll(void* adq_cu_ptr, int adq208_num, int n_divider, int r_divider, int vco_divider, int channel_divider);
DLL_IMPORT int                 ADQ208_ADCReg(void* adq_cu_ptr, int adq208_num, unsigned char addr, unsigned char adc, unsigned int val);
DLL_IMPORT int                 ADQ208_SetPreTrigSamples(void* adq_cu_ptr, int adq208_num, unsigned int PreTrigSamples);
DLL_IMPORT int                 ADQ208_SetTriggerHoldOffSamples(void* adq_cu_ptr, int adq208_num, unsigned int TriggerHoldOffSamples);
DLL_IMPORT int                 ADQ208_ResetOverheat(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_SetClockSource(void* adq_cu_ptr, int adq208_num, int source);
DLL_IMPORT char*               ADQ208_GetBoardSerialNumber(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_GetLastError(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_EnableClockRefOut(void* adq_cu_ptr, int ADQ208_num, unsigned char enable);
DLL_IMPORT unsigned int        ADQ208_RegisterNameLookup(void* adq_cu_ptr, int adq208_num, const char* rn, unsigned int* address, unsigned int allow_assertion);
DLL_IMPORT unsigned int        ADQ208_ReadGPIO(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_WriteGPIO(void* adq_cu_ptr, int adq208_num, unsigned int data, unsigned int mask);
DLL_IMPORT int                 ADQ208_SetDirectionGPIO(void* adq_cu_ptr, int adq208_num, unsigned int direction, unsigned int mask);
DLL_IMPORT unsigned int        ADQ208_GetOutputWidth(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_GetNofChannels(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT int                 ADQ208_SetDirectionTrig(void* adq_cu_ptr, int adq208_num, int direction);
DLL_IMPORT int                 ADQ208_WriteTrig(void* adq_cu_ptr, int adq208_num, int data);
DLL_IMPORT unsigned int        ADQ208_SetInternalTriggerPeriod(void* adq_cu_ptr, int ADQ208_num, unsigned int TriggerPeriodClockCycles);
DLL_IMPORT unsigned int        ADQ208_SetTestPatternMode(void* adq_cu_ptr, int adq208_num, int mode);
DLL_IMPORT unsigned int        ADQ208_SetTestPatternConstant(void* adq_cu_ptr, int adq208_num, int value);
DLL_IMPORT unsigned int        ADQ208_SetEthernetPllFreq(void* adq_cu_ptr, int ADQ208_num, unsigned char eth10_freq, unsigned char eth1_freq);
DLL_IMPORT unsigned int        ADQ208_SetPointToPointPllFreq(void* adq_cu_ptr, int ADQ208_num, unsigned char pp_freq);
DLL_IMPORT unsigned int        ADQ208_SetEthernetPll(void* adq_cu_ptr, int ADQ208_num, unsigned short refdiv, unsigned char useref2, unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv, unsigned char eth10_outdiv, unsigned char eth1_outdiv);
DLL_IMPORT unsigned int        ADQ208_SetPointToPointPll(void* adq_cu_ptr, int ADQ208_num, unsigned short refdiv, unsigned char useref2,  unsigned char a, unsigned short b, unsigned char p, unsigned char vcooutdiv, unsigned char pp_outdiv, unsigned char ppsync_outdiv);
DLL_IMPORT unsigned int        ADQ208_SetDirectionMLVDS(void* adq_cu_ptr, int ADQ208_num, unsigned char direction);
DLL_IMPORT unsigned int        ADQ208_StorePCIeConfig(void* adq_cu_ptr, int ADQ208_num, UINT32* pci_space);
DLL_IMPORT unsigned int        ADQ208_ReloadPCIeConfig(void* adq_cu_ptr, int ADQ208_num, UINT32* pci_space);
DLL_IMPORT int                ADQ208_IsUSBDevice(void* adq_cu_ptr, int ADQ208_num);
DLL_IMPORT int                ADQ208_IsPCIeDevice(void* adq_cu_ptr, int ADQ208_num);
DLL_IMPORT const char*         ADQ208_GetNGCPartNumber(void* adq_cu_ptr, int ADQ208_num);
DLL_IMPORT const char*         ADQ208_GetUserLogicPartNumber(void* adq_cu_ptr, int ADQ208_num);
DLL_IMPORT int                 ADQ208_SetInterleavingMode(void* adq_cu_ptr, int ADQ208_num, char interleaving);
DLL_IMPORT const char*         ADQ208_GetCardOption(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_SetSampleSkip(void* adq_cu_ptr, int adq208_num, unsigned int skipfactor);
DLL_IMPORT unsigned int        ADQ208_GetPCIeLinkWidth(void* adq_cu_ptr, int ADQ208_num);
DLL_IMPORT unsigned int        ADQ208_GetPCIeLinkRate(void* adq_cu_ptr, int ADQ208_num);
DLL_IMPORT int                 ADQ208_Blink(void* adq_cu_ptr, int ADQ208_num);
DLL_IMPORT unsigned int        ADQ208_GetMaxNofRecordsFromNofSamples(void* adq_cu_ptr, int ADQ208_num, unsigned int NofSamples, unsigned int* MaxNofRecords);
DLL_IMPORT unsigned int        ADQ208_GetMaxNofSamplesFromNofRecords(void* adq_cu_ptr, int ADQ208_num, unsigned int NofRecords, unsigned int* MaxNofSamples);
DLL_IMPORT unsigned int        ADQ208_SPISend(void* adq_cu_ptr, int adq208_num, unsigned char addr, const char* data, unsigned char length, unsigned int negedge, unsigned int* ret);
DLL_IMPORT int                 ADQ208_SetDelayLineValues(void* adq_cu_ptr, int adq208_num, int samplerate, unsigned int linear_interpolation);
DLL_IMPORT int                 ADQ208_SetDelayLineValuesDirect(void* adq_cu_ptr, int adq208_num, unsigned int delay1, unsigned int delay2);
DLL_IMPORT unsigned int        ADQ208_GetPPTStatus(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_InitPPT(void* adq_cu_ptr, int adq208_num);
DLL_IMPORT unsigned int        ADQ208_SetPPTActive(void* adq_cu_ptr, int adq208_num, unsigned int active);
DLL_IMPORT unsigned int        ADQ208_SetPPTInitOffset(void* adq_cu_ptr, int adq208_num, unsigned int init_offset);
DLL_IMPORT unsigned int        ADQ208_SetPPTPeriod(void* adq_cu_ptr, int adq208_num, unsigned int period);
DLL_IMPORT unsigned int        ADQ208_SetPPTBurstMode(void* adq_cu_ptr, int adq208_num, unsigned int burst_mode);
DLL_IMPORT unsigned int        ADQ208_WriteUserRegister(void* adq_cu_ptr, int ADQ208_num, unsigned int ul_target, unsigned int regnum, unsigned int mask, unsigned int data, unsigned int* retval);
DLL_IMPORT unsigned int        ADQ208_ReadUserRegister(void* adq_cu_ptr, int ADQ208_num, unsigned int ul_target, unsigned int regnum, unsigned int* retval);
DLL_IMPORT const char* ADQ208_GetADQDSPOption(void* adq_cu_ptr, int ADQ208_num);
DLL_IMPORT unsigned int ADQ208_GetProductFamily(void* adq_cu_ptr, int ADQ208_num, unsigned int* family);
DLL_IMPORT unsigned int ADQ208_SetupLevelTrigger(void* adq_cu_ptr, int ADQ208_num, int* level, int* edge, int* resetlevel, unsigned int channel, unsigned int individual_mode);
DLL_IMPORT unsigned int ADQ208_SetConfigurationTrig(void* adq_cu_ptr, int ADQ208_num, unsigned int mode, unsigned int pulselength, unsigned int invertoutput);
DLL_IMPORT unsigned int ADQ208_GetPCIeTLPSize(void* adq_cu_ptr, int ADQ208_num);

#ifdef __cplusplus
}
#endif


#endif //ADQAPI - use #endif! File is merged during build.
