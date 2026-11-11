#include "BoardState.h"

BoardState::BoardState()
{
	Reset();
}

void BoardState::Reset()
{
	for (int row = 0; row < maxHeight; row++)
	{
		for (int col = 0; col < width; col++)
			board[row][col] = 0;
	}

	score = 0;
	lineClearCount = 0;
	piecesPlaceCount = 0;
	boardHeight = 0;
}

int BoardState::GetLineClearCount() const
{
	return lineClearCount;
}

int BoardState::GetPiecesPlaceCount() const
{
	return piecesPlaceCount;
}

int BoardState::GetBoardHeight() const
{
	return boardHeight;
}

void BoardState::AddLineClearCount(int count)
{
	if (count <= 0)
		return;

	static constexpr int lineClearScores[] = { 0, 100, 300, 500, 800 };

	if (count <= 4)
		score += lineClearScores[count];

	lineClearCount += count;
}

void BoardState::RecordPiecePlaced()
{
	piecesPlaceCount++;
}

void BoardState::RecalculateBoardHeight()
{
	boardHeight = 0;

	for (int row = 0; row < height; row++)
	{
		bool occupied = false;

		for (int col = 0; col < width; col++)
		{
			if (board[row][col] != 0)
			{
				occupied = true;
				break;
			}
		}

		if (occupied)
			boardHeight = height - row;
	}
}

map_size(*BoardState::GetBoard())[width]
{
	return board;
}

int BoardState::GetScore() const
{
	return score;
}
