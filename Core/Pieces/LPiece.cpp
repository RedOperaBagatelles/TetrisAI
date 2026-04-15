#include "LPiece.h"

const std::array<std::array<Position, 4>, 4> LPiece::rotationData =
{
	std::array<Position, 4>{ Position{-1,  0}, Position{ 0,  0}, Position{ 1,  0}, Position{ 1,  1} },
	std::array<Position, 4>{ Position{ 0,  1}, Position{ 0,  0}, Position{ 0, -1}, Position{ 1, -1} },
	std::array<Position, 4>{ Position{-1,  0}, Position{ 0,  0}, Position{ 1,  0}, Position{-1, -1} },
	std::array<Position, 4>{ Position{-1,  1}, Position{ 0,  1}, Position{ 0,  0}, Position{ 0, -1} }
};

LPiece::LPiece(int x, int y) : Piece(x, y)
{
}

const std::array<std::array<Position, 4>, 4>& LPiece::GetRotationData() const
{
	return rotationData;
}


