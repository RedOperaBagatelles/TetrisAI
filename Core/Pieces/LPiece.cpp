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
void LPiece::Move(Position beforePosition)
{
	PIECE_MOVE(beforePosition);
}

void LPiece::Draw()
{
	PIECE_DRAW(PieceType::L);
}