#include "AIOutput.h"

#include "Core/Pieces/Piece.h"
#include "Core/Tetris.h"

#include "Utility/Type.h"

#include <cstdint>
#include <deque>
#include <memory>
#include <vector>

AIOutput::AIOutput(Tetris& tetris) : tetris(tetris)
{

}

AIOutputData AIOutput::GetOutputData() const
{
	AIOutputData data;

	map_size(*board)[Tetris::width] = tetris.GetBoard();

	for (map_size y = 0; y < Tetris::maxHeight; y++)
	{
		for (map_size x = 0; x < Tetris::width; x++)
			data.board[y][x] = board[y][x];
	}

	std::shared_ptr<Piece> currentPiece = tetris.GetCurrentPiece();

	if (currentPiece != nullptr)
	{
		data.currentPiece.type = currentPiece->GetPieceType();
		data.currentPiece.rotation = currentPiece->GetRotation();
		data.currentPiece.position = currentPiece->GetPosition();
	}

	data.holdPiece = tetris.GetHoldPieceType();

	const std::deque<PieceType>& nextPieces = tetris.GetNextPieces();

	for (size_t i = 0; i < PiecesQueue::queueSize && i < nextPieces.size(); i++)
		data.nextPieces[i] = nextPieces[i];

	return data;
}

std::vector<uint8_t> AIOutput::ToByte() const
{
	AIOutputData data = GetOutputData();

	std::vector<uint8_t> bytes;
	bytes.reserve(messageSize);

	bytes.push_back(messageType);

	for (map_size y = 0; y < Tetris::maxHeight; y++)
	{
		for (map_size x = 0; x < Tetris::width; x++)
			bytes.push_back(static_cast<uint8_t>(data.board[y][x]));
	}

	bytes.push_back(static_cast<uint8_t>(data.currentPiece.type));
	bytes.push_back(static_cast<uint8_t>(data.currentPiece.rotation));
	bytes.push_back(static_cast<uint8_t>(data.currentPiece.position.x));
	bytes.push_back(static_cast<uint8_t>(data.currentPiece.position.y));

	bytes.push_back(static_cast<uint8_t>(data.holdPiece));

	for (size_t i = 0; i < PiecesQueue::queueSize; i++)
		bytes.push_back(static_cast<uint8_t>(data.nextPieces[i]));

	return bytes;
}
