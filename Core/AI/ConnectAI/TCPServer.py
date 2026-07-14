import socket
import json

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
    try:
        data = conn.recv(4096)
        
    except ConnectionResetError:
        print("Connection closed by client.")
        break

    if not data:
        break
        
    message = data.decode()

    print("Receive : ", message)

    try:
        request = json.loads(message)

        if request.get("type") == "connection_check" and request.get("status") == "request":
            res = {"type": "connection_check", "status": "response"}
            
            # C++ 구현부의 Json::ToString 출력 형태와 일치시키기 위해 공백 없이 직렬화
            res_str = json.dumps(res, separators=(',', ':'))
            conn.send(res_str.encode())
        else:
            conn.send(json.dumps({"error": "invalid_request"}).encode())

    except json.JSONDecodeError:
        print("Invalid JSON format")
        conn.send(b'{"error": "invalid_json"}')

conn.close()
server.close()