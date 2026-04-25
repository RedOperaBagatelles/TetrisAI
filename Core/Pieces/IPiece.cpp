#include "IPiece.h"
#include "Piece.h"

#include "Core/Tetris.h"
#include "Utility/Type.h"

IPiece::IPiece(Tetris& tetris) : Piece(3, 0, tetris) // I 조각은 초기 위치가 (3, 0)으로 설정됨
{
	Draw();
}

void IPiece::Update(float deltaTime)
{
	Piece::Update(deltaTime);
}

void IPiece::Move(Position beforePosition)
{
	PIECE_MOVE(beforePosition);
}

void IPiece::Draw()
{
	PIECE_DRAW(PieceType::I);
}