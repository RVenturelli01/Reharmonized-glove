import socket
import time 

def main():
    server_ip = '0.0.0.0'
    server_port = 12345
    buffer_size = 1024
    time_inizio = 0

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((server_ip, server_port))
    server_socket.listen(1)

    print(f"Server listening on {server_ip}:{server_port}")

    while True:
        client_socket, client_address = server_socket.accept()
        print(f"Accepted connection from {client_address}")

        while True:
            data = client_socket.recv(buffer_size)
            if not data:
                break

            data = data.decode('utf-8')
            data = data[0:len(data)-1]
            print("data grezza: ",data)

            messages = [i for i in data.split('[')]
            for message in messages:
                message_float = [float(i) for i in message.split(';')]
                print("data pulita: ",message_float)
            #print(f"Received data: {data.decode('utf-8')}")
            

        client_socket.close()
        print(f"Connection with {client_address} closed")

if __name__ == "__main__":
    main()