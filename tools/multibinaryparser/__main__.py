import argparse
from contextlib import ExitStack
from matplotlib import pyplot as plt

from multibinaryparser.parsers import RecordParser
from multibinaryparser.record_sinks import PlotSink, PrintSink, TrigPortDebugger
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
    args.add_argument('channels', nargs="+", metavar='path', type=str, help='path to the data files')
    args.add_argument('-l', '--limit_records', default=0, help='number of records to extract, 0 for all')
    args.add_argument('-p', '--plot', action='store_true', help='plot using matplotlib')
    args.add_argument('-d', '--debug_trig', type=int, default=-1, help='plots and prints out records which have bool(generalPurpose0)==False; set to index of records file containing the GPIO output in headers')
    args.add_argument('--ap_cutoff', type=float, default=0, help='afterpulse detection window in ms, use with -d')
    args = args.parse_args()
    recordsinks = []

    if args.plot:
        recordsinks.append(PrintSink())
        recordsinks.append(PlotSink(len(args.channels)))


    if args.debug_trig != -1:
        if 0 > args.debug_trig or args.debug_trig > len(args.channels):
            print(f"debug_trig set to a file not present in channels list. Given {len(args.channels)} record files, but debug_trig was set to {args.debug_trig}")
            exit()
        recordsinks.append(TrigPortDebugger(args.debug_trig, args.plot, cutoff_ms=args.ap_cutoff)) 
    
    
    with ExitStack() as stack:
        files = [
            stack.enter_context(open(fname, 'rb'))
            for fname in args.channels
        ]
        versions = [f.read(ct.sizeof(ct.c_uint64)) for f in files]
        versions = [ct.c_uint64.from_buffer_copy(version).value for version in versions]
        if not all(v == versions[0] for v in versions):
            print("Version mismatch in data files. Cannot proceed.", versions)
            exit()
        for f in files: f.seek(0)
        parser = RecordParser(versions[0], recordsinks)
        parser.parse(files, record_limit=args.limit_records)



