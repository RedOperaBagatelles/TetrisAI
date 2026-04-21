#include "IPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

IPiece::IPiece(Tetris& tetris) : Piece(3, 0) // I 조각은 초기 위치가 (3, 0)으로 설정됨
{
	auto map = tetris.GetBoard();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			map[Tetris::maxHeight - 1 - i][currentX + j] = rotateShape[currentRotation][i][j] * (low_uint)PieceType::I; // I 조각의 블록을 게임 보드에 배치
	}
}