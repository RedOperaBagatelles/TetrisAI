from dataclasses import dataclass
from enum import IntEnum
from typing import List, Optional


class PieceType(IntEnum):
    """Core/Pieces/Piece.h 의 enum class PieceType 과 값이 반드시 일치해야 함"""
    NONE = 0
    I = 1
    J = 2
    L = 3
    O = 4
    S = 5
    T = 6
    Z = 7
    GHOST = 8


@dataclass
class PieceState:
    """현재 조각의 종류, 회전 상태, 위치 (Core/AI/AIOutput.h 의 AIPieceState 대응)"""
    type: PieceType
    rotation: int
    x: int
    y: int


@dataclass
class GameState:
    """Core/AI/AIOutput.cpp 의 AIOutput::ToByte() 가 직렬화한 게임 상태를 역직렬화한 결과

    아래 크기 관련 값(BOARD_HEIGHT, BOARD_WIDTH, QUEUE_SIZE, MESSAGE_SIZE)은
    connection_check 핸드셰이크 시 C++ 클라이언트로부터 수신한 값으로 configure()를 통해
    갱신될 수 있음. 클래스 정의 시점의 값은 C++과 어긋났을 때를 대비한 기본값(fallback)임.
    """

    BOARD_HEIGHT = 23  # Tetris::maxHeight (height + topMarginBlock + 1)
    BOARD_WIDTH = 10   # Tetris::width
    QUEUE_SIZE = 5     # PiecesQueue::queueSize

    MESSAGE_TYPE = 0x01  # AIOutput::messageType (game_state 메시지 식별 바이트)

    # 헤더(1) + 보드(BOARD_HEIGHT * BOARD_WIDTH) + 현재 조각(type, rotation, x, y = 4) + 홀드 조각(1) + 다음 조각들(QUEUE_SIZE)
    MESSAGE_SIZE = 1 + (BOARD_HEIGHT * BOARD_WIDTH) + 4 + 1 + QUEUE_SIZE

    board: List[List[int]]
    current_piece: PieceState
    hold_piece: PieceType
    next_pieces: List[PieceType]

    @classmethod
    def configure(cls, board_width: int, board_height: int, queue_size: int, piece_type_count: Optional[int] = None, ) -> None:
        """connection_check 핸드셰이크 시 C++ 클라이언트로부터 수신한 크기 정보로 GameState의 클래스 설정(보드 크기, 큐 크기, 메시지 크기)을 갱신
        piece_type_count가 주어지면 PieceType enum의 멤버 개수와 일치하는지 검증만 하고 (PieceType 값 자체는 재구성하지 않음), 불일치 시 경고 로그만 남김
        """

        cls.BOARD_WIDTH = board_width
        cls.BOARD_HEIGHT = board_height
        cls.QUEUE_SIZE = queue_size

        cls.MESSAGE_SIZE = 1 + (board_height * board_width) + 4 + 1 + queue_size

        if piece_type_count is not None:
            expected_count = len(PieceType)

            if piece_type_count != expected_count:
                print(f"[WARN] piece_type_count mismatch! "f"client={piece_type_count}, server_enum_count={expected_count}")

    @classmethod
    def from_bytes(cls, data: bytes) -> "GameState":
        """AIOutput::ToByte() 와 동일한 순서로 바이트를 파싱해 GameState 객체로 변환"""

        if len(data) != cls.MESSAGE_SIZE:
            raise ValueError(f"Invalid game state size: {len(data)} (expected {cls.MESSAGE_SIZE})")

        if data[0] != cls.MESSAGE_TYPE:
            raise ValueError(f"Invalid message type: {data[0]:#x} (expected {cls.MESSAGE_TYPE:#x})")

        offset = 1

        board = []

        for y in range(cls.BOARD_HEIGHT):
            board.append(list(data[offset:offset + cls.BOARD_WIDTH]))
            offset += cls.BOARD_WIDTH

        current_piece = PieceState(type=PieceType(data[offset]), rotation=data[offset + 1], x=data[offset + 2], y=data[offset + 3], )
        offset += 4

        hold_piece = PieceType(data[offset])
        offset += 1

        next_pieces = [PieceType(value) for value in data[offset:offset + cls.QUEUE_SIZE]]

        return cls(board=board, current_piece=current_piece, hold_piece=hold_piece, next_pieces=next_pieces)