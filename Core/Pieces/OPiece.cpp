#include "OPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

OPiece::OPiece(Tetris& tetris) : Piece(4, 0) // O 조각은 초기 위치가 (4, 0)으로 설정됨
{
	auto map = tetris.GetBoard();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			map[Tetris::maxHeight - 1 - i][currentX + j] = rotateShape[currentRotation][i][j] * (low_uint)PieceType::O; // O 조각의 블록을 게임 보드에 배치
	}
}
