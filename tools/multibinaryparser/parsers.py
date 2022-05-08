from abc import ABC, abstractclassmethod, abstractmethod, abstractproperty, abstractstaticmethod
import ctypes as ct
from multibinaryparser.record_sinks import RecordSink
from multibinaryparser.structures import get_structures
import numpy as np

def parse_version(fi):
    version = ct.c_uint64.from_buffer_copy(fi.read(ct.sizeof(ct.c_uint64)))
    fi.seek(0)
    return int(version)


class DataParser(ABC):
    def __init__(self, record_listeners:list):
        self.listeners = record_listeners

    @abstractmethod
    def parse(self, files:list, *, record_limit=0):
        pass


class RecordParser(DataParser):
    def __init__(self, version:int, record_listeners:list[RecordSink]):
        super().__init__(record_listeners)
        self.version = version

    def on_record(self, header, samples):
        for l in self.listeners:
            l.on_record(header, samples)

    def on_finish(self):
        for l in self.listeners:
            l.on_finish()
    
    def on_config(self, configs:list[ct.Structure]):
        for l in self.listeners:
            l.on_config(configs)

    def parse(self, files:list, *, record_limit=0):
        configstruct, headerstruct = get_structures(self.version)
        configs = []
        for f in files:
            bconf = f.read(ct.sizeof(configstruct))
            conf = configstruct.from_buffer_copy(bconf)
            configs.append(conf)
            print(f"{conf.version=}, {conf.fileTag=}, {conf.recordLength=}, {conf.inputRangeFloat=}")
        self.on_config(configs)
        eof = False
        record_length = conf.recordLength
        record_count = 0
        last_timestamp = 0
        while not eof:
            headers, samples_list = [], []
            for f in files:
                bhead = f.read(ct.sizeof(headerstruct))
                if(len(bhead) == 0):
                    eof = True
                    break
                head = headerstruct.from_buffer_copy(bhead)
                record_length = head.recordLength
                record_count += 1
                bsamples = f.read(record_length*ct.sizeof(ct.c_int16))
                if(len(bsamples) == 0):
                    eof = True
                    break
                samples = np.frombuffer(bsamples, dtype=np.int16, count=-1)
                headers.append(head)
                samples_list.append(samples)
            if not eof:
                self.on_record(headers, samples_list)            
        self.on_finish()
        print("The file contained {} records".format(record_count))
        
