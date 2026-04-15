#include "TPiece.h"

const std::array<std::array<Position, 4>, 4> TPiece::rotationData =
{
	std::array<Position, 4>{ Position{ 0,  1}, Position{-1,  0}, Position{ 0,  0}, Position{ 1,  0} },
	std::array<Position, 4>{ Position{ 0,  1}, Position{ 0,  0}, Position{ 1,  0}, Position{ 0, -1} },
	std::array<Position, 4>{ Position{-1,  0}, Position{ 0,  0}, Position{ 1,  0}, Position{ 0, -1} },
	std::array<Position, 4>{ Position{ 0,  1}, Position{-1,  0}, Position{ 0,  0}, Position{ 0, -1} }
};

TPiece::TPiece(int x, int y) : Piece(x, y)
{
}

const std::array<std::array<Position, 4>, 4>& TPiece::GetRotationData() const
{
	return rotationData;
}


