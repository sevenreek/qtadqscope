import argparse
from matplotlib import pyplot as plt

from parsers import RecordParser
from record_sinks import ASCIISink, AveragingSpectrumSink, PlotSink, PrintSink, TimeOrderVerifier
import ctypes as ct

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
    args = argparse.ArgumentParser()
    args.add_argument('filepath', metavar='path', type=str, help='path to the data file')
    args.add_argument('-v', '--verify', action='store_true', help='run the script in verify mode; checks record headers for mistakes')
    args.add_argument('-o', '--output_ascii', default=None, help='path to destination ascii file')
    args.add_argument('-l', '--limit_records', default=0, help='number of records to extract, 0 for all')
    args.add_argument('-i', '--spectrum', choices=['avg'], help='calculate spectrum using one of the available methods')
    args.add_argument('-p', '--plot', action='store_true', help='plot using matplotlib')
    args.add_argument('-s', '--silent', action='store_true', help='silent mode')
    args = args.parse_args()
    recordsinks = []

    if args.verify: 
        recordsinks.append(TimeOrderVerifier())
    if args.output_ascii:
        recordsinks.append(ASCIISink(args.output_ascii))
    if not args.silent:
        recordsinks.append(PrintSink(['generalPurpose0', 'generalPurpose1']))
    if args.plot:
        recordsinks.append(PlotSink())
    
    if args.spectrum == "avg":
        recordsinks.append(AveragingSpectrumSink(512))
    

    with open(args.filepath, "rb") as f:
        version = f.read(ct.sizeof(ct.c_uint64))
        version = ct.c_uint64.from_buffer_copy(version).value
        f.seek(0)
        parser = RecordParser(version, recordsinks)
        parser.parse(f, record_limit=args.limit_records)



