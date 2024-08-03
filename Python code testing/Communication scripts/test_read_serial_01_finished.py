'''

To check the serial port available digit the command "python -m serial.tools.list_ports" in the cmd

Tutorial pyserial:

x = s.read()            # read one byte
y = s.read(10)          # read up to ten bytes (timeout)
line = s.readline()     # read a '\n' terminated line
s.write(b'hello')       # la lettera b specifica che la stringa è di bytes
msg = "hello"           # stringa classica
s.write(msg.encode())   # msg deve essere convertita a una stringa di bytes

'''

import serial

# Open serial port
s = serial.Serial('COM9', 115200, timeout=1)  # Adjust port and baud rate as needed

# parameters
speed = 0
volume = 0

while True:
    # read data from serial until "\n"
    data = s.readline().decode('utf-8')
    print(data)

    # update parameters
    if data.startswith("S"):
        speed = int(data[1:])
    if data.startswith("V"):
        volume = int(data[1:])

    print(speed, volume)

s.close()


