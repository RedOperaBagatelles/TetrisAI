#pragma once

#include "Core/AI/Control/PiecePlacementExplorer.h"
#include "Utility/Type.h"

#include <cstdint>
#include <map>
#include <vector>

// 현재 조각의 상태와 가능한 배치 목록을 서버로 전송하는 클래스
class PiecePlacementSender
{
public:
	PiecePlacementSender() = delete;

	// 현재 조각의 X, Y 위치, 회전 상태, 종류, 홀드 조각의 종류와 가능한 배치 목록을 서버로 전송하는 메소드
	static void Send(const class Tetris& tetris, const std::vector<PiecePlacement>& placements);

	// 서버 연결 전에 계산되어 전송하지 못한 메시지를 연결 성공 후 전송하는 메소드
	static void Flush();

	// 헤더 : 타입(1) + gameID(gameIDSize) + 현재 조각(x, y, rotation = 3, type = 1) + 홀드 조각(1) + 배치 개수(2)
	static constexpr size_t headerSize = 1 + gameIDSize + 3 + 1 + 1 + 2;
	static constexpr size_t placementSize = 3 + 2;

private:
	static constexpr uint8_t messageType = 0x02;	// possible_placements 메시지를 나타내는 식별 바이트

	// 서버 연결 전에 계산되어 아직 전송하지 못한 메시지 (게임별 마지막 메시지 보관)
	static std::map<GameCapacity, std::vector<uint8_t>> pendingMessages;
};
