#include "SPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

SPiece::SPiece(Tetris& tetris) : Piece(4, 0)
{
	auto map = tetris.GetBoard();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			map[Tetris::maxHeight - 1 - i][currentX + j] = rotateShape[currentRotation][i][j] * (low_uint)PieceType::S; // S 조각의 블록을 게임 보드에 배치
	}
}