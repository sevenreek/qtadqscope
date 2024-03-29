from typing import Any, Iterable, Optional
import numpy as np
import os
import matplotlib.pyplot as plt
from abc import ABC, abstractmethod
import ctypes as ct


class RecordSink(ABC):
    @abstractmethod
    def on_record(self, header:ct.Structure, samples:np.ndarray):
        pass

    def on_finish(self):
        pass

    def on_config(self, config:ct.Structure):
        pass

class TimeOrderVerifier(RecordSink):
    def __init__(self, timestamp_resolution=125):
        self.previous_header = None
        self.timestamp_multiplier = timestamp_resolution

    def on_record(self, header:ct.Structure, samples:np.ndarray):
        if not self.previous_header: return
        if self.previous_header.recordNumber >= header.recordNumber \
            or self.previous_header.timestamp >= header.timestamp:
            print(f"\t Timing mismatch between records:")
            print(f"\t\t> #{self.previous_header.recordNumber} ({self.previous_header.timestamp * self.timestamp_multiplier})")
            print(f"\t\t> #{header.recordNumber} ({header.timestamp * self.timestamp_multiplier})")
        
class PrintSink(RecordSink):
    def __init__(self, print_fields:Iterable[str], timestamp_resolution=125):
        self.tres = timestamp_resolution
        self.print_fields = print_fields

    def on_record(self, header:ct.Structure, samples:np.ndarray):
        print(f"#{header.recordNumber, header.timestamp * self.tres}")
        attributes = (f"{name}: {header.__getattribute__(name)}" for name in self.print_fields if name in vars(header))
        print("\t", ', '.join(attributes))


class PlotSink(RecordSink):

    def on_record(self, header:ct.Structure, samples:np.ndarray):
        plt.plot(samples)
        plt.show()
    

class ASCIISink(RecordSink):
    def __init__(self, filename:str):
        if os.path.exists(filename):
            os.remove(filename)
        self.output_file = open(filename, "a")

    def on_record(self, header:ct.Structure, samples:np.ndarray):
        np.savetext(self.output_file, samples.astype(int), fmt="%d")


class AveragingSpectrumSink(RecordSink):
    def __init__(self, bins:Optional[int]=None):
        self.averages = []
        self.bins = bins

    def on_record(self, header: ct.Structure, samples: np.ndarray):
        cast_samples = samples.astype(int)
        self.averages.append(np.average(samples - self.offset))

    def on_config(self, config: ct.Structure):
        self.offset = config.analogOffset + config.digitalOffset + config.dcBias

    def on_finish(self):
        plt.hist(self.averages, self.bins)
        plt.show()