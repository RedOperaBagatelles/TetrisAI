#pragma once

#include "Utility/Type.h"

class BoardState
{
public:
	static constexpr map_size height = 20;
	static constexpr map_size width = 10;
	static constexpr map_size topMargin = 2;
	static constexpr map_size maxHeight = height + topMargin + 1;

	BoardState();

	void Reset();

	map_size(*GetBoard())[width];
	int GetScore() const;
	int GetLineClearCount() const;
	int GetPiecesPlaceCount() const;
	int GetBoardHeight() const;

	void AddLineClearCount(int count);		// 제거된 줄 수를 추가하고 점수를 계산
	void RecordPiecePlaced();				// 조각이 놓였음을 기록
	void RecalculateBoardHeight();			// 현재 보드 상태를 기반으로 보드의 높이를 재계산

private:
	map_size board[maxHeight][width] = {};

	int score = 0;
	int lineClearCount = 0;
	int piecesPlaceCount = 0;
	int boardHeight = 0;
};
