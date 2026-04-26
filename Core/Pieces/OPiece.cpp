#include "OPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

OPiece::OPiece(Tetris& tetris) : Piece(4, 0, tetris) // O 조각은 초기 위치가 (4, 0)으로 설정됨
{
	Draw();
}

void OPiece::Update(float deltaTime)
{
	Piece::Update(deltaTime);
}

const low_uint(&OPiece::GetRotateShape() const)[4][4][4]
{
	return rotateShape;
}

const PieceType OPiece::GetPieceType() const
{
	return PieceType::O;
}