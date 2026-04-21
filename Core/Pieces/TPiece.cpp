#include "TPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

TPiece::TPiece(Tetris& tetris) : Piece(3, 0)
{
	auto map = tetris.GetBoard();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			map[Tetris::maxHeight - 1 - i][currentX + j] = rotateShape[currentRotation][i][j] * (low_uint)PieceType::T; // T 조각의 블록을 게임 보드에 배치
	}
}