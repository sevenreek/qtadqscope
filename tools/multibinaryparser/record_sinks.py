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

    def on_record(self, headers: list[ct.Structure], samples: list[np.ndarray]):
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
    
