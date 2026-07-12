import socket

HOST = "127.0.0.1"
PORT = 5000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen(1)            # 서버를 열고 연결 요청을 기다림 (1개의 연결만 허용)

print("Waiting...")

# 클라이언트 접속 받기 (c++에서 connect가 호출될때까지 대기, conn : 클라이언트와 통신할 소켓, addr: 클라이언트 주소)
conn, addr = server.accept()

print("Connected : ", addr)

while True:
    # 클라이언트로부터 데이터 수신 (4096바이트까지 수신)
    data = conn.recv(4096)
    
    if not data:
        break
        
    message = data.decode()

    print("Recevie : ", message)
    
    conn.send("Hello Client!".encode())
    
conn.close()
server.close()