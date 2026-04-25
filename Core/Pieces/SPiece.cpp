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

void SPiece::Move(Position beforePosition)
{
	PIECE_MOVE(beforePosition);
}

void SPiece::Draw()
{
	PIECE_DRAW(PieceType::S);
}