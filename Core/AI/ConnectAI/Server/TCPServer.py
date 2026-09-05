import socket
from GameState import GameState

class TCPServer:
    def __init__(self, host="127.0.0.1", port=5000, backlog=1):
        self.host = host            # 서버의 IP 주소
        self.port = port            # 서버의 포트 번호
        self.backlog = backlog      # 동시에 대기 가능한 연결 요청 개수
        self.maxReceiveSize = 4096  # 텍스트(JSON) 메시지 수신 시 한 번에 읽어올 최대 바이트 수
        self.server = None          # 서버 소켓
        self.connectSocket = None   # 클라이언트와 통신할 소켓
        self.address = None         # 연결된 클라이언트 주소

    # 서버 소켓 생성 및 연결 대기 시작 메소드
    def Start(self):
        # IPv4(AF_INET), TCP(SOCK_STREAM) 소켓 생성
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # 지정한 IP와 포트에 서버 소켓 바인딩
        self.server.bind((self.host, self.port))

        # 클라이언트의 연결 요청을 받을 수 있도록 대기 상태로 전환
        self.server.listen(self.backlog)

        print(f"[Info] 클라이언트 연결 중... ({self.host}:{self.port})")

    # 클라이언트 연결 수락 메소드
    def Accept(self):
        self.connectSocket, self.address = self.server.accept()

        print(f"[Info] 연결된 클라이언트 주소 : {self.address}")

    # 정확히 size 바이트를 모아서 수신 (TCP는 스트림이라 한 번의 recv로 다 오지 않을 수 있음)
    def ReceiveDataFromClient(self, size):
        buffer = bytearray()

        while len(buffer) < size:
            try:
                # 남은 만큼만 추가로 수신
                chunk = self.connectSocket.recv(size - len(buffer))

            except ConnectionResetError:
                print("[Info] 클라이언트에 의해 연결이 종료되었습니다.")
                return None

            # 빈 데이터는 연결 종료를 의미
            if not chunk:
                return None

            # 들어온 데이터만큼 buffer에 추가 (클라이언트로부터 수신되지 못한 데이터는 다음에 recv에서 수신됨)
            buffer.extend(chunk)

        return bytes(buffer)

    # 클라이언트로부터 메시지 하나를 수신해 (종류, 내용) 튜플로 반환하는 메소드
    def ReceiveMessage(self):
        # 아직 연결된 클라이언트가 없는 경우
        if self.connectSocket is None:
            return None, None

        header = self.ReceiveDataFromClient(1)

        if header is None:
            return None, None

        # 첫 바이트가 GameState.MESSAGE_TYPE이면 고정 크기 게임 상태 바이너리로 보고 정확히 그 크기만큼 수신
        if header[0] == GameState.MESSAGE_TYPE:
            body = self.ReceiveDataFromClient(GameState.MESSAGE_SIZE - 1)

            # 빈 데이터는 연결 종료를 의미
            if body is None:
                return None, None

            return "game_state", header + body

        # 그 외에는 기존과 동일하게 텍스트(JSON) 메시지로 보고 나머지를 한 번에 수신
        try:
            rest = self.connectSocket.recv(self.maxReceiveSize)

        # 연결이 끊어졌거나 수신에 실패하면 (None, None)을 반환
        except ConnectionResetError:
            print("[ERROR] 클라이언트로부터 연결이 끊어졌습니다.")
            return None, None

        # bytes → 문자열로 변환하여 반환
        try:
            return "text", (header + rest).decode()

        except UnicodeDecodeError:
            print("[ERROR] 텍스트 메시지 디코딩에 실패했습니다.")
            return None, None

    # 클라이언트에게 문자열 전송 메소드
    def Send(self, message):
        # 연결된 클라이언트가 있을 경우
        if self.connectSocket is not None:
            self.connectSocket.send(message.encode())    # 문자열을 bytes로 변환하여 전송

    # 클라이언트 및 서버 소켓 종료 메소드
    def Close(self):
        # 클라이언트 연결 종료
        if self.connectSocket:
            self.connectSocket.close()

        # 서버 소켓 종료
        if self.server:
            self.server.close()