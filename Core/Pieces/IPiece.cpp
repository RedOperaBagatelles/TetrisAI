#include "IPiece.h"

#include "Core/Tetris.h"

const std::array<std::array<Position, 4>, 4> IPiece::rotationData =
{
	std::array<Position, 4>{ Position{-1,  0}, Position{ 0,  0}, Position{ 1,  0}, Position{ 2,  0} },	// 0도
	std::array<Position, 4>{ Position{ 1,  1}, Position{ 1,  0}, Position{ 1, -1}, Position{ 1,  2} },	// 90도
	std::array<Position, 4>{ Position{-1,  1}, Position{ 0,  1}, Position{ 1,  1}, Position{ 2,  1} },	// 180도
	std::array<Position, 4>{ Position{ 0,  1}, Position{ 0,  0}, Position{ 0, -1}, Position{ 0,  2} }	// 270도
};

const std::array<std::array<Position, 5>, 4> IPiece::iClockwiseKickData =
{
	std::array<Position, 5>{ Position{ 0,  0 }, Position{-2,  0 }, Position{ 1,  0 }, Position{-2, -1 }, Position{ 1,  2 } }, // 0 -> 1
	std::array<Position, 5>{ Position{ 0,  0 }, Position{-1,  0 }, Position{ 2,  0 }, Position{-1,  2 }, Position{ 2, -1 } }, // 1 -> 2
	std::array<Position, 5>{ Position{ 0,  0 }, Position{ 2,  0 }, Position{-1,  0 }, Position{ 2,  1 }, Position{-1, -2 } }, // 2 -> 3
	std::array<Position, 5>{ Position{ 0,  0 }, Position{ 1,  0 }, Position{-2,  0 }, Position{ 1, -2 }, Position{-2,  1 } }  // 3 -> 0
};

const std::array<std::array<Position, 5>, 4> IPiece::iCounterClockwiseKickData =
{
	std::array<Position, 5>{ Position{ 0,  0 }, Position{-1,  0 }, Position{ 2,  0 }, Position{-1,  2 }, Position{ 2, -1 } }, // 0 -> 3
	std::array<Position, 5>{ Position{ 0,  0 }, Position{ 2,  0 }, Position{-1,  0 }, Position{ 2,  1 }, Position{-1, -2 } }, // 1 -> 0
	std::array<Position, 5>{ Position{ 0,  0 }, Position{ 1,  0 }, Position{-2,  0 }, Position{ 1, -2 }, Position{-2,  1 } }, // 2 -> 1
	std::array<Position, 5>{ Position{ 0,  0 }, Position{-2,  0 }, Position{ 1,  0 }, Position{-2, -1 }, Position{ 1,  2 } }  // 3 -> 2
};


IPiece::IPiece(int x, int y) : Piece(x, y)
{
}

const std::array<std::array<Position, 4>, 4>& IPiece::GetRotationData() const
{
	return rotationData;
}

const std::array<std::array<Position, 5>, 4>& IPiece::GetClockwiseKickData() const
{
	return iClockwiseKickData;
}

const std::array<std::array<Position, 5>, 4>& IPiece::GetCounterClockwiseKickData() const
{
	return iCounterClockwiseKickData;
}

