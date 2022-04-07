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
    parser.add_argument('-v', '--verify', action='store_true', help='run the script in verify mode; checks record headers for mistakes')
    parser.add_argument('-o', '--output_ascii', default=None, help='path to destination ascii file')
    parser.add_argument('-l', '--limit_records', default=0, help='number of records to extract, 0 for all')
    parser.add_argument('-n', '--no_pack', action='store_true', help='do not pack strucutres')
    parser.add_argument('-p', '--plot', action='store_true', help='plot using matplotlib')
    parser.add_argument('-s', '--silent', action='store_true', help='silent mode')
    parser.add_argument('-t', '--tag_size', default=128, help='size of the acquisition tag at the start of a minified channel configuration, default=128')
    parser = parser.parse_args()
    # the structures are subject to change and on some machines might require
    # inheriting from ct.LittleEndianStructure or ct.BigEndianStructure
    # and setting proper pack argument, blackbox seems to work with LittleEndianStructure and pack==False
    
    if(parser.output_ascii):
        if os.path.exists(parser.output_ascii):
            os.remove(parser.output_ascii)
        output_file = open(parser.output_ascii, "a")
    with open(parser.filepath, "rb") as f:
        
    
    if(parser.output_ascii):
        output_file.close()


