import json
from queue import Queue
from threading import Thread
from TCPServer import TCPServer
from GameState import GameState, PieceType
from GameVisualizer import GameVisualizer


def server_loop(data_queue: Queue):
    """서버 메인 루프 (별도 스레드에서 실행)"""

    while True:
        try:
            server = TCPServer()
            server.start()
            server.accept()

            while True:
                kind, message = server.recv_message()

                if kind is None:
                    break

                if kind == "game_state":
                    state = GameState.from_bytes(message)
                    data_queue.put((state, message))
                    continue

                try:
                    request = json.loads(message)

                    if (request.get("type") == "connection_check" and request.get("status") == "request"):
                        board_width = int(request.get("board_width", GameState.BOARD_WIDTH))
                        board_height = int(request.get("board_height", GameState.BOARD_HEIGHT))
                        queue_size = int(request.get("queue_size", GameState.QUEUE_SIZE))
                        piece_type_count = request.get("piece_type_count")
                        client_message_size = request.get("message_size")

                        GameState.configure(board_width, board_height, queue_size, int(piece_type_count) if piece_type_count is not None else None,)

                        if client_message_size is not None and int(client_message_size) != GameState.MESSAGE_SIZE:
                            print(f"[WARN] message_size mismatch! client={client_message_size}, server={GameState.MESSAGE_SIZE}")

                        response = {"type": "connection_check", "status": "response"}
                        server.send(json.dumps(response, separators=(',', ':')))

                    else:
                        server.send(json.dumps({"error": "invalid_request"}))

                except json.JSONDecodeError:
                    server.send('{"error":"invalid_json"}')

        except Exception as e:
            print(f"[ERROR] Server exception: {e}")

        finally:
            try:
                server.close()
            except Exception:
                pass


data_queue = Queue()

server_thread = Thread(target=server_loop, args=(data_queue,), daemon=True)
server_thread.start()

visualizer = GameVisualizer(data_queue)
visualizer.run()