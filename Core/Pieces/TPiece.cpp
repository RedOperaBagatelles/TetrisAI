#include "TPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

TPiece::TPiece(Tetris& tetris) : Piece(3, 0, tetris)
{

}

void TPiece::Initialize()
{
	Draw();
}

void TPiece::Update(float deltaTime)
{
	Piece::Update(deltaTime);
}

const low_uint(&TPiece::GetRotateShape() const)[4][4][4]
{
	return rotateShape;
}

const PieceType TPiece::GetPieceType() const
{
	return PieceType::T;
}