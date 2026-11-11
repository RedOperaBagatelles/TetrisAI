#pragma once

#include "Core/Pieces/Piece.h"
#include "Utility/Type.h"

#include <deque>

class PiecesQueue
{
public:
	PiecesQueue();

	PieceType GetPiece();								// 다음 조각의 종류를 반환하는 메소드
	const std::deque<PieceType>& GetNextPieces() const;	// 대기 중인 조각의 종류를 반환하는 메소드
	void Reset();										// 이 보드의 공유 큐 진행 위치를 처음으로 되돌림

	static constexpr low_uint queueSize = 5;	// 다음에 나올 조각들의 최대 개수

private:
	static void EnsurePieces(size_t count);		// sharedPieces에 최소 count개의 조각이 있도록 보장하는 메소드

	static std::deque<PieceType> sharedPieces;	// 모든 보드가 공유하는 피스 순서
	size_t pieceIndex = 0;						// 이 보드가 소비할 다음 순서
	mutable std::deque<PieceType> nextPieces;	// UI 표시용 다음 피스 목록
};
