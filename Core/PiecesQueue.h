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

private:
	void AddPiece();

	static constexpr low_uint queueSize = 5;	// 다음에 나올 조각들의 최대 개수

	std::deque<PieceType> nextPieces;			// 다음에 나올 조각들의 종류를 저장하는 큐
};