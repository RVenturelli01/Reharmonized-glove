import sounddevice as sd
import numpy as np
import matplotlib.pyplot as plt

# Sound settings
sample_rate = 44100  # Sampling frequency
sample_time = 1/sample_rate

i = 0

class Sound:
    def __init__(self, amplitude, freq):
        self.freq = freq
        self.amplitude = amplitude
        self.starting_point = 0

    def set_freq(self, freq):
        self.freq = freq

    def set_amplitude(self, amplitude):
        self.amplitude = amplitude

    def get_wave(self, n_samples, sample_time):
        t_initial = self.starting_point / (self.freq * 2 * np.pi)
        t_final = t_initial + n_samples * sample_time
        t = np.linspace(t_initial, t_final, n_samples)
        wave = self.amplitude * np.sin(2 * np.pi * self.freq * t)
        self.starting_point = t_final * self.freq * 2 * np.pi
        return wave

i = 0
sound1 = Sound(1, 500)
sound2 = Sound(1, 100)


# Callback function to write the data to the stream
def callback(outdata, frames, time, status):
    global i, line

    if status:
        print(status)

    i += 1
    if i == 100:
        print("freq changed")
        sound1.set_freq(600)
        sound1.set_amplitude(0.5)
    if i == 200:
        print("freq changed")
        sound2.set_freq(100)
        sound2.set_amplitude(0.5)

    wave = sound1.get_wave(frames, sample_time)
    wave += sound2.get_wave(frames, sample_time)

    outdata[:] = wave.reshape(-1, 1)


# Open an OutputStream
with sd.OutputStream(samplerate=sample_rate, channels=1, callback=callback):
    print("Playing sound for {} seconds...".format(10))
    sd.sleep(int(10 * 1000))

print("Playback finished.", i)
