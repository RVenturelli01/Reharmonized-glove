import sounddevice as sd
import numpy as np
import serial
import soundfile as sf


# Load the audio file
file_path = 'Media/SI.wav'
SI, _ = sf.read(file_path)
SI = (SI[:,0] + SI[:,1])/2

file_path = 'Media/SOL.wav'
SOL, samplerate = sf.read(file_path)
SOL = (SOL[:,0] + SOL[:,1])/2

# samplerate = 44100


# Open serial port
s = serial.Serial('COM9', 115200, timeout=1)  # Adjust port and baud rate as needed

wave_duration = 1    #seconds
wave = np.zeros(int(wave_duration*samplerate))

# Callback function to write the data to the stream
def callback(outdata, frames, time, status):
    global wave

    if status:
        print(status)

    outdata[:] = wave[:frames].reshape(-1, 1)

    wave_ = wave
    wave = np.zeros(int(wave_duration*samplerate))
    wave[:-frames] = wave_[frames:]


# Open an OutputStream
with sd.OutputStream(samplerate=samplerate, channels=1, callback=callback):
    print("Playing sound for {} seconds...".format(10))

    while True:
        # read data from serial until "\n"
        data = s.readline().decode('utf-8')
        print(data)

        # update parameters
        if data.startswith("SI"):
            wave[:SI.size] = SI
        if data.startswith("SOL"):
            wave[:SOL.size] = SOL

s.close()

