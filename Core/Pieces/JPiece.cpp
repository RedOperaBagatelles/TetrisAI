#include "JPiece.h"

const std::array<std::array<Position, 4>, 4> JPiece::rotationData =
{
	std::array<Position, 4>{ Position{-1,  0}, Position{ 0,  0}, Position{ 1,  0}, Position{-1,  1} },
	std::array<Position, 4>{ Position{ 0,  1}, Position{ 0,  0}, Position{ 0, -1}, Position{ 1,  1} },
	std::array<Position, 4>{ Position{ 1,  0}, Position{ 0,  0}, Position{-1,  0}, Position{ 1, -1} },
	std::array<Position, 4>{ Position{ 0, -1}, Position{ 0,  0}, Position{ 0,  1}, Position{-1, -1} }
};

JPiece::JPiece(int x, int y) : Piece(x, y)
{
}

const std::array<std::array<Position, 4>, 4>& JPiece::GetRotationData() const
{
	return rotationData;
}


