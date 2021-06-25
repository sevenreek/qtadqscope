import numpy as np

SAMPLE_COUNT = 4096
DATA_TYPE = np.int16
SIG_FREQ = 2*np.pi/SAMPLE_COUNT * 10
CHANNEL_CONFIGS = [
    (np.sin, 1000),
    (np.sin, 2000),
    (np.sin, 3000),
    (np.sin, 4000),
]

if __name__ == '__main__':
    time = range(4096)
    samples = []
    for i, ch in enumerate(CHANNEL_CONFIGS):
        samples.append(ch[0](np.array(time)*SIG_FREQ)*ch[1])
        samples[i] = samples[i].astype(DATA_TYPE)
        with open(f'ch{i+1}.mock', 'wb') as f:
            f.write(samples[i].tobytes())
    