import socket

from GameState import GameState


class TCPServer:
    def __init__(self, host="127.0.0.1", port=5000, backlog=1):
        # 서버의 IP 주소
        self.host = host

        # 서버의 포트 번호
        self.port = port

        # 동시에 대기 가능한 연결 요청 개수
        self.backlog = backlog

        # 텍스트(JSON) 메시지 수신 시 한 번에 읽어올 최대 바이트 수
        self.recv_size = 4096

        # 서버 소켓
        self.server = None

        # 클라이언트와 통신할 소켓
        self.conn = None

        # 연결된 클라이언트 주소
        self.addr = None

    def start(self):
        """서버 소켓 생성 및 연결 대기 시작"""

        # IPv4(AF_INET), TCP(SOCK_STREAM) 소켓 생성
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # 지정한 IP와 포트에 서버 소켓 바인딩
        self.server.bind((self.host, self.port))

        # 클라이언트의 연결 요청을 받을 수 있도록 대기 상태로 전환
        self.server.listen(self.backlog)

        print(f"Waiting... ({self.host}:{self.port})")

    def accept(self):
        """클라이언트 연결 수락"""

        # 클라이언트가 connect()를 호출할 때까지 대기
        # conn : 클라이언트와 데이터를 송수신할 소켓
        # addr : 클라이언트의 IP 주소와 포트 번호
        self.conn, self.addr = self.server.accept()

        print("Connected :", self.addr)

    def _recv_exact(self, size):
        """정확히 size 바이트를 모아서 수신 (TCP는 스트림이라 한 번의 recv로 다 오지 않을 수 있음)"""

        buffer = bytearray()

        while len(buffer) < size:
            try:
                # 남은 만큼만 추가로 수신
                chunk = self.conn.recv(size - len(buffer))

            except ConnectionResetError:
                print("Connection closed by client.")
                return None

            # 빈 데이터는 연결 종료를 의미
            if not chunk:
                return None

            buffer.extend(chunk)

        return bytes(buffer)

    def recv_message(self):
        """클라이언트로부터 메시지 하나를 수신해 (종류, 내용) 튜플로 반환

        - 첫 바이트가 GameState.MESSAGE_TYPE 이면 고정 크기 게임 상태 바이너리로 보고 정확히 그 크기만큼 수신
        - 그 외에는 기존과 동일하게 텍스트(JSON) 메시지로 보고 나머지를 한 번에 수신

        연결이 끊어졌거나 수신에 실패하면 (None, None)을 반환
        """

        # 아직 연결된 클라이언트가 없는 경우
        if self.conn is None:
            return None, None

        header = self._recv_exact(1)

        if header is None:
            return None, None

        if header[0] == GameState.MESSAGE_TYPE:
            body = self._recv_exact(GameState.MESSAGE_SIZE - 1)

            if body is None:
                return None, None

            return "game_state", header + body

        try:
            rest = self.conn.recv(self.recv_size)

        except ConnectionResetError:
            print("Connection closed by client.")
            return None, None

        try:
            # bytes → 문자열로 변환하여 반환
            return "text", (header + rest).decode()

        except UnicodeDecodeError:
            print("Failed to decode text message.")
            return None, None

    def send(self, message):
        """클라이언트에게 문자열 전송"""

        # 연결된 클라이언트가 있을 경우
        if self.conn is not None:

            # 문자열을 bytes로 변환하여 전송
            self.conn.send(message.encode())

    def close(self):
        """클라이언트 및 서버 소켓 종료"""

        # 클라이언트 연결 종료
        if self.conn:
            self.conn.close()

        # 서버 소켓 종료
        if self.server:
            self.server.close()