#pragma once

#include "Core/Pieces/Piece.h"
#include "Core/PiecesQueue.h"
#include "Core/Tetris.h"
#include "Utility/Type.h"

#include <cstdint>
#include <vector>

// 현재 조각의 종류, 회전 상태, 위치를 나타내는 데이터 구조
struct AIPieceState
{
	PieceType type = PieceType::None;
	low_uint rotation = 0;
	Position position;
};

// AI에게 전송할 게임 상태(맵 + 현재 조각 + 홀드 조각 + 대기 중인 다음 조각들)를 나타내는 데이터 구조
struct AIOutputData
{
	map_size board[Tetris::maxHeight][Tetris::width] = { 0 };
	AIPieceState currentPiece;
	PieceType holdPiece = PieceType::None;
	PieceType nextPieces[PiecesQueue::queueSize] = { PieceType::None };
};

class AIOutput
{
public:
	AIOutput(class Tetris& tetris);

	AIOutputData GetOutputData() const;		// 현재 게임 상태를 데이터 구조로 반환하는 메소드
	std::vector<uint8_t> ToByte() const;	// TCP 전송을 위해 현재 게임 상태를 고정 크기 바이너리로 직렬화하는 메소드

	// 직렬화된 메시지의 고정 바이트 크기 : 헤더(1) + 보드(maxHeight * width) + 조각 정보(type, rotation, x, y = 4) + 홀드 조각(1) + 대기 중인 다음 조각들(queueSize)
	static constexpr size_t messageSize = 1 + (Tetris::maxHeight * Tetris::width) + 4 + 1 + PiecesQueue::queueSize;

private:
	static constexpr uint8_t messageType = 0x01;	// game_state 메시지를 나타내는 식별 바이트

	Tetris& tetris;
};
