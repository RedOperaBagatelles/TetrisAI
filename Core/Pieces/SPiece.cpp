#include "SPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

SPiece::SPiece(Tetris& tetris) : Piece(4, 0, tetris)
{
	Draw();
}

void SPiece::Update(float deltaTime)
{
	Piece::Update(deltaTime);
}

const low_uint(&SPiece::GetRotateShape() const)[4][4][4]
{
	return rotateShape;
}

const PieceType SPiece::GetPieceType() const
{
	return PieceType::S;
}