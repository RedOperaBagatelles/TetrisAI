#pragma once

#include "Core/AI/Control/PiecePlacementExplorer.h"

#include <cstdint>
#include <vector>

// 현재 조각의 상태와 가능한 배치 목록을 서버로 전송하는 클래스
class PiecePlacementSender
{
public:
	PiecePlacementSender() = delete;

	// 현재 조각의 X, Y 위치, 회전 상태와 가능한 배치 목록을 서버로 전송하는 메소드
	static void Send(const class Tetris& tetris, const std::vector<PiecePlacement>& placements);

	// 서버 연결 전에 계산되어 전송하지 못한 메시지를 연결 성공 후 전송하는 메소드
	static void Flush();

	// 전송 가능한 배치 메시지의 최대 크기 : 헤더(1) + 현재 조각(x, y, rotation = 3) + 배치 개수(2) + 배치(위치 3 + 액션 개수 2 + 액션들)
	static constexpr size_t headerSize = 1 + 3 + 2;
	static constexpr size_t placementSize = 3 + 2;

private:
	static constexpr uint8_t messageType = 0x02;	// possible_placements 메시지를 나타내는 식별 바이트

	static std::vector<uint8_t> pendingMessage;	// 서버 연결 전에 계산되어 아직 전송하지 못한 메시지
};
