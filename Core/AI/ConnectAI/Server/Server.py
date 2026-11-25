import json
from queue import Queue
from threading import Thread
from TCPServer import TCPServer
from GameState import GameState, PieceType
from PossiblePlacements import PossiblePlacements
from GameVisualizer import GameVisualizer

# 초기 테스트 명령 대상: 짝수 번호 게임 전체 + 기존대로 1번 게임
# 1번만 제외하고 짝수 번호만 실행시키려면 False로 바꾸면 된다.
INITIAL_TEST_INCLUDE_GAME_ONE = True


def GetInitialTestTargets(gameCount: int):
    """초기 테스트 명령을 보낼 gameID 집합"""
    targets = set(range(2, gameCount + 1, 2)) if gameCount > 0 else set()

    if INITIAL_TEST_INCLUDE_GAME_ONE:
        targets.add(1)

    return targets


def ServerLoop(dataQueue: Queue):
    """서버 메인 루프 (별도 스레드에서 실행)"""

    while True:
        try:
            server = TCPServer()
            server.Start()
            server.Accept()

            # 테스트용: 연결 후 첫 조각에 대해 같은 배치 번호를 여러 게임에 한 번씩 실행
            server.selectionPending = True
            server.testPlacementIndex = None      # 1번 게임의 배치 목록에서 고른 인덱스
            server.testCommandSentGameIDs = set()  # 이미 테스트 명령을 보낸 게임 (중복 실행 방지)
            server.gameCount = 0

            def SendInitialTestCommand(gameID, index):
                # 대상 게임이 아니거나 이미 보냈으면 무시
                if gameID not in GetInitialTestTargets(server.gameCount):
                    return

                if gameID in server.testCommandSentGameIDs:
                    return

                # 해당 게임의 배치 목록이 도착해야 같은 인덱스를 실행할 수 있다.
                known = server.placementsByGameID.get((server.sessionID, gameID))

                if known is None or index >= len(known.placements):
                    return

                selected = known.placements[index]

                # 선택한 배치 정보를 시각화 큐로 전달 (보드 변화는 클라이언트가 배치 후 보내는 상태로 갱신)
                dataQueue.put({
                    "kind": "selection",
                    "placement": selected,
                    "index": index,
                    "gameID": gameID,
                    "sessionID": server.sessionID,
                })

                try:
                    server.SelectPathIndex(index, gameID)
                    server.testCommandSentGameIDs.add(gameID)
                except Exception as e:
                    print(f"[ERROR] SelectPathIndex(gameID={gameID}, index={index}) failed: {e}")
                    return

                # 모든 대상 게임에 한 번씩 명령을 보냈으면 테스트 종료
                if GetInitialTestTargets(server.gameCount).issubset(server.testCommandSentGameIDs):
                    server.selectionPending = False

            while True:
                messageType, message = server.ReceiveMessage()

                if messageType is None:
                    break

                # 현재 받은 MessageType이 게임 상태인 경우
                if messageType == "game_state":
                    state = GameState.FromBytes(message)
                    dataQueue.put({
                        "kind": "state",
                        "state": state,
                        "rawBytes": message,
                        "gameID": state.gameID,
                        "sessionID": server.sessionID,
                    })
                    continue

                # 현재 받은 MessageType이 가능한 배치 목록인 경우 콘솔에 출력
                if messageType == "possible_placements":
                    placements = PossiblePlacements.FromBytes(message)

                    print(f"[Placements][Game {placements.gameID}] 현재 조각 {placements.currentPiece.name} ({placements.x}, {placements.y}) rotation={placements.rotation} | 홀드 조각 {placements.holdPiece.name} | 가능한 배치 {len(placements.placements)}개")

                    for placement in placements.placements:
                        actions = ", ".join(action.name for action in placement.actions)
                        print(f"  - ({placement.x}, {placement.y}) rotation={placement.rotation} : [{actions}]")

                    # 테스트용: 1번 게임의 배치 목록에서 인덱스를 정하고,
                    # 같은 인덱스를 짝수 번호 게임에도 각각 한 번씩 실행한다.
                    if getattr(server, 'selectionPending', False) and placements.placements:
                        if server.testPlacementIndex is None:
                            # 기준은 항상 1번 게임의 배치 목록
                            if placements.gameID == 1:
                                server.testPlacementIndex = min(20, len(placements.placements) - 1)

                                # 1번보다 먼저 배치 목록이 도착한 게임도 놓치지 않도록 처리
                                knownGameIDs = [gid for (sid, gid) in server.placementsByGameID if sid == server.sessionID]

                                for knownGameID in knownGameIDs:
                                    SendInitialTestCommand(knownGameID, server.testPlacementIndex)

                        if server.testPlacementIndex is not None:
                            SendInitialTestCommand(placements.gameID, server.testPlacementIndex)

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
                        gameCount = int(request.get("GameCount", 1))
                        server.gameCount = gameCount
                        gameIDSize = request.get("GameIDSize")

                        # gameID는 세션 안에서 1부터 시작하므로 재접속 시 이전 세션 상태와 구분할 세션 ID를 기억
                        server.sessionID = int(request.get("SessionID", 0))

                        dataQueue.put({"kind": "game_count", "count": gameCount})

                        # Configure가 GAME_ID_SIZE를 덮어쓰기 전에 서버 기본값과 비교한다.
                        expectedGameIDSize = GameState.GAME_ID_SIZE

                        # 클라이언트로 부터 받은 보드 크기, 큐 크기, 메시지 크기 정보로 GameState 클래스 설정 갱신
                        GameState.Configure(
                            boardWidth,
                            boardHeight,
                            queueSize,
                            int(pieceTypeCount) if pieceTypeCount is not None else None,
                            int(gameIDSize) if gameIDSize is not None else None,
                        )

                        if gameIDSize is not None and int(gameIDSize) != expectedGameIDSize:
                            print(f"[WARN] gameID 크기 불일치! 클라이언트={gameIDSize}, 서버={expectedGameIDSize}")

                        # 서버 내부적으로 설정한 메시지 크기와 클라이언트로부터 가져온 메시지 크기가 다르면 경고 로그 출력
                        if messageSize is not None and int(messageSize) != GameState.MESSAGE_SIZE:
                            print(f"[WARN] 클라이언트로부터 가져온 메시지 크기 불일치! 클라이언트={messageSize}, 서버={GameState.MESSAGE_SIZE}")

                        # 클라이언트에게 connection_check 응답 전송
                        response = {"Type": "ConnectionCheck", "Status": "Response"}
                        server.Send(json.dumps(response, separators=(',', ':')))

                    elif request.get("Type") == "SelectPathIndex":
                        # 서버에서 경로 인덱스를 선택하여 클라이언트에 응답 (클라이언트는 응답을 대기하지 않음)
                        index = request.get("Index", request.get("index"))

                        if index is None and request.get("PathIndex") is not None:
                            index = request.get("PathIndex")

                        gameID = request.get("GameID", request.get("gameID"))
                        sessionID = request.get("SessionID", request.get("sessionID"))

                        server.SelectPathIndex(index, gameID, sessionID)

                        continue

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
