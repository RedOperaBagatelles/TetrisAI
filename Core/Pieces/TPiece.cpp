#include "TPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

TPiece::TPiece(Tetris& tetris) : Piece(3, 0, tetris)
{
	Draw();
}

void TPiece::Update(float deltaTime)
{
	Piece::Update(deltaTime);
}

void TPiece::Move(Position beforePosition)
{
	PIECE_MOVE(beforePosition);
}

void TPiece::Draw()
{
	PIECE_DRAW(PieceType::T);
}