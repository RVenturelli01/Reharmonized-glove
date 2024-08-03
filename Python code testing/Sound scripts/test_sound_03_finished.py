import sounddevice as sd
import numpy as np
import serial

# Open serial port
s = serial.Serial('COM9', 115200, timeout=1)  # Adjust port and baud rate as needed

# Sound settings
sample_rate = 44100  # Sampling frequency
sample_time = 1/sample_rate

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


sound1 = Sound(0, 1)
sound2 = Sound(0, 1)

# Callback function to write the data to the stream
def callback(outdata, frames, time, status):
    if status:
        print(status)

    wave = sound1.get_wave(frames, sample_time)
    wave += sound2.get_wave(frames, sample_time)

    outdata[:] = wave.reshape(-1, 1)


# Open an OutputStream
with sd.OutputStream(samplerate=sample_rate, channels=1, callback=callback):
    print("Playing sound for {} seconds...".format(10))

    while True:
        # read data from serial until "\n"
        data = s.readline().decode('utf-8')
        print(data)

        # update parameters
        if data.startswith("F1"):
            sound1.set_freq(int(data[2:]))
        if data.startswith("A1"):
            sound1.set_amplitude(int(data[2:]))
        if data.startswith("F2"):
            sound2.set_freq(int(data[2:]))
        if data.startswith("A2"):
            sound2.set_amplitude(int(data[2:]))


s.close()

print("Playback finished.", i)

