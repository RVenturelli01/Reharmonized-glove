import threading
import sounddevice as sd
import numpy as np
import serial
import soundfile as sf
import time


# Load the audio file
file_path = "Music mixer/Media/SI.wav"
SI, _ = sf.read(file_path)
SI = (SI[:,0] + SI[:,1])/2

file_path = "Music mixer/Media/SOL.wav"
SOL, samplerate = sf.read(file_path)
SOL = (SOL[:,0] + SOL[:,1])/2
# samplerate = 44100

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



# Function to read from the serial port
def read_from_serial(ser):
    while True:
        try:
            if ser.in_waiting > 0:

                # read data from serial until "\n"
                data = ser.readline().decode('utf-8')
                print(data)

                # update parameters
                if data.startswith("SI"):
                    wave[:SI.size] = SI
                if data.startswith("SOL"):
                    wave[:SOL.size] = SOL

        except serial.SerialException as e:
            print(f"Error reading from serial port: {e}")
            break


def stream_audio():
    with sd.OutputStream(samplerate=samplerate, channels=1, callback=callback):
        while True:
            time.sleep(1)



# Main function
def main():
    
    # Open serial port
    ser = serial.Serial('COM9', 115200, timeout=1)  

    if not ser.is_open:
        ser.open()

    # Create and start the serial reading thread
    serial_thread = threading.Thread(target=read_from_serial, args=(ser,))
    serial_thread.daemon = True  # This ensures the thread will exit when the main program does
    serial_thread.start()
    # Create and start the serial reading thread
    stream_thread = threading.Thread(target=stream_audio, args=())
    stream_thread.daemon = True  # This ensures the thread will exit when the main program does
    stream_thread.start()

    try:
        while True:
            # Main thread can perform other tasks or simply wait
            time.sleep(10)            
            print("im in")
                
    except KeyboardInterrupt:
        print("Exiting program.")

    ser.close()

if __name__ == "__main__":
    main()
