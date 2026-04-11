#include "Tetris.h"
#include "Pieces.h"
#include "Window.h"

#include "SFML/Graphics.hpp"

void Tetris::Ready(int x, int y)
{
	startX = x;
	startY = y;

	currentRenderPieces.clear();

	// 테스트용 맵 데이터 설정
	board[0][0] = 1;	// I
	board[0][1] = 2;	// J
	board[0][2] = 3;	// L
	board[0][3] = 4;	// O
	board[0][4] = 5;	// S
	board[0][5] = 6;	// T
	board[0][6] = 7;	// Z

}

void Tetris::Update(Window& window)
{
	currentRenderPieces.clear();

	// 맵에 있는 모든 조각들의 렌더링 객체 생성
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			// 해당 위치에 조각이 없으면 건너뜀
			if (board[i][j] == 0)
				continue;

			Pieces pieces;
			currentRenderPieces.push_back(pieces.GetRenderOnePices(j, i, static_cast<PieceType>(board[i][j])));
		}
	}

	window.AddRenderTargets(currentRenderPieces);
}