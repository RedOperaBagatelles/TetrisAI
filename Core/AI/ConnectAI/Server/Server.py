import json
from queue import Queue
from threading import Thread
from TCPServer import TCPServer
from GameState import GameState, PieceType
from PossiblePlacements import PossiblePlacements
from GameVisualizer import GameVisualizer


def ServerLoop(dataQueue: Queue):
    """서버 메인 루프 (별도 스레드에서 실행)"""

    while True:
        try:
            server = TCPServer()
            server.Start()
            server.Accept()

            while True:
                messageType, message = server.ReceiveMessage()

                if messageType is None:
                    break

                # 현재 받은 MessageType이 게임 상태인 경우
                if messageType == "game_state":
                    state = GameState.FromBytes(message)
                    dataQueue.put((state, message))
                    continue

                # 현재 받은 MessageType이 가능한 배치 목록인 경우 콘솔에 출력
                if messageType == "possible_placements":
                    placements = PossiblePlacements.FromBytes(message)

                    print(f"[Placements] 현재 조각 ({placements.x}, {placements.y}) rotation={placements.rotation} | 가능한 배치 {len(placements.placements)}개")

                    for placement in placements.placements:
                        actions = ", ".join(action.name for action in placement.actions)
                        print(f"  - ({placement.x}, {placement.y}) rotation={placement.rotation} : [{actions}]")

                    continue

                try:
                    request = json.loads(message)

                    # 처음 클라이언트와 연결시 보드 정보를 가져오기 위한 요청 처리
                    if (request.get("Type") == "ConnectionCheck" and request.get("Status") == "Request"):
                        boardWidth = int(request.get("BoardWidth", GameState.BOARD_WIDTH))
                        boardHeight = int(request.get("BoardHeight", GameState.BOARD_HEIGHT))
                        queueSize = int(request.get("QueueSize", GameState.NEXT_PIECE_QUEUE_SIZE))
                        pieceTypeCount = request.get("PieceTypeCount")
                        messageSize = request.get("MessageSize")

                        # 클라이언트로 부터 받은 보드 크기, 큐 크기, 메시지 크기 정보로 GameState 클래스 설정 갱신
                        GameState.Configure(boardWidth, boardHeight, queueSize, int(pieceTypeCount) if pieceTypeCount is not None else None,)

                        # 서버 내부적으로 설정한 메시지 크기와 클라이언트로부터 가져온 메시지 크기가 다르면 경고 로그 출력
                        if messageSize is not None and int(messageSize) != GameState.MESSAGE_SIZE:
                            print(f"[WARN] 클라이언트로부터 가져온 메시지 크기 불일치! 클라이언트={messageSize}, 서버={GameState.MESSAGE_SIZE}")

                        # 클라이언트에게 connection_check 응답 전송
                        response = {"Type": "ConnectionCheck", "Status": "Response"}
                        server.Send(json.dumps(response, separators=(',', ':')))

                    else:
                        server.Send(json.dumps({"Error": "InvalidRequest"}))

                except json.JSONDecodeError:
                    server.Send('{"Error":"InvalidJson"}')

        except Exception as e:
            print(f"[ERROR] Server exception: {e}")

        finally:
            try:
                server.Close()
            except Exception:
                pass


dataQueue = Queue()

serverThread = Thread(target=ServerLoop, args=(dataQueue,), daemon=True)
serverThread.start()

visualizer = GameVisualizer(dataQueue)
visualizer.Run()