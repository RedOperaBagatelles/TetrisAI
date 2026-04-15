#include "OPiece.h"

const std::array<std::array<Position, 4>, 4> OPiece::rotationData =
{
	std::array<Position, 4>{ Position{ 0,  0}, Position{ 1,  0}, Position{ 0,  1}, Position{ 1,  1} },
	std::array<Position, 4>{ Position{ 0,  0}, Position{ 1,  0}, Position{ 0,  1}, Position{ 1,  1} },
	std::array<Position, 4>{ Position{ 0,  0}, Position{ 1,  0}, Position{ 0,  1}, Position{ 1,  1} },
	std::array<Position, 4>{ Position{ 0,  0}, Position{ 1,  0}, Position{ 0,  1}, Position{ 1,  1} }
};

OPiece::OPiece(int x, int y) : Piece(x, y)
{
}

void OPiece::Rotate(class Tetris& tetris)
{
	(void)tetris;
}

void OPiece::BackwardRotate(class Tetris& tetris)
{
	(void)tetris;
}

const std::array<std::array<Position, 4>, 4>& OPiece::GetRotationData() const
{
	return rotationData;
}


