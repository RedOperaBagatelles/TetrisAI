import json
from TCPServer import TCPServer
from GameState import GameState, PieceType


def render_board(state: GameState) -> str:
    """수신한 GameState의 board를 콘솔에 출력하기 좋은 문자열 그리드로 변환
    빈 칸은 '.', 채워진 칸은 해당 PieceType의 앞 글자로 표시"""

    lines = []

    for row in state.board:
        chars = []

        for cell in row:
            if cell == PieceType.NONE:
                chars.append(".")
            else:
                try:
                    chars.append(PieceType(cell).name[0])
                except ValueError:
                    chars.append("?")

        lines.append("".join(chars))

    return "\n".join(lines)

# TCP 서버 객체 생성
server = TCPServer()

# 서버 소켓 생성 및 대기 상태로 전환
server.start()

# 클라이언트(C++)의 접속을 기다림 (connect()가 호출될 때까지 여기서 대기)
server.accept()

# 클라이언트와 연결된 동안 계속 실행
while True:
    # 클라이언트로부터 메시지 하나를 수신 (kind : "text" 또는 "game_state")
    kind, message = server.recv_message()

    # 연결이 종료되었으면 반복문 종료
    if kind is None:
        break

    # AIOutput::ToByte()가 보낸 게임 상태 바이너리 메시지
    if kind == "game_state":
        state = GameState.from_bytes(message)

        print(f"Receive game_state : current={state.current_piece}, hold={state.hold_piece.name}, next={[p.name for p in state.next_pieces]}")
        print(render_board(state))
        print("-" * state.BOARD_WIDTH)
        continue

    print("Receive :", message)

    try:
        # 수신한 문자열을 JSON 객체로 변환
        request = json.loads(message)

        # 연결 확인 패킷인지 검사
        if (request.get("type") == "connection_check" and request.get("status") == "request"):
            board_width = int(request.get("board_width", GameState.BOARD_WIDTH))
            board_width = int(request.get("board_width", GameState.BOARD_WIDTH))
            board_height = int(request.get("board_height", GameState.BOARD_HEIGHT))
            queue_size = int(request.get("queue_size", GameState.QUEUE_SIZE))
            piece_type_count = request.get("piece_type_count")
            client_message_size = request.get("message_size")

            GameState.configure(board_width, board_height,queue_size, int(piece_type_count) if piece_type_count is not None else None,)

            if client_message_size is not None and int(client_message_size) != GameState.MESSAGE_SIZE:
                print(f"[WARN] message_size mismatch! client={client_message_size}, server={GameState.MESSAGE_SIZE}")

            print(f"Client config : board={board_width}x{board_height}, queue_size={queue_size}, "
                  f"piece_type_count={piece_type_count}, message_size={GameState.MESSAGE_SIZE}")

            response = { "type": "connection_check", "status": "response" }
            server.send(json.dumps(response, separators=(',', ':')))

        else:
            # 정의되지 않은 요청인 경우 에러 메시지 전송
            server.send(json.dumps({"error": "invalid_request"}))

    # JSON 형식이 잘못된 경우
    except json.JSONDecodeError:
        print("Invalid JSON format")

        # JSON 파싱 실패 응답 전송
        server.send('{"error":"invalid_json"}')

# 클라이언트 및 서버 소켓 종료
server.close()