import ctypes as ct

# the structures are subject to change and on some machines might require
# inheriting from ct.LittleEndianStructure or ct.BigEndianStructure
# and setting proper pack argument, blackbox seems to work with LittleEndianStructure and pack==False
def get_structures(version:int):
    packed = False # do differnt things depending on version
    tag_size = 128

    if version >= 2:
        class MinifiedRecordHeader(ct.Structure):
            _pack_ = packed
            _fields_ = [
                ("recordLength", ct.c_uint32),
                ("recordNumber", ct.c_uint32),
                ("timestamp", ct.c_uint64),
                ("generalPurpose0", ct.c_uint16),
                ("generalPurpose1", ct.c_uint16)
            ]
    else:
        class MinifiedRecordHeader(ct.Structure):
            _pack_ = packed
            _fields_ = [
                ("recordLength", ct.c_uint32),
                ("recordNumber", ct.c_uint32),
                ("timestamp", ct.c_uint64)
            ]


    class MinifiedChannelConfiguration(ct.Structure):
        _pack_ = packed 
        _fields_ = [
            ("version", ct.c_uint64),
            ("fileTag", ct.c_char * tag_size),
            ("isStreamContinuous", ct.c_uint32),
            ("userLogicBypass", ct.c_uint32),
            ("channelMask", ct.c_uint32),
            ("channel", ct.c_uint32),
            ("sampleSkip", ct.c_uint32),
            ("inputRangeFloat", ct.c_double),
            ("triggerEdge", ct.c_uint32),
            ("triggerMode", ct.c_uint32),
            ("triggerLevelCode", ct.c_int32),
            ("triggerLevelReset", ct.c_int32),
            ("digitalOffset", ct.c_int32),
            ("analogOffset", ct.c_int32),
            ("digitalGain", ct.c_int32),
            ("dcBias", ct.c_int32),
            ("recordLength", ct.c_uint32),
            ("recordCount", ct.c_uint32),
            ("pretrigger", ct.c_uint32),
            ("triggerDelay", ct.c_uint32),
        ]
    return MinifiedChannelConfiguration, MinifiedRecordHeader