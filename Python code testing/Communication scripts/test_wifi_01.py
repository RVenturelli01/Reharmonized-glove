import requests
import time

url = "http://192.168.1.156/flex"  # Replace <your_arduino_ip> with the IP address of your Arduino

while True:
    response = requests.get(url)
    if response.status_code == 200:
        print(response.text)
    else:
        print("Failed to get data from Arduino")
    time.sleep(0.1)  # Fetch data every second
