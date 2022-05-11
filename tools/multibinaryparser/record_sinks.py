from typing import Any, Iterable, Optional
import numpy as np
import os
import matplotlib.pyplot as plt
from abc import ABC, abstractmethod
import ctypes as ct


class RecordSink(ABC):
    def __init__(self, channel_count:int):
        self.channel_count = channel_count

    @abstractmethod
    def on_record(self, headers:list[ct.Structure], samples:list[np.ndarray]):
        pass

    def on_finish(self):
        pass

    def on_config(self, configs:list[ct.Structure]):
        pass

class TrigPortDebugger(RecordSink):
    def __init__(self, trig_channel:int):
        self.trig_channel = trig_channel
        self.first_timestamp = None
        self.count = 0

    def on_record(self, headers: list[ct.Structure], samples: list[np.ndarray]):
        if self.first_timestamp is None:
            self.first_timestamp = headers[0].timestamp * 125
        self.last_timestamp = headers[0].timestamp * 125
        self.count += 1
        if not headers[self.trig_channel].generalPurpose0:
            print("Detected low TRIG in records:")
            print([f"#{header.recordNumber}" for header in headers])
            print(
                [header.generalPurpose0 for header in headers]
            )
            for i, (header, samples) in enumerate(zip(headers, samples)):
                plt.subplot(self.channel_count, 1, i+1)
                plt.plot(samples)
            plt.show()
    
    def on_finish(self):
        elapsed = self.last_timestamp - self.first_timestamp
        elapsed_s = elapsed / 10**12
        print(f"Elapsed {elapsed} ps. Gathered {self.count} records. Record rate {self.count/elapsed_s} 1/s.")

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
    
