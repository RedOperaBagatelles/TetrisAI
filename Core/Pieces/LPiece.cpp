#include "LPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

LPiece::LPiece(Tetris& tetris) : Piece(3, 0, tetris)
{
	Draw();
}

void LPiece::Update(float deltaTime)
{
	Piece::Update(deltaTime);
}

const low_uint(&LPiece::GetRotateShape() const)[4][4][4]
{
	return rotateShape;
}

const PieceType LPiece::GetPieceType() const
{
	return PieceType::L;
}