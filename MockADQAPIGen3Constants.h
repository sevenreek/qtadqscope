#ifndef MOCKADQAPIGEN3CONSTANTS_H
#define MOCKADQAPIGEN3CONSTANTS_H
#include <cstdint>

#define ADQ_EOK (0) /* OK */
#define ADQ_EINVAL (-1) /* Invalid argument */
#define ADQ_EAGAIN (-2) /* Resource temporarily unavailable */
#define ADQ_EOVERFLOW (-3) /* Overflow */
#define ADQ_ENOTREADY (-4) /* Resource not ready */
#define ADQ_EINTERRUPTED (-5) /* Operation interrupted */
#define ADQ_EIO (-6) /* I/O error */
#define ADQ_EEXTERNAL (-7) /* External errors, e.g. from OS-level operations */
#define ADQ_EUNSUPPORTED (-8) /* Operation not supported by the device. */
#define ADQ_EINTERNAL (-9) /* Internal errors, cannot be addressed by the user. */

enum ADQAPIObjectID
{
  ADQAPI_OBJECT_RESERVED = 0, /**< Unused */
  ADQAPI_OBJECT_ATD_WFA_STRUCT = 1, /**< struct ATDWFABufferStruct */
  ADQAPI_OBJECT_ADQ_RECORD_HEADER = 2, /**< struct ADQRecordHeader */
  ADQAPI_OBJECT_ADQ_INFO_LIST_ENTRY = 3, /**< struct ADQInfoListEntry */
  ADQAPI_OBJECT_ADQ_INFO_LIST_PRE_ALLO_ARRAY = 4, /**< struct ADQInfoListPreAlloArray */
  ADQAPI_OBJECT_SD_CARD_CONFIGURATION = 5, /**< struct SDCardConfiguration */
  ADQAPI_OBJECT_ADQ_DAISY_CHAIN_TRIGGER_INFORMATION = 7, /**< struct ADQDaisyChainTriggerInformation*/
  ADQAPI_OBJECT_ADQ_DAISY_CHAIN_DEVICE_INFORMATION = 8, /**< struct ADQDaisyChainDeviceInformation */
};

enum ATDWFABufferFormat
{
  ATD_WFA_BUFFER_FORMAT_INT32 = 0, /**< int_32t (default) */
  ATD_WFA_BUFFER_FORMAT_STRUCT = 1 /**< struct ATDWFABufferStruct */
};

struct ATDWFABufferStruct
{
  uint64_t Timestamp; /**< Timestamp of first trigger in accumulation */
  int32_t *Data; /**< Pointer to data array. Memory is manage by user application */
  uint32_t RecordNumber; /**< Starts at 1 */
  uint32_t Status;
  uint32_t RecordsAccumulated; /**< Number of accumulated records in current batch */
  uint8_t Channel; /**< Indexed from 1 */
};

enum ADQProductID_Enum
{
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
  PID_EV12AS350_EVM = 0x0022,
  PID_ADQ7 = 0x0023,
  PID_ADQ8 = 0x0026,
  PID_ADQ12 = 0x0027,
  PID_ADQ7Virtual = 0x0030,
  PID_ADQ3 = 0x0031,
  PID_ADQSM = 0x0032,
  PID_TX320 = 0x201A,
  PID_RX320 = 0x201C,
  PID_S6000 = 0x2019
};

enum ADQHWIFEnum
{
  HWIF_USB = 0, /**< USB2 */
  HWIF_PCIE = 1, /**< PCIe */
  HWIF_USB3 = 2, /**< USB3 */
  HWIF_PCIELITE = 3, /**< PCIe lite */
  HWIF_ETH_ADQ7 = 4, /**< 10Gb Ethernet for ADQ7 */
  HWIF_ETH_ADQ14 = 5, /**< 10Gb Ethernet for ADQ7 */
  HWIF_VIRTUAL = 6, /**< Virtual device */
  HWIF_QPCIE = 7, /**< PCIe */
  HWIF_OTHER = 8/**< Reserved */
};

enum ADQCommunicationInterface
{
  ADQ_COMMUNICATION_INTERFACE_INVALID = 0,
  ADQ_COMMUNICATION_INTERFACE_PCIE = 1,
  ADQ_COMMUNICATION_INTERFACE_USB = 2,
  ADQ_COMMUNICATION_INTERFACE_MAX_VAL = INT32_MAX
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
  struct ADQInfoListEntry ADQlistArray[128];
};

struct ADQRecordHeader
{
  uint8_t RecordStatus; /**< Status of record. */
  uint8_t UserID; /**< ID set by user. */
  uint8_t Channel; /**< Channel. */
  uint8_t DataFormat; /**< Data format. */
  uint32_t SerialNumber; /**< Device serial. */
  uint32_t RecordNumber; /**< Record number. */
  int32_t SamplePeriod; /**< Sample period [ps]. */
  uint64_t Timestamp; /**< Record timestamp. */
  int64_t RecordStart; /**< Record start. */
  uint32_t RecordLength; /**< Record length [samples]. */
  uint16_t GeneralPurpose0; /**< General purpose 0. */
  uint16_t GeneralPurpose1; /**< General purpose 1. */
};

struct ADQRecord
{
  struct ADQRecordHeader *header;
  void *data;
  uint64_t size;
};

struct ADQDaisyChainTriggerInformation
{
  uint64_t Timestamp;
  int64_t *RecordStart;
  double *ExtendedPrecision;
};

struct ADQDaisyChainDeviceInformation
{
  int64_t SampleRate;
  int64_t Position;
  int64_t PretriggerSamples;
  int64_t TriggerDelaySamples;
};

struct SDCardConfiguration
{
  unsigned int Version;
  unsigned int Valid;
  unsigned int Length;

  char SerialNumber[16];
  char ISODate[32];

  unsigned int NumberOfChannels;
  unsigned int ChannelMask;
  unsigned int NumberOfParallelSamples;
  unsigned int CyclicBuffersEnabled;
  unsigned int ChunkSize;
  unsigned int TriggerMode;
  int DaisyChainPosition;

  unsigned int CBufSize;
  unsigned int CBufMemArea;

  unsigned int RecordLength[8];
  unsigned int NumberOfRecords[8];
  unsigned int SampleSkip[8];
  unsigned int PreTrigger[8];
  unsigned int TriggerDelay[8];
};

#define ADQAPI_VERSION_MAJOR 4
#define ADQAPI_VERSION_MINOR 0

#define ADQ_MAX_NOF_CHANNELS 8
#define ADQ_MAX_NOF_BUFFERS 16
#define ADQ_MAX_NOF_PORTS 8
#define ADQ_MAX_NOF_PINS 16
#define ADQ_MAX_NOF_ADC_CORES 4
#define ADQ_MAX_NOF_INPUT_RANGES 8
#define ADQ_MAX_NOF_PATTERN_GENERATORS 2
#define ADQ_MAX_NOF_PULSE_GENERATORS 4
#define ADQ_MAX_NOF_PATTERN_INSTRUCTIONS 16
#define ADQ_MAX_NOF_TEMPERATURE_SENSORS 16
#define ADQ_MAX_NOF_FILTER_COEFFICIENTS 10

#define ADQ_MARKER_BYTES_WRITTEN_INDEX 0
#define ADQ_MARKER_NOF_RECORDS_INDEX 1
#define ADQ_MARKER_STATUS_INDEX 2
#define ADQ_MARKER_DRAM_LEVEL_INDEX 3
#define ADQ_MARKER_COUNTER_INDEX 4

#define ADQ_MARKER_STATUS_OVERFLOW_POS (0)
#define ADQ_MARKER_STATUS_OVERFLOW (1u << ADQ_MARKER_STATUS_OVERFLOW_POS)
#define ADQ_MARKER_STATUS_FLUSH_POS (1)
#define ADQ_MARKER_STATUS_FLUSH (1u << ADQ_MARKER_STATUS_FLUSH_POS)

#define ADQ_ANY_CHANNEL (-1)
#define ADQ_INFINITE_RECORD_LENGTH (-1)
#define ADQ_INFINITE_NOF_RECORDS (-1)

enum ADQStatusId
{
  ADQ_STATUS_ID_RESERVED = 0,
  ADQ_STATUS_ID_OVERFLOW = 1,
  ADQ_STATUS_ID_DRAM = 2,
  ADQ_STATUS_ID_ACQUISITION = 3,
  ADQ_STATUS_ID_TEMPERATURE = 4,
  ADQ_STATUS_ID_PACKET_OVERFLOW = 65537,
  ADQ_STATUS_ID_MEMCOM_OVERFLOW = 65538
};

struct ADQOverflowStatus
{
  int32_t overflow;
  int32_t reserved;
};

struct ADQDramStatus
{
  uint64_t fill;
  uint64_t fill_max;
};

struct ADQAcquisitionStatus
{
  int64_t acquired_records[ADQ_MAX_NOF_CHANNELS];
};

struct ADQTemperatureStatusSensor
{
  char label[32];
  float value;
};

struct ADQTemperatureStatus
{
  int32_t nof_sensors;
  struct ADQTemperatureStatusSensor sensor[ADQ_MAX_NOF_TEMPERATURE_SENSORS];
};

enum ADQParameterId
{
  ADQ_PARAMETER_ID_RESERVED = 0,
  ADQ_PARAMETER_ID_DATA_ACQUISITION = 1,
  ADQ_PARAMETER_ID_DATA_TRANSFER = 2,
  ADQ_PARAMETER_ID_DATA_READOUT = 3,
  ADQ_PARAMETER_ID_CONSTANT = 4,
  ADQ_PARAMETER_ID_DIGITAL_GAINANDOFFSET = 5,
  ADQ_PARAMETER_ID_EVENT_SOURCE_LEVEL = 6,
  ADQ_PARAMETER_ID_DBS = 7,
  ADQ_PARAMETER_ID_SAMPLE_SKIP = 8,
  ADQ_PARAMETER_ID_TEST_PATTERN = 9,
  ADQ_PARAMETER_ID_EVENT_SOURCE_PERIODIC = 10,
  ADQ_PARAMETER_ID_EVENT_SOURCE_TRIG = 11,
  ADQ_PARAMETER_ID_EVENT_SOURCE_SYNC = 12,
  ADQ_PARAMETER_ID_ANALOG_FRONTEND = 13,
  ADQ_PARAMETER_ID_PATTERN_GENERATOR0 = 14,
  ADQ_PARAMETER_ID_PATTERN_GENERATOR1 = 15,
  ADQ_PARAMETER_ID_EVENT_SOURCE = 16,
  ADQ_PARAMETER_ID_SIGNAL_PROCESSING = 17,
  ADQ_PARAMETER_ID_FUNCTION = 18,
  ADQ_PARAMETER_ID_TOP = 19,
  ADQ_PARAMETER_ID_PORT_TRIG = 20,
  ADQ_PARAMETER_ID_PORT_SYNC = 21,
  ADQ_PARAMETER_ID_PORT_SYNCO = 22,
  ADQ_PARAMETER_ID_PORT_SYNCI = 23,
  ADQ_PARAMETER_ID_PORT_CLK = 24,
  ADQ_PARAMETER_ID_PORT_CLKI = 25,
  ADQ_PARAMETER_ID_PORT_CLKO = 26,
  ADQ_PARAMETER_ID_PORT_GPIOA = 27,
  ADQ_PARAMETER_ID_PORT_GPIOB = 28,
  ADQ_PARAMETER_ID_PORT_PXIE = 29,
  ADQ_PARAMETER_ID_PORT_MTCA = 30,
  ADQ_PARAMETER_ID_PULSE_GENERATOR0 = 31,
  ADQ_PARAMETER_ID_PULSE_GENERATOR1 = 32,
  ADQ_PARAMETER_ID_PULSE_GENERATOR2 = 33,
  ADQ_PARAMETER_ID_PULSE_GENERATOR3 = 34,
  ADQ_PARAMETER_ID_TIMESTAMP_SYNCHRONIZATION = 35,
  ADQ_PARAMETER_ID_FIR_FILTER = 36,
  ADQ_PARAMETER_ID_CLOCK_SYSTEM = 40,
#ifdef ADQAPI_INTERNAL
  ADQ_PARAMETER_ID_INTERNAL_DIGITAL_GAINANDOFFSET = 65536,
#endif
  ADQ_PARAMETER_ID_MAX_VAL = INT32_MAX
};

#define ADQ_PARAMETERS_MAGIC (0xAA559977AA559977ull)
#define ADQ_PARAMETERS_MAX_SIZE (sizeof(struct ADQParameters))

enum ADQEventSource
{
  ADQ_EVENT_SOURCE_INVALID = 0,
  ADQ_EVENT_SOURCE_SOFTWARE = 1,
  ADQ_EVENT_SOURCE_TRIG = 2,
  ADQ_EVENT_SOURCE_LEVEL = 3,
  ADQ_EVENT_SOURCE_PERIODIC = 4,
  ADQ_EVENT_SOURCE_PXIE_STARB = 6,
  ADQ_EVENT_SOURCE_TRIG2 = 7,
  ADQ_EVENT_SOURCE_TRIG3 = 8,
  ADQ_EVENT_SOURCE_SYNC = 9,
  ADQ_EVENT_SOURCE_MTCA_MLVDS = 10,
  ADQ_EVENT_SOURCE_TRIG_GATED_SYNC = 11,
  ADQ_EVENT_SOURCE_TRIG_CLKREF_SYNC = 12,
  ADQ_EVENT_SOURCE_MTCA_MLVDS_CLKREF_SYNC = 13,
  ADQ_EVENT_SOURCE_PXI_TRIG = 14,
  ADQ_EVENT_SOURCE_PXIE_STARB_CLKREF_SYNC = 16,
  ADQ_EVENT_SOURCE_SYNC_CLKREF_SYNC = 19,
  ADQ_EVENT_SOURCE_DAISY_CHAIN = 23,
  ADQ_EVENT_SOURCE_SOFTWARE_CLKREF_SYNC = 24,
  ADQ_EVENT_SOURCE_GPIOA0 = 25,
  ADQ_EVENT_SOURCE_GPIOA1 = 26,
  ADQ_EVENT_SOURCE_LEVEL_CHANNEL0 = 100,
  ADQ_EVENT_SOURCE_LEVEL_CHANNEL1 = 101,
  ADQ_EVENT_SOURCE_LEVEL_CHANNEL2 = 102,
  ADQ_EVENT_SOURCE_LEVEL_CHANNEL3 = 103,
  ADQ_EVENT_SOURCE_LEVEL_CHANNEL4 = 104,
  ADQ_EVENT_SOURCE_LEVEL_CHANNEL5 = 105,
  ADQ_EVENT_SOURCE_LEVEL_CHANNEL6 = 106,
  ADQ_EVENT_SOURCE_LEVEL_CHANNEL7 = 107,
  ADQ_EVENT_SOURCE_MAX_VAL = INT32_MAX
};

enum ADQTestPatternSource
{
  ADQ_TEST_PATTERN_SOURCE_DISABLE = 0, /**< Normal operation, no test pattern. */
  ADQ_TEST_PATTERN_SOURCE_COUNT_UP = 1, /**< Sawtooth counting up */
  ADQ_TEST_PATTERN_SOURCE_COUNT_DOWN = 2, /**< Sawtooth counting down */
  ADQ_TEST_PATTERN_SOURCE_TRIANGLE = 3, /**< Triangle wave */
  ADQ_TEST_PATTERN_SOURCE_PULSE = 4, /**< Regular pulse */
  ADQ_TEST_PATTERN_SOURCE_PULSE_PRBS_WIDTH = 5, /**< PRBS width pulse */
  ADQ_TEST_PATTERN_SOURCE_PULSE_PRBS_AMPLITUDE = 6, /**< PRBS amplitude pulse */
  ADQ_TEST_PATTERN_SOURCE_PULSE_PRBS_WIDTH_AMPLITUDE = 7, /**< PRBS width and amplitude pulse */
  ADQ_TEST_PATTERN_SOURCE_PULSE_NOISE = 8, /**< Regular pulse with noise */
  ADQ_TEST_PATTERN_SOURCE_PULSE_NOISE_PRBS_WIDTH = 9, /**< PRBS width pulse with noise */
  ADQ_TEST_PATTERN_SOURCE_PULSE_NOISE_PRBS_AMPLITUDE = 10, /**< PRBS amplitude pulse with noise */
  ADQ_TEST_PATTERN_SOURCE_PULSE_NOISE_PRBS_WIDTH_AMPLITUDE = 11, /**< PRBS width and amplitude pulse with noise */
  ADQ_TEST_PATTERN_SOURCE_MAX_VAL = INT32_MAX
};

enum ADQPort
{
  ADQ_PORT_TRIG = 0,
  ADQ_PORT_SYNC = 1,
  ADQ_PORT_SYNCO = 2,
  ADQ_PORT_SYNCI = 3,
  ADQ_PORT_CLK = 4,
  ADQ_PORT_CLKI = 5,
  ADQ_PORT_CLKO = 6,
  ADQ_PORT_GPIOA = 7,
  ADQ_PORT_GPIOB = 8,
  ADQ_PORT_PXIE = 9,
  ADQ_PORT_MTCA = 10,
  ADQ_PORT_MAX_VAL = INT32_MAX
};

enum ADQPinPxie
{
  ADQ_PIN_PXIE_TRIG0 = 0,
  ADQ_PIN_PXIE_TRIG1 = 1,
  ADQ_PIN_PXIE_STARA = 2,
  ADQ_PIN_PXIE_STARB = 3,
  ADQ_PIN_PXIE_MAX_VAL = INT32_MAX
};

enum ADQPinMtca
{
  ADQ_PIN_MTCA_R17 = 0,
  ADQ_PIN_MTCA_T17 = 1,
  ADQ_PIN_MTCA_R18 = 2,
  ADQ_PIN_MTCA_T18 = 3,
  ADQ_PIN_MTCA_R19 = 4,
  ADQ_PIN_MTCA_T19 = 5,
  ADQ_PIN_MTCA_R20 = 6,
  ADQ_PIN_MTCA_T20 = 7,
  ADQ_PIN_MTCA_MAX_VAL = INT32_MAX
};

enum ADQImpedance
{
  ADQ_IMPEDANCE_50_OHM = 0,
  ADQ_IMPEDANCE_HIGH = 1,
  ADQ_IMPEDANCE_MAX_VAL = INT32_MAX
};

enum ADQDirection
{
  ADQ_DIRECTION_IN = 0,
  ADQ_DIRECTION_OUT = 1,
  ADQ_DIRECTION_INOUT = 2,
  ADQ_DIRECTION_MAXVAL = INT32_MAX
};

enum ADQEdge
{
  ADQ_EDGE_FALLING = 0,
  ADQ_EDGE_RISING = 1,
  ADQ_EDGE_BOTH = 2,
  ADQ_EDGE_MAX_VAL = INT32_MAX
};

enum ADQClockSource
{
  ADQ_CLOCK_SOURCE_INVALID = -1,
  ADQ_CLOCK_SOURCE_INTREF = 0,
  ADQ_CLOCK_SOURCE_EXTREF = 1,
  ADQ_CLOCK_SOURCE_EXTCLK = 2,
  ADQ_CLOCK_SOURCE_PXIE_10M = 3,
  ADQ_CLOCK_SOURCE_MTCA_TCLKA = 4,
  ADQ_CLOCK_SOURCE_MTCA_TCLKB = 5,
  ADQ_CLOCK_SOURCE_PXIE_100M = 6,
  ADQ_CLOCK_SOURCE_EXTREF_LOWJITTER = 7,
  ADQ_CLOCK_SOURCE_EXTREF_DELAY = 8,
  ADQ_CLOCK_SOURCE_MAX_VAL = INT32_MAX
};

enum ADQClockReferenceSource
{
  ADQ_CLOCK_REFERENCE_SOURCE_INVALID = 0,
  ADQ_CLOCK_REFERENCE_SOURCE_INTERNAL = 1,
  ADQ_CLOCK_REFERENCE_SOURCE_PORT_CLK = 2,
  ADQ_CLOCK_REFERENCE_SOURCE_PXIE_10M = 3,
  ADQ_CLOCK_REFERENCE_SOURCE_MTCA_TCLKA = 4,
  ADQ_CLOCK_REFERENCE_SOURCE_MTCA_TCLKB = 5,
  ADQ_CLOCK_REFERENCE_SOURCE_PXIE_100M = 6,
  ADQ_CLOCK_REFERENCE_SOURCE_MAX_VAL = INT32_MAX
};

enum ADQClockGenerator
{
  ADQ_CLOCK_GENERATOR_INVALID = 0,
  ADQ_CLOCK_GENERATOR_INTERNAL_PLL = 1,
  ADQ_CLOCK_GENERATOR_EXTERNAL_CLOCK = 2,
  ADQ_CLOCK_GENERATOR_MAX_VAL = INT32_MAX
};

enum ADQFunction
{
  ADQ_FUNCTION_INVALID = 0,
  ADQ_FUNCTION_PATTERN_GENERATOR0 = 1,
  ADQ_FUNCTION_PATTERN_GENERATOR1 = 2,
  ADQ_FUNCTION_GPIO = 3,
  ADQ_FUNCTION_PULSE_GENERATOR0 = 4,
  ADQ_FUNCTION_PULSE_GENERATOR1 = 5,
  ADQ_FUNCTION_PULSE_GENERATOR2 = 6,
  ADQ_FUNCTION_PULSE_GENERATOR3 = 7,
  ADQ_FUNCTION_TIMESTAMP_SYNCHRONIZATION = 8,
  ADQ_FUNCTION_MAX_VAL = INT32_MAX
};

enum ADQRoundingMethod
{
  ADQ_ROUNDING_METHOD_TIE_AWAY_FROM_ZERO = 0,
  ADQ_ROUNDING_METHOD_TIE_TOWARDS_ZERO = 1,
  ADQ_ROUNDING_METHOD_TIE_TO_EVEN = 2,
  ADQ_ROUNDING_METHOD_MAX_VAL = INT32_MAX
};

enum ADQCoefficientFormat
{
  ADQ_COEFFICIENT_FORMAT_DOUBLE = 0,
  ADQ_COEFFICIENT_FORMAT_FIXED_POINT = 1,
  ADQ_COEFFICIENT_FORMAT_MAX_VAL = INT32_MAX
};

enum ADQUserLogic
{
  ADQ_USER_LOGIC_RESERVED = 0,
  ADQ_USER_LOGIC1 = 1,
  ADQ_USER_LOGIC2 = 2,
  ADQ_USER_LOGIC_MAX_VAL = INT32_MAX
};

#define ADQ_GEN4_RECORD_HEADER_MISC_PATTERN_GENERATOR_POS (0)
#define ADQ_GEN4_RECORD_HEADER_MISC_PATTERN_GENERATOR_MASK (0x0Fu << ADQ_GEN4_RECORD_HEADER_MISC_PATTERN_GENERATOR_POS)

struct ADQGen4RecordHeader
{
  uint8_t version_major;
  uint8_t version_minor;
  uint16_t timestamp_synchronization_counter;
  uint16_t general_purpose_start;
  uint16_t general_purpose_stop;
  uint64_t timestamp;
  int64_t record_start;
  uint32_t record_length;
  uint8_t user_id;
  uint8_t misc;
  uint16_t record_status;
  uint32_t record_number;
  uint8_t channel;
  uint8_t data_format;
  char serial_number[10];
  uint64_t sampling_period;
  float time_unit;
  uint32_t reserved;
};

struct ADQGen4RecordHeaderRaw
{
  uint8_t version_major;
  uint8_t version_minor;
  uint16_t timestamp_synchronization_counter;
  uint16_t general_purpose_start;
  uint16_t general_purpose_stop;
  uint64_t timestamp;
  uint32_t trigger_vector;
  uint8_t record_start_index;
  uint8_t pad0[3];
  uint32_t record_length;
  uint8_t user_id;
  uint8_t misc;
  uint16_t record_status;
  uint32_t record_number;
  uint8_t channel;
  uint8_t data_format;
  char serial_number[10];
  uint64_t sampling_period;
  float time_unit;
  uint32_t reserved;
};

struct ADQGen4Record
{
  struct ADQGen4RecordHeader *header;
  void *data;
  uint64_t size;
};

struct ADQClockSystemParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  enum ADQClockGenerator clock_generator;
  enum ADQClockReferenceSource reference_source;
  double sampling_frequency;
  double reference_frequency;
  double delay_adjustment;
  int32_t low_jitter_mode_enabled;
  int32_t delay_adjustment_enabled;
  uint64_t magic;
};

enum ADQPatternGeneratorOperation
{
  ADQ_PATTERN_GENERATOR_OPERATION_TIMER = 0,
  ADQ_PATTERN_GENERATOR_OPERATION_EVENT = 1,
  ADQ_PATTERN_GENERATOR_OPERATION_MAX_VAL = INT32_MAX
};

struct ADQPatternGeneratorInstruction
{
  int64_t count;
  int64_t count_prescaling;
  enum ADQPatternGeneratorOperation op;
  enum ADQEventSource source;
  enum ADQEdge source_edge;
  enum ADQEventSource reset_source;
  enum ADQEdge reset_source_edge;
  int32_t output_value;
  int32_t output_value_transition;
  int32_t reserved;
};

struct ADQPatternGeneratorParameters
{
  enum ADQParameterId id;
  int32_t nof_instructions;
  struct ADQPatternGeneratorInstruction instruction[ADQ_MAX_NOF_PATTERN_INSTRUCTIONS];
  uint64_t magic;
};

struct ADQPulseGeneratorParameters
{
  enum ADQParameterId id;
  enum ADQEventSource source;
  enum ADQEdge edge;
  int32_t reserved;
  int64_t length;
  uint64_t magic;
};

struct ADQDataAcquisitionParametersCommon
{
  int64_t reserved;
};

struct ADQDataAcquisitionParametersChannel
{
  int64_t horizontal_offset;
  int64_t record_length;
  int64_t nof_records;
  enum ADQEventSource trigger_source;
  enum ADQEdge trigger_edge;
  enum ADQFunction trigger_blocking_source;
  int32_t reserved;
};

struct ADQDataAcquisitionParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQDataAcquisitionParametersCommon common;
  struct ADQDataAcquisitionParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

enum ADQMarkerMode
{
  ADQ_MARKER_MODE_HOST_AUTO = 0,
  ADQ_MARKER_MODE_HOST_MANUAL = 1,
  ADQ_MARKER_MODE_USER_ADDR = 2,
  ADQ_MARKER_MODE_MAX_VAL = INT32_MAX
};

enum ADQMemoryOwner
{
  ADQ_MEMORY_OWNER_API = 0,
  ADQ_MEMORY_OWNER_USER = 1,
  ADQ_MEMORY_OWNER_MAX_VAL = INT32_MAX
};

struct ADQDataTransferParametersCommon
{
  int64_t record_buffer_packed_size;
  int64_t metadata_buffer_packed_size;
  enum ADQMarkerMode marker_mode;
  int32_t write_lock_enabled;
  int32_t transfer_records_to_host_enabled;
  int32_t packed_buffers_enabled;
};

struct ADQDataTransferParametersChannel
{
  uint64_t record_buffer_bus_address[ADQ_MAX_NOF_BUFFERS];
  uint64_t metadata_buffer_bus_address[ADQ_MAX_NOF_BUFFERS];
  uint64_t marker_buffer_bus_address[ADQ_MAX_NOF_BUFFERS];
  int64_t nof_buffers;
  int64_t record_size;
  int64_t record_buffer_size;
  int64_t metadata_buffer_size;
  int64_t record_buffer_packed_offset;
  int64_t metadata_buffer_packed_offset;
  volatile void *record_buffer[ADQ_MAX_NOF_BUFFERS];
  volatile void *metadata_buffer[ADQ_MAX_NOF_BUFFERS];
  volatile uint32_t *marker_buffer[ADQ_MAX_NOF_BUFFERS];
  int32_t record_length_infinite_enabled;
  int32_t metadata_enabled;
};

struct ADQDataTransferParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQDataTransferParametersCommon common;
  struct ADQDataTransferParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

struct ADQDataReadoutParametersCommon
{
  enum ADQMemoryOwner memory_owner;
  int32_t reserved;
};

struct ADQDataReadoutParametersChannel
{
  int64_t nof_record_buffers_max;
  int64_t record_buffer_size_max;
  int64_t record_buffer_size_increment;
  int32_t incomplete_records_enabled;
  int32_t bytes_per_sample;
};

struct ADQDataReadoutParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQDataReadoutParametersCommon common;
  struct ADQDataReadoutParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

#define ADQ_DATA_READOUT_STATUS_FLAGS_OK (0)
#define ADQ_DATA_READOUT_STATUS_FLAGS_STARVING (1u << 0)
#define ADQ_DATA_READOUT_STATUS_FLAGS_INCOMPLETE (1u << 1)

struct ADQDataReadoutStatus
{
  uint32_t flags;
};

struct ADQP2pStatusChannel
{
  uint32_t flags;
  int32_t nof_completed_buffers;
  int16_t completed_buffers[ADQ_MAX_NOF_BUFFERS];
};

struct ADQP2pStatus
{
  struct ADQP2pStatusChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint32_t flags;
  int32_t reserved;
};

struct ADQConstantParametersCommunicationInterface
{
  enum ADQCommunicationInterface type;
  int32_t link_width;
  int32_t link_generation;
  int32_t reserved;
};

struct ADQConstantParametersFirFilter
{
  int32_t is_present;
  int32_t order;
  int32_t nof_coefficients;
  int32_t coefficient_bits;
  int32_t coefficient_fractional_bits;
  int32_t reserved;
};

struct ADQConstantParametersChannel
{
  double base_sampling_rate;
  double input_range[ADQ_MAX_NOF_INPUT_RANGES];
  char label[8];
  int32_t nof_adc_cores;
  int32_t nof_input_ranges;
  int32_t has_variable_dc_offset;
  int32_t has_variable_input_range;
  struct ADQConstantParametersFirFilter fir_filter;
};

struct ADQConstantParametersPin
{
  enum ADQEventSource event_source;
  enum ADQDirection direction;
  int32_t has_configurable_threshold;
  int32_t reserved;
};

struct ADQConstantParametersPort
{
  int32_t nof_pins;
  int32_t is_present;
  char label[16];
  struct ADQConstantParametersPin pin[ADQ_MAX_NOF_PINS];
};

struct ADQConstantParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQClockSystemParameters clock_system;
  int32_t nof_channels;
  int32_t nof_pattern_generators;
  int32_t max_nof_pattern_generator_instructions;
  int32_t nof_pulse_generators;
  char dna[40];
  char serial_number[16];
  char product_name[32];
  char product_options[32];
  char firmware_name[32];
  char firmware_revision[16];
  char firmware_type[16];
  struct ADQConstantParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  struct ADQConstantParametersPort port[ADQ_MAX_NOF_PORTS];
  struct ADQConstantParametersCommunicationInterface communication_interface;
  uint64_t magic;
};

#define ADQ_UNITY_GAIN (1024)

struct ADQDigitalGainAndOffsetParametersChannel
{
  int64_t gain;
  int64_t offset;
};

struct ADQDigitalGainAndOffsetParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQDigitalGainAndOffsetParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

struct ADQEventSourceLevelParametersChannel
{
  int64_t level;
  int64_t arm_hysteresis;
};

struct ADQEventSourceLevelParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQEventSourceLevelParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

struct ADQDbsParametersChannel
{
  int64_t level;
  int64_t lower_saturation_level;
  int64_t upper_saturation_level;
  int32_t bypass;
  int32_t reserved;
};

struct ADQDbsParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQDbsParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

struct ADQSampleSkipParametersChannel
{
  int64_t skip_factor;
};

struct ADQSampleSkipParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQSampleSkipParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

struct ADQFirFilterParametersChannel
{
  enum ADQRoundingMethod rounding_method;
  enum ADQCoefficientFormat format;
  double coefficient[ADQ_MAX_NOF_FILTER_COEFFICIENTS];
  int32_t coefficient_fixed_point[ADQ_MAX_NOF_FILTER_COEFFICIENTS];
};

struct ADQFirFilterParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQFirFilterParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

struct ADQTestPatternParametersPulse
{
  int64_t baseline;
  int64_t amplitude;
  int64_t period;
  int64_t width;
  int64_t nof_pulses_in_burst;
  int64_t nof_bursts;
  int64_t burst_period;
  int64_t prbs_amplitude_seed;
  int64_t prbs_amplitude_scale;
  int64_t prbs_width_seed;
  int64_t prbs_width_scale;
  int64_t prbs_noise_seed;
  int64_t prbs_noise_scale;
  int32_t trigger_mode_enabled;
  int32_t reserved;
};

struct ADQTestPatternParametersChannel
{
  enum ADQTestPatternSource source;
  int32_t reserved;
  struct ADQTestPatternParametersPulse pulse;
};

struct ADQTestPatternParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQTestPatternParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

struct ADQEventSourcePortParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  double threshold;
  uint64_t magic;
};

struct ADQEventSourcePeriodicParameters
{
  enum ADQParameterId id;
  enum ADQEventSource synchronization_source;
  int64_t period;
  int64_t high;
  int64_t low;
  double frequency;
  uint64_t magic;
};

struct ADQPortParametersPin
{
  enum ADQImpedance input_impedance;
  enum ADQDirection direction;
  enum ADQFunction function;
  int32_t value;
  int32_t invert_output;
  int32_t reserved;
};

struct ADQPortParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQPortParametersPin pin[ADQ_MAX_NOF_PINS];
  uint64_t magic;
};

struct ADQAnalogFrontendParametersChannel
{
  double input_range;
  double dc_offset;
};

struct ADQAnalogFrontendParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQAnalogFrontendParametersChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};

enum ADQTimestampSynchronizationMode
{
  ADQ_TIMESTAMP_SYNCHRONIZATION_MODE_DISABLE = 0,
  ADQ_TIMESTAMP_SYNCHRONIZATION_MODE_FIRST = 1,
  ADQ_TIMESTAMP_SYNCHRONIZATION_MODE_ALL = 2,
  ADQ_TIMESTAMP_SYNCHRONIZATION_MODE_MAX_VAL = INT32_MAX
};

enum ADQTimestampSynchronizationArm
{
  ADQ_TIMESTAMP_SYNCHRONIZATION_ARM_IMMEDIATE = 0,
  ADQ_TIMESTAMP_SYNCHRONIZATION_ARM_ACQUISITION = 1,
  ADQ_TIMESTAMP_SYNCHRONIZATION_ARM_MAX_VAL = INT32_MAX
};

struct ADQTimestampSynchronizationParameters
{
  enum ADQParameterId id;
  enum ADQEventSource source;
  enum ADQEdge edge;
  enum ADQTimestampSynchronizationMode mode;
  enum ADQTimestampSynchronizationArm arm;
  int32_t reserved;
  uint64_t seed;
  uint64_t magic;
};

enum ADQSystemManagerType
{
  ADQ_SYSTEM_MANAGER_NOT_PRESENT = 0,
  ADQ_SYSTEM_MANAGER_CONMAN_I2C = 1,
  ADQ_SYSTEM_MANAGER_CONMAN_SPI = 2,
  ADQ_SYSTEM_MANAGER_SYSMAN_GEN4 = 3,
  ADQ_SYSTEM_MANAGER_MAX_VAL = INT32_MAX
};

struct ADQEventSourceParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQEventSourcePeriodicParameters periodic;
  struct ADQEventSourceLevelParameters level;
  struct ADQEventSourcePortParameters port[ADQ_MAX_NOF_PORTS];
  uint64_t magic;
};

struct ADQSignalProcessingParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQDigitalGainAndOffsetParameters gain_offset;
  struct ADQSampleSkipParameters sample_skip;
  struct ADQDbsParameters dbs;
  struct ADQFirFilterParameters fir_filter;
  uint64_t magic;
};

struct ADQFunctionParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQPatternGeneratorParameters pattern_generator[ADQ_MAX_NOF_PATTERN_GENERATORS];
  struct ADQPulseGeneratorParameters pulse_generator[ADQ_MAX_NOF_PULSE_GENERATORS];
  struct ADQTimestampSynchronizationParameters timestamp_synchronization;
  uint64_t magic;
};

struct ADQParameters
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQConstantParameters constant;
  struct ADQAnalogFrontendParameters afe;
  struct ADQPortParameters port[ADQ_MAX_NOF_PORTS];
  struct ADQEventSourceParameters event_source;
  struct ADQFunctionParameters function;
  struct ADQTestPatternParameters test_pattern;
  struct ADQSignalProcessingParameters signal_processing;
  struct ADQDataAcquisitionParameters acquisition;
  struct ADQDataTransferParameters transfer;
  struct ADQDataReadoutParameters readout;
  uint64_t magic;
};


#ifdef ADQAPI_INTERNAL
struct ADQDigitalGainAndOffsetParametersInternalCore
{
  int64_t gain;
  int64_t offset;
  int32_t override_all;
  int32_t override_input_range;
};

struct ADQDigitalGainAndOffsetParametersInternalChannel
{
  struct ADQDigitalGainAndOffsetParametersInternalCore core[ADQ_MAX_NOF_ADC_CORES];
};

struct ADQDigitalGainAndOffsetParametersInternal
{
  enum ADQParameterId id;
  int32_t reserved;
  struct ADQDigitalGainAndOffsetParametersInternalChannel channel[ADQ_MAX_NOF_CHANNELS];
  uint64_t magic;
};
#endif

#endif // MOCKADQAPIGEN3CONSTANTS_H
