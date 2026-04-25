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

void ZPiece::Move(Position beforePosition)
{
	PIECE_MOVE(beforePosition);
}

void ZPiece::Draw()
{
	PIECE_DRAW(PieceType::Z);
}