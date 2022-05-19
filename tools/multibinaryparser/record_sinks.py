from typing import Any, Iterable, Optional
import numpy as np
import os
import matplotlib.pyplot as plt
from abc import ABC, abstractmethod
import ctypes as ct
import pandas as pd

class RecordSink(ABC):

    @abstractmethod
    def on_record(self, headers:list[ct.Structure], samples:list[np.ndarray]):
        pass

    def on_finish(self):
        pass

    def on_config(self, configs:list[ct.Structure]):
        pass

class TrigPortDebugger(RecordSink):
    def __init__(self, trig_channel:int, plot:bool, cutoff_ms:float=None):
        self.trig_channel = trig_channel
        self.timestamp_diff = []
        self.last_timestamp = None
        self.last_high_timestamp = None
        self.plot = plot
        self.cutoff_ms = cutoff_ms
    
    def plot_samples(self, headers:list[ct.Structure], samples:list[np.ndarray]):
        for i, (header, samples) in enumerate(zip(headers, samples)):
            plt.subplot(len(headers), 1, i+1)
            plt.plot(samples)
        plt.show()

    def on_record(self, headers: list[ct.Structure], samples: list[np.ndarray]):
        if self.last_timestamp:
            self.timestamp_diff.append((headers[0].timestamp - self.last_timestamp) * 125 / 1e12)
        self.last_timestamp = headers[0].timestamp

        if not headers[self.trig_channel].generalPurpose0:
            if self.last_high_timestamp:
                ms_diff = (headers[self.trig_channel].timestamp - self.last_high_timestamp) * 125 / 1e9
                if self.cutoff_ms:
                    if self.cutoff_ms < ms_diff:
                        print(f"Low TRIG level detected outside afterpulse window, time from last high: {ms_diff:.4f} ms")
                        if self.plot: self.plot_samples()
                else:
                    print(f"Low TRIG level detected, time from last high: {ms_diff:.4f} ms")
                    if self.plot: self.plot_samples()
        else:
            self.last_high_timestamp = headers[self.trig_channel].timestamp;
    
    def on_finish(self):
        tsdiff = np.array(self.timestamp_diff)
        print(pd.DataFrame(tsdiff).describe())



class PrintSink(RecordSink):
    def __init__(self, timestamp_resolution=125):
        self.tres = timestamp_resolution

    def on_record(self, headers:list[ct.Structure], samples:list[np.ndarray]):
        print([f"#{header.recordNumber}" for header in headers])
        print(
            [header.generalPurpose0 for header in headers]
        )

class PlotSink(RecordSink):
    def on_record(self, headers:list[ct.Structure], samples:list[np.ndarray]):
        for i, (header, samples) in enumerate(zip(headers, samples)):
            plt.subplot(self.channel_count, 1, i+1)
            plt.plot(samples)
        plt.show()
    
