import numpy as np
import sounddevice as sd

# Parameters
sample_rate = 44100  # Samples per second
frequency = 440  # Frequency of the sine wave (Hz)
duration = 1.0  # Duration in seconds
amplitude = 1

# Generate time points in seconds
t = np.linspace(0, duration, int(sample_rate * duration))
# np.linspace(start, stop, number of samples)

# Generate the sine wave
wave = amplitude * np.sin(2 * np.pi * frequency * t)

# Play the sound
sd.play(wave, samplerate=sample_rate)
sd.wait()   # Wait until the sound has finished playing
