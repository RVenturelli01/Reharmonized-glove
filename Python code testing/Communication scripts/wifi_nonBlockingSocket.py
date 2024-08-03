import numpy as np
import requests
import soundfile as sf
import sounddevice as sd
import rtmidi
import socket
import select
import time

def main():
    server_ip = '0.0.0.0'
    server_port = 12345
    buffer_size = 1024

    midiout = rtmidi.MidiOut()
    available_ports = midiout.get_ports()

    if available_ports:
        midiout.open_port(0)
    else:
        midiout.open_virtual_port("My virtual output")

    def create_server_socket():
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((server_ip, server_port))
        server_socket.listen(1)
        server_socket.setblocking(False)
        return server_socket

    server_socket = create_server_socket()
    print(f"Server listening on {server_ip}:{server_port}")

    sockets_list = [server_socket]
    client_socket = None

    while True:
        try:
            read_sockets, _, exception_sockets = select.select(sockets_list, [], sockets_list)

            for notified_socket in read_sockets:
                if notified_socket == server_socket:
                    client_socket, client_address = server_socket.accept()
                    client_socket.setblocking(False)
                    sockets_list.append(client_socket)
                    print(f"Accepted connection from {client_address}")
                else:
                    data = notified_socket.recv(buffer_size)
                    if data:
                        print(f"Received data: {data.decode('utf-8')}")
                        data = data.decode('utf-8')
                        data = data[0:len(data)-1]
                        print("data grezza: ",data)

                        messages = [i for i in data.split('[')]
                        for message in messages:
                            message_float = [float(i) for i in message.split(';')]
                            print("data pulita: ", message_float)
                            midiout.send_message([message_float[0], message_float[1], message_float[2]])

                    else:
                        print("Connection closed by the client")
                        sockets_list.remove(notified_socket)
                        client_socket.close()
                        client_socket = None

            for notified_socket in exception_sockets:
                sockets_list.remove(notified_socket)
                if client_socket == notified_socket:
                    client_socket.close()
                    client_socket = None

        except socket.error as e:
            print(f"Socket error: {e}")
            if client_socket:
                client_socket.close()
                client_socket = None
            time.sleep(5)  # Attendi prima di tentare di nuovo

        except Exception as e:
            print(f"Unexpected error: {e}")
            if client_socket:
                client_socket.close()
                client_socket = None
            time.sleep(5)  # Attendi prima di tentare di nuovo

        if client_socket is None:
            try:
                print("Attempting to reconnect...")
                client_socket = create_server_socket()
                sockets_list = [client_socket]
                print("Reconnected successfully")
            except Exception as e:
                print(f"Reconnection failed: {e}")
                time.sleep(5)  # Attendi prima di riprovare

if __name__ == "__main__":
    main()