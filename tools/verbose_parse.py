import ctypes as ct
import numpy as np
import sys
import os
import argparse
from matplotlib import pyplot as plt
PACK_STRUCTURES = 0


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
    parser.add_argument('-o', '--output_ascii', default=None, help='path to destination ascii file')
    parser.add_argument('-l', '--limit_records', default=0, help='number of records to extract, 0 for all')
    parser.add_argument('-n', '--no_pack', action='store_true', help='do not pack strucutres')
    parser.add_argument('-p', '--plot', action='store_true', help='plot using matplotlib')
    parser.add_argument('-t', '--tag_size', default=128, help='size of the acquisition tag at the start of a minified channel configuration, default=128')
    parser = parser.parse_args()
    print("Using file tag ", parser.tag_size)
    # the structures are subject to change and on some machines might require
    # inheriting from ct.LittleEndianStructure or ct.BigEndianStructure
    # and setting proper pack argument, blackbox seems to work with LittleEndianStructure and pack==False
    class MinifiedRecordHeader(ct.Structure):
        _pack_ = not parser.no_pack
        _fields_ = [
            ("recordLength", ct.c_uint32),
            ("recordNumber", ct.c_uint32),
            ("timestamp", ct.c_uint64)
        ]

    class MinifiedChannelConfiguration(ct.Structure):
        _pack_ = not parser.no_pack
        _fields_ = [
            ("fileTag", ct.c_char * parser.tag_size),
            ("isStreamContinuous", ct.c_uint8),
            ("userLogicBypass", ct.c_uint8),
            ("channelMask", ct.c_uint8),
            ("channel", ct.c_uint8),
            ("sampleSkip", ct.c_uint16),
            ("inputRangeFloat", ct.c_float),
            ("triggerEdge", ct.c_uint8),
            ("triggerMode", ct.c_uint8),
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
    print("MinifiedConfig sizeof=", ct.sizeof(MinifiedChannelConfiguration))
    if(parser.output_ascii):
        if os.path.exists(parser.output_ascii):
            os.remove(parser.output_ascii)
        output_file = open(parser.output_ascii, "a")
    with open(parser.filepath, "rb") as f:
        bconf = f.read(ct.sizeof(MinifiedChannelConfiguration))
        conf = MinifiedChannelConfiguration.from_buffer_copy(bconf)
        print(conf.fileTag, conf.recordLength, conf.inputRangeFloat)
        eof = False
        record_length = conf.recordLength
        record_count = 0
        while not eof:
            if(not conf.isStreamContinuous):
                bhead = f.read(ct.sizeof(MinifiedRecordHeader))
                if(len(bhead) == 0):
                    eof = True
                    break
                head = MinifiedRecordHeader.from_buffer_copy(bhead)
                # do anything else you need with the header
                print('#{} {}ps'.format(head.recordNumber, head.timestamp*125))
                record_length = head.recordLength
                record_count += 1
                
            bsamples = f.read(record_length*ct.sizeof(ct.c_int16))
            if(len(bsamples) == 0):
                eof = True
                break
            print(len(bsamples))
            samples = np.frombuffer(bsamples, dtype=np.int16, count=-1)
                    
            if(parser.output_ascii):
                np.savetxt(output_file, samples.astype(int), fmt='%d')
            if(parser.limit_records and record_count >= int(parser.limit_records)):
                break
            if(parser.plot):
                plt.plot(samples)
                plt.show()
    
    if(parser.output_ascii):
        output_file.close()
