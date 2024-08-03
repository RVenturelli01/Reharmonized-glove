import requests
import time

#url = "http://192.168.1.156/imu"  # Replace <your_arduino_ip> with the IP address of your Arduino
url = "http://196.168.183.69/imu"  

while True:
    response = requests.get(url)
    if response.status_code == 200:
        data = response.text
        print(data)
    else:
        print("Failed to get data from Arduino")
    #time.sleep(1)  # Fetch data every second
