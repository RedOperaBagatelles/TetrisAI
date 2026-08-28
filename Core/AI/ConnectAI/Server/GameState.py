from dataclasses import dataclass
from enum import IntEnum
from typing import List, Optional


# 조각 종류를 나타내는 열거형 (Core/Pieces/Piece.h 의 enum class PieceType 과 값이 반드시 일치해야 함)
class PieceType(IntEnum):
    NONE    = 0
    I       = 1
    J       = 2
    L       = 3
    O       = 4
    S       = 5
    T       = 6
    Z       = 7
    GHOST   = 8


# 현재 조각의 종류, 회전 상태, 위치 (Core/AI/AIOutput.h 의 AIPieceState 대응)
@dataclass
class PieceState:
    type        : PieceType
    rotation    : int
    x           : int
    y           : int


# 클라이언트로부터 수신한 게임 상태를 나타내는 클래스 (Core/AI/AIOutput.h 의 AIOutput 대응)
@dataclass
class GameState:
    BOARD_HEIGHT = 23           # Tetris::maxHeight (height + topMarginBlock + 1)
    BOARD_WIDTH = 10            # Tetris::width
    NEXT_PIECE_QUEUE_SIZE = 5   # PiecesQueue::queueSize

    MESSAGE_TYPE = 0x01  # AIOutput::messageType (game_state 메시지 식별 바이트)

    # 헤더(1) + 보드(BOARD_HEIGHT * BOARD_WIDTH) + 현재 조각(type, rotation, x, y = 4) + 홀드 조각(1) + 다음 조각들(NEXT_PIECE_QUEUE_SIZE)
    MESSAGE_SIZE = 1 + (BOARD_HEIGHT * BOARD_WIDTH) + 4 + 1 + NEXT_PIECE_QUEUE_SIZE

    board: List[List[int]]      # 보드 정보를 담는 2차원 리스트 (0=빈칸, 1~7=조각 종류)
    currentPiece: PieceState    # 현재 조각의 종류
    holdPiece: PieceType        # 홀드 조각의 종류
    nextPieces: List[PieceType] # 다음 조각들의 종류를 담는 리스트 (크기 = NEXT_PIECE_QUEUE_SIZE)

    # 첫 클라이언트와 연결 시 C++ 클라이언트로부터 수신한 보드 크기, 큐 크기, 메시지 크기 정보로 GameState 클래스 설정 갱신
    @classmethod
    def Configure(cls, boardWidth: int, boardHeight: int, nextPieceQueueSize: int, pieceTypeCount: Optional[int] = None, ) -> None:
        cls.BOARD_WIDTH = boardWidth
        cls.BOARD_HEIGHT = boardHeight
        cls.NEXT_PIECE_QUEUE_SIZE = nextPieceQueueSize

        cls.MESSAGE_SIZE = 1 + (boardHeight * boardWidth) + 4 + 1 + nextPieceQueueSize

        # pieceTypeCount가 제공되면 PieceType 열거형의 예상 개수와 비교하여 경고 로그 출력
        if pieceTypeCount is not None:
            serverPieceTypeCount = len(PieceType)

            if pieceTypeCount != serverPieceTypeCount:
                print(f"[WARN] 서버에서 내부적으로 구성한 조각 유형 수와 다름! "f"클라이언트={pieceTypeCount}, 서버={serverPieceTypeCount}")

    # AIOutput::ToByte() 와 동일한 순서로 바이트를 파싱해 GameState 객체로 변환
    @classmethod
    def FromBytes(thisClass, data: bytes) -> "GameState":
        # MESSAGE_SIZE와 MESSAGE_TYPE 검증
        if len(data) != thisClass.MESSAGE_SIZE:
            raise ValueError(f"잘못된 게임 상태 크기 : {len(data)} (expected {thisClass.MESSAGE_SIZE})")

        if data[0] != thisClass.MESSAGE_TYPE:
            raise ValueError(f"잘못된 메시지 유형 : {data[0]:#x} (expected {thisClass.MESSAGE_TYPE:#x})")
        
        offset = 1

        board = []

        for y in range(thisClass.BOARD_HEIGHT):
            board.append(list(data[offset:offset + thisClass.BOARD_WIDTH]))
            offset += thisClass.BOARD_WIDTH

        # 현재 조각의 종류 정보 파싱
        currentPiece = PieceState(type=PieceType(data[offset]), rotation=data[offset + 1], x=data[offset + 2], y=data[offset + 3], )
        offset += 4

        # 홀드 조각의 종류 정보 파싱
        holdPiece = PieceType(data[offset])
        offset += 1

        # 다음 조각들의 종류 정보 파싱
        nextPieces = [PieceType(value) for value in data[offset:offset + thisClass.NEXT_PIECE_QUEUE_SIZE]]

        return thisClass(board=board, currentPiece=currentPiece, holdPiece=holdPiece, nextPieces=nextPieces)