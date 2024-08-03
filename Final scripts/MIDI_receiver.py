import rtmidi
import socket
import select
import threading
import time

def handle_client(client_socket, buffer_size, midiout):
    while True:
        try:
            data = client_socket.recv(buffer_size)
            if data:
                print(f"Received data: {data.decode('utf-8')}")
                data = data.decode('utf-8').strip()
                print("Data grezza: ", data)

                midi_messages = [i for i in data.split('[') if i]
                for midi_message_string in midi_messages:
                    midi_message_array = [float(i) for i in midi_message_string.split(';') if i]
                    print("Data pulita: ", midi_message_array)
                    if len(midi_message_array) == 3:  # Assicurarsi che ci siano esattamente 3 elementi
                        midiout.send_message([int(midi_message_array[0]), int(midi_message_array[1]), int(midi_message_array[2])])
            else:
                break
        except socket.error as e:
            if e.errno == 10035:  # WinError 10035 - WSAEWOULDBLOCK
                continue
            else:
                print(f"Error receiving data: {e}")
                break
        except Exception as e:
            print(f"Error receiving data: {e}")
            break
    client_socket.close()

def server_thread(server_ip, server_port, buffer_size, midiout):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((server_ip, server_port))
    server_socket.listen(5)
    server_socket.setblocking(False)
    print(f"Server listening on {server_ip}:{server_port}")

    while True:
        try:
            read_sockets, _, exception_sockets = select.select([server_socket], [], [server_socket])
            for notified_socket in read_sockets:
                client_socket, client_address = server_socket.accept()
                client_socket.setblocking(False)
                print(f"Accepted connection from {client_address}")
                threading.Thread(target=handle_client, args=(client_socket, buffer_size, midiout)).start()

            for notified_socket in exception_sockets:
                print(f"Exception on {notified_socket}")
                notified_socket.close()

        except socket.error as e:
            print(f"Socket error: {e}")
            time.sleep(5)

        except Exception as e:
            print(f"Unexpected error: {e}")
            time.sleep(5)

def main():
    server_ip = '0.0.0.0'
    port1 = 12345
    port2 = 12346
    port3 = 12347
    port4 = 12348
    buffer_size = 1024

    midiout = rtmidi.MidiOut()
    available_ports = midiout.get_ports()

    if available_ports:
        midiout.open_port(0)
    else:
        midiout.open_virtual_port("My virtual output")

    # set marimba on the channel 0
    program_change = [0xC0, 12]
    midiout.send_message(program_change)
    # set piano on the channel 1
    program_change = [0xC0 + 1, 0]
    midiout.send_message(program_change)

    thread1 = threading.Thread(target=server_thread, args=(server_ip, port1, buffer_size, midiout))
    thread2 = threading.Thread(target=server_thread, args=(server_ip, port2, buffer_size, midiout))
    thread3 = threading.Thread(target=server_thread, args=(server_ip, port3, buffer_size, midiout))
    thread4 = threading.Thread(target=server_thread, args=(server_ip, port4, buffer_size, midiout))

    thread1.start()
    thread2.start()
    thread3.start()
    thread4.start()

    thread1.join()
    thread2.join()
    thread3.join()
    thread4.join()

if __name__ == "__main__":
    main()



