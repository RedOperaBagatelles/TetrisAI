#include "PiecesQueue.h"

#include <random>

std::deque<PieceType> PiecesQueue::sharedPieces;

PiecesQueue::PiecesQueue()
{
	EnsurePieces(queueSize);
}

PieceType PiecesQueue::GetPiece()
{
	EnsurePieces(pieceIndex + queueSize);

	const PieceType piece = sharedPieces[pieceIndex++];
	EnsurePieces(pieceIndex + queueSize);

	return piece;
}

const std::deque<PieceType>& PiecesQueue::GetNextPieces() const
{
	// 다음 조각 큐에서 현재 보드가 소비할 다음 순서까지(QueueSize)의 조각들을 확보
	EnsurePieces(pieceIndex + queueSize);

	nextPieces.clear();

	for (size_t i = pieceIndex; i < pieceIndex + queueSize; i++)
		nextPieces.push_back(sharedPieces[i]);

	return nextPieces;
}

void PiecesQueue::Reset()
{
	pieceIndex = 0;
	EnsurePieces(queueSize);
}

void PiecesQueue::EnsurePieces(size_t count)
{
	static std::mt19937 random(std::random_device{}());
	static std::uniform_int_distribution<int> distribution(1, static_cast<int>(PieceType::Z));

	// 지금까지 생성된 조각이 요청된 개수보다 적으면 새로운 조각을 생성하여 sharedPieces에 추가
	while (sharedPieces.size() < count)
		sharedPieces.push_back(static_cast<PieceType>(distribution(random)));
}
