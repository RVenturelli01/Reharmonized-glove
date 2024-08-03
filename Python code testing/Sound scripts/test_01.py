import numpy as np
import requests
import time
import soundfile as sf
import sounddevice as sd
import rtmidi


#url_imu = "http://192.168.1.156/imu" 
#url_flex = "http://192.168.1.156/flex"  
url_flex = "http://192.168.1.156/flex"  

soglia = 0.5
        

# Main function
def main():
    
    midiout = rtmidi.MidiOut()
    available_ports = midiout.get_ports()

    if available_ports:
        midiout.open_port(0)
    else:
        midiout.open_virtual_port("My virtual output")

    while True:
        
        try:
            response_flex = requests.get(url_flex)
        except requests.RequestException as e:
                print(f"Error during HTTP request: {e}")
                
        if response_flex.status_code == 200:
            data = response_flex.text
            print(data)

            imuData = [float(i) for i in data.split(';')]
            if(imuData[3]):
                midiout.send_message([imuData[3], imuData[4], imuData[5]])
        else:
            print("Failed to get data from Arduino")
        
        
        time.sleep(0.01)  # Fetch data every second


if __name__ == "__main__":
    main()
