import ctypes as ct
import numpy as np
import sys
import argparse
from matplotlib import pyplot as plt
PACK_STRUCTURES = 0
"""
struct MinifiedRecordHeader {
    uint32_t recordLength;
    uint32_t recordNumber;
    uint64_t timestamp;
};

struct MinifiedChannelConfiguration {
    unsigned char userLogicBypass;
    unsigned short sampleSkip;
    float inputRangeFloat;
    unsigned char triggerEdge;
    unsigned char triggerMode;
    unsigned char isStreamContinuous;
    short triggerLevelCode;
    short triggerLevelReset;

    short digitalOffset;
    short analogOffset;
    short digitalGain;
    short dcBias;

    unsigned int recordLength;
    unsigned int recordCount;
    unsigned short pretrigger;
    unsigned short triggerDelay;

};
"""

# The binary strucutre of files created with the Verbose Buffered Binary file output mode are as follows:
# The file begins with a struct MinifiedChannelConfiguration written in binary

# If the acquisition was done in Continuous streaming mode i.e. MinifiedChannelConfiguration.isStreamContinuous == true 
# then the remaining data in the file is a binary buffer of the acquired samples. To read such file:
# 1) read sizeof(MinifiedChannelConfiguration) bytes and cast it to a MinifiedChannelConfiguration
# 2) read the rest of the file in chunks(or at once) into a short* or short[]

# If the acquisition was done in triggered streaming mode i.e: MinifiedChannelConfiguration.isStreamContinuous == false
# then the remaining data is alternates between a MinifiedRecordHeader and a short buffer of length specified in MinifiedRecordHeader.recordLength.
# This value in general is equal to MinifiedChannelConfiguration.recordLength. To read such a file:
# 1)  read sizeof(MinifiedChannelConfiguration) bytes and cast it to a MinifiedChannelConfiguration
# 2a) read sizeof(MinifiedRecordHeader) bytes and cast it to a MinifiedRecordHeader
# 2b) read sizeof(short)*recordLength into a short* or short[] 
# repeat steps 2ab until EOF is reached.


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('filepath', metavar='path', type=str, help='path to the data file')
    parser.add_argument('-p', '--pack', action='store_true', help='pack strucutres')
    parser = parser.parse_args()

    # the structures are subject to change and on some machines might require
    # inheriting from ct.LittleEndianStructure or ct.BigEndianStructure
    # and setting proper pack argument, blackbox seems to work with LittleEndianStructure and pack==False
    class MinifiedRecordHeader(ct.Structure):
        _pack_ = parser.pack
        _fields_ = [
            ("recordLength", ct.c_uint32),
            ("recordNumber", ct.c_uint32),
            ("timestamp", ct.c_uint64)
        ]

    class MinifiedChannelConfiguration(ct.Structure):
        _pack_ = parser.pack
        _fields_ = [
            ("userLogicBypass", ct.c_uint8),
            ("sampleSkip", ct.c_uint16),
            ("inputRangeFloat", ct.c_float),
            ("triggerEdge", ct.c_uint8),
            ("triggerMode", ct.c_uint8),
            ("isStreamContinuous", ct.c_uint8),
            ("triggerLevelCode", ct.c_int16),
            ("triggerLevelReset", ct.c_int16),
            ("digitalOffset", ct.c_int16),
            ("analogOffset", ct.c_int16),
            ("digitalGain", ct.c_int16),
            ("dcBias", ct.c_int16),
            ("recordLength", ct.c_uint32),
            ("recordCount", ct.c_uint32),
            ("pretrigger", ct.c_uint16),
            ("triggerDelay", ct.c_uint16),
        ]
        
    with open(parser.filepath, "rb") as f:
        bconf = f.read(ct.sizeof(MinifiedChannelConfiguration))
        conf = MinifiedChannelConfiguration.from_buffer_copy(bconf)
        eof = False
        record_length = conf.recordLength
        while not eof:
            if(not conf.isStreamContinuous):
                bhead = f.read(ct.sizeof(MinifiedRecordHeader))
                if(len(bhead) == 0):
                    eof = True
                    break
                head = MinifiedRecordHeader.from_buffer_copy(bhead)
                # do anything else you need with the header
                record_length = head.recordLength
                
            bsamples = f.read(record_length*ct.sizeof(ct.c_int16))
            if(len(bsamples) == 0):
                eof = True
                break
            print(len(bsamples))
            samples = np.frombuffer(bsamples, dtype=np.int16, count=-1)
            plt.plot(samples)
            plt.show()

