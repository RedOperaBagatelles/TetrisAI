#include "SPiece.h"

const std::array<std::array<Position, 4>, 4> SPiece::rotationData =
{
	std::array<Position, 4>{ Position{ 0,  1}, Position{ 1,  1}, Position{-1,  0}, Position{ 0,  0} },
	std::array<Position, 4>{ Position{ 0,  1}, Position{ 0,  0}, Position{ 1,  0}, Position{ 1, -1} },
	std::array<Position, 4>{ Position{ 0,  0}, Position{ 1,  0}, Position{-1, -1}, Position{ 0, -1} },
	std::array<Position, 4>{ Position{-1,  1}, Position{-1,  0}, Position{ 0,  0}, Position{ 0, -1} }
};

SPiece::SPiece(int x, int y) : Piece(x, y)
{
}

const std::array<std::array<Position, 4>, 4>& SPiece::GetRotationData() const
{
	return rotationData;
}


