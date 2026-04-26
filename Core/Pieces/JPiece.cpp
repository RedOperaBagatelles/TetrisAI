#include "JPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

JPiece::JPiece(Tetris& tetris) : Piece(3, 0, tetris)
{
	Draw();
}

void JPiece::Update(float deltaTime)
{
	Piece::Update(deltaTime);
}

const low_uint(&JPiece::GetRotateShape() const)[4][4][4]
{
	return rotateShape;
}

const PieceType JPiece::GetPieceType() const
{
	return PieceType::J;
}