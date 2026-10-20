from dataclasses import dataclass
from enum import IntEnum
from typing import List


# AI가 수행할 수 있는 행동을 나타내는 열거형 (Core/AI/AIInput.h 의 enum class AIAction 과 값이 반드시 일치해야 함)
class AIAction(IntEnum):
    NONE                    = 0
    MOVE_LEFT               = 1
    MOVE_RIGHT              = 2
    MOVE_DOWN               = 3
    ROTATE_CLOCKWISE        = 4
    ROTATE_COUNTERCLOCKWISE = 5
    HARD_DROP               = 6
    HOLDING                 = 7


# 하나의 배치(조각을 놓을 위치와 회전 상태, 도달하기 위한 행동 시퀀스)를 나타내는 클래스
@dataclass
class Placement:
    x       : int            # 조각을 놓을 X 위치
    y       : int            # 조각을 놓을 Y 위치
    rotation: int            # 조각을 놓을 회전 상태
    actions : List[AIAction] # 해당 위치까지 도달하기 위한 행동들의 시퀀스


# 클라이언트로부터 수신한 가능한 배치 목록을 나타내는 클래스 (Core/AI/ConnectAI/PiecePlacementSender 대응)
@dataclass
class PossiblePlacements:
    MESSAGE_TYPE = 0x02  # PiecePlacementSender::messageType (possible_placements 메시지 식별 바이트)

    # 헤더(1) + 현재 조각(x, y, rotation = 3) + 배치 개수(2)
    HEADER_SIZE = 1 + 3 + 2

    # 배치(위치 x, y, rotation = 3) + 액션 개수(2)
    PLACEMENT_SIZE = 3 + 2

    x         : int                 # 현재 조각의 X 위치
    y         : int                 # 현재 조각의 Y 위치
    rotation  : int                 # 현재 조각의 회전 상태
    placements: List[Placement]     # 조각을 놓을 수 있는 모든 경우의 수

    # PiecePlacementSender::Send() 와 동일한 순서로 바이트를 파싱해 PossiblePlacements 객체로 변환
    @classmethod
    def FromBytes(thisClass, data: bytes) -> "PossiblePlacements":
        if len(data) < thisClass.HEADER_SIZE:
            raise ValueError(f"잘못된 배치 메시지 크기 : {len(data)} (expected >= {thisClass.HEADER_SIZE})")

        if data[0] != thisClass.MESSAGE_TYPE:
            raise ValueError(f"잘못된 메시지 유형 : {data[0]:#x} (expected {thisClass.MESSAGE_TYPE:#x})")

        offset = 1

        # 현재 조각의 위치와 회전 상태 파싱
        x = data[offset]
        y = data[offset + 1]
        rotation = data[offset + 2]
        offset += 3

        # 배치 개수 파싱 (리틀엔디안 2바이트)
        placementCount = int.from_bytes(data[offset:offset + 2], 'little')
        offset += 2

        placements = []

        for _ in range(placementCount):
            if offset + thisClass.PLACEMENT_SIZE > len(data):
                raise ValueError(f"배치 데이터가 끊겼습니다 : {len(data)}바이트, offset={offset}")

            # 배치의 위치와 회전 상태, 액션 개수 파싱
            placementX = data[offset]
            placementY = data[offset + 1]
            placementRotation = data[offset + 2]
            actionCount = int.from_bytes(data[offset + 3:offset + 5], 'little')
            offset += 5

            if offset + actionCount > len(data):
                raise ValueError(f"액션 데이터가 끊겼습니다 : {len(data)}바이트, offset={offset}")

            # 액션 시퀀스 파싱
            actions = [AIAction(value) for value in data[offset:offset + actionCount]]
            offset += actionCount

            placements.append(Placement(x=placementX, y=placementY, rotation=placementRotation, actions=actions))

        # 남은 바이트가 있으면 잘못된 메시지
        if offset != len(data):
            raise ValueError(f"잘못된 배치 메시지 크기 : {len(data)} (expected {offset})")

        return thisClass(x=x, y=y, rotation=rotation, placements=placements)
