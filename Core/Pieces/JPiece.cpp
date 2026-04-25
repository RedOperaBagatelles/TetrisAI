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

void JPiece::Move(Position beforePosition)
{
	PIECE_MOVE(beforePosition);
}

void JPiece::Draw()
{
	PIECE_DRAW(PieceType::J);
}