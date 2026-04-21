#include "PiecesQueue.h"

#include <deque>
#include <random>

PiecesQueue::PiecesQueue()
{
	// 초기 큐에 다음 조각들의 종류를 채움
	AddPiece();
}

PieceType PiecesQueue::GetPiece()
{
	if (nextPieces.empty())
		AddPiece();

	// 이번에 나올 조각의 종류를 큐에서 꺼냄
	PieceType nextPiece = nextPieces.front();
	nextPieces.pop_front();

	// 큐에 다음 조각의 종류가 부족하면 새로 추가
	AddPiece();

	return nextPiece;
}

const std::deque<PieceType>& PiecesQueue::GetNextPieces() const
{
	return nextPieces;
}

void PiecesQueue::AddPiece()
{
	while (nextPieces.size() < queueSize)
	{
		std::mt19937 random(std::random_device{}());								// 난수 생성기 초기화
		std::uniform_int_distribution<int> dist(1, static_cast<int>(PieceType::Z));	// 1~7 사이의 균등 분포 생성

		// 랜덤한 조각 종류 생성
		PieceType newPiece = static_cast<PieceType>(dist(random));
		nextPieces.push_back(newPiece);
	}
}