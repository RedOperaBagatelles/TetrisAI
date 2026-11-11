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

	// 실제 게임 보드의 상태를 AIOutputData 구조체에 복사
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

		// 현재 조각의 위치와 회전 상태를 기반으로 보드에서 해당 조각의 블록을 제거
		const auto& shape = currentPiece->GetRotateShape()[data.currentPiece.rotation];
		const Position position = data.currentPiece.position;
		const int pieceType = static_cast<int>(data.currentPiece.type);

		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				// shape 배열에서 0인 부분은 조각이 없는 부분이므로 건너뜀
				if (shape[y][x] == 0)
					continue;

				// 보드 좌표를 계산하고, 해당 좌표가 보드 범위 내에 있으며 현재 조각의 블록과 일치하는 경우 보드에서 제거
				const int boardX = static_cast<int>(position.x) + x;
				const int boardY = Tetris::maxHeight - (static_cast<int>(position.y) + y + 1);

				// 보드 좌표가 유효한 범위 내에 있는지 확인하고, 현재 조각의 블록과 일치하는 경우 보드에서 제거
				if (boardX >= 0 && boardX < Tetris::width && boardY >= 0 && boardY < Tetris::maxHeight && data.board[boardY][boardX] == pieceType)
					data.board[boardY][boardX] = 0;
			}
		}
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
