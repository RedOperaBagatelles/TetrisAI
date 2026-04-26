#include "ZPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

ZPiece::ZPiece(Tetris& tetris) : Piece(3, 0, tetris)
{
	Draw();
}

void ZPiece::Update(float deltaTime)
{
	Piece::Update(deltaTime);
}

const low_uint(&ZPiece::GetRotateShape() const)[4][4][4]
{
	return rotateShape;
}

const PieceType ZPiece::GetPieceType() const
{
	return PieceType::Z;
}