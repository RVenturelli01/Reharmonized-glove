import socket

server_ip = "192.168.183.2"  # IP del tuo PC
server_port = 12345        # Porta del server, deve essere la stessa usata nell'Arduino

def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((server_ip, server_port))
        s.listen()
        print(f"Server listening on {server_ip}:{server_port}")
        
        while True:
            conn, addr = s.accept()
            with conn:
                print(f"Connected by {addr}")
                while True:
                    data = conn.recv(1024)
                    if not data:
                        break
                    print(f"Received message: {data.decode()}")

if __name__ == "__main__":
    start_server()
