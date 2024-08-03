import numpy as np
import time
import soundfile as sf
import sounddevice as sd
import rtmidi

import serial

# Open serial port
s = serial.Serial('COM11', 115200, timeout=1)  # Adjust port and baud rate as needed

# Main function
def main():
    
    midiout = rtmidi.MidiOut()
    available_ports = midiout.get_ports()

    if available_ports:
        midiout.open_port(0)
    else:
        midiout.open_virtual_port("My virtual output")

    while True:
        data = s.readline().decode('utf-8')
        if(data):
            imuData = [float(i) for i in data.split(';')]
            print(data)
            try:
                midiout.send_message([imuData[0], imuData[1], imuData[2]])
            except:
                print("")


if __name__ == "__main__":
    main()
