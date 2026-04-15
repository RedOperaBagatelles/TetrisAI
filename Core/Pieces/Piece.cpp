#include "Piece.h"

#include "Core/Tetris.h"

const std::array<std::array<Position, 5>, 4> Piece::defaultClockwiseKickData =
{
	std::array<Position, 5>{ Position{ 0,  0 }, Position{-1,  0 }, Position{-1,  1 }, Position{ 0, -2 }, Position{-1, -2 } }, // 0 -> 1
	std::array<Position, 5>{ Position{ 0,  0 }, Position{ 1,  0 }, Position{ 1, -1 }, Position{ 0,  2 }, Position{ 1,  2 } }, // 1 -> 2
	std::array<Position, 5>{ Position{ 0,  0 }, Position{ 1,  0 }, Position{ 1,  1 }, Position{ 0, -2 }, Position{ 1, -2 } }, // 2 -> 3
	std::array<Position, 5>{ Position{ 0,  0 }, Position{-1,  0 }, Position{-1, -1 }, Position{ 0,  2 }, Position{-1,  2 } }  // 3 -> 0
};

const std::array<std::array<Position, 5>, 4> Piece::defaultCounterClockwiseKickData =
{
	std::array<Position, 5>{ Position{ 0,  0 }, Position{ 1,  0 }, Position{ 1,  1 }, Position{ 0, -2 }, Position{ 1, -2 } }, // 0 -> 3
	std::array<Position, 5>{ Position{ 0,  0 }, Position{ 1,  0 }, Position{ 1, -1 }, Position{ 0,  2 }, Position{ 1,  2 } }, // 1 -> 0
	std::array<Position, 5>{ Position{ 0,  0 }, Position{-1,  0 }, Position{-1,  1 }, Position{ 0, -2 }, Position{-1, -2 } }, // 2 -> 1
	std::array<Position, 5>{ Position{ 0,  0 }, Position{-1,  0 }, Position{-1, -1 }, Position{ 0,  2 }, Position{-1,  2 } }  // 3 -> 2
};

Piece::Piece(int x, int y) : centerX(x), centerY(y)
{

}

void Piece::Rotate(Tetris& tetris)
{
	const int targetRotation = NormalizeRotation(currentRotation + 1);
	const auto& kickData = GetClockwiseKickData();
	const auto& rotData = GetRotationData();

	for (const auto& kick : kickData[currentRotation])
	{
		if (!CanPlace(tetris, centerX + kick.x, centerY + kick.y, rotData[targetRotation]))
			continue;

		centerX += kick.x;
		centerY += kick.y;
		currentRotation = targetRotation;

		return;
	}
}

void Piece::BackwardRotate(Tetris& tetris)
{
	const int targetRotation = NormalizeRotation(currentRotation - 1);
	const auto& kickData = GetCounterClockwiseKickData();
	const auto& rotData = GetRotationData();

	for (const auto& kick : kickData[currentRotation])
	{
		if (!CanPlace(tetris, centerX + kick.x, centerY + kick.y, rotData[targetRotation]))
			continue;

		centerX += kick.x;
		centerY += kick.y;
		currentRotation = targetRotation;

		return;
	}
}

bool Piece::CanRotate(const Tetris& tetris, int rotationCount) const
{
	return CanPlace(tetris, centerX, centerY, GetRotationData()[NormalizeRotation(rotationCount)]);
}

int Piece::NormalizeRotation(int value)
{
	return (value % rotationCount + rotationCount) % rotationCount;
}

bool Piece::CanPlace(const Tetris& tetris, int centerX, int centerY, const std::array<Position, 4>& blocks) const
{
	const auto board = tetris.GetBoard();

	for (const auto& block : blocks)
	{
		const int x = centerX + block.x;
		const int y = centerY + block.y;

		if (x < 0 || x >= Tetris::width)
			return false;

		if (y < 0 || y >= Tetris::height + Tetris::topMarginBlock)
			return false;

		if (board[y][x] != 0)
			return false;
	}

	return true;
}

const std::array<std::array<Position, 5>, 4>& Piece::GetClockwiseKickData() const
{
	return defaultClockwiseKickData;
}

const std::array<std::array<Position, 5>, 4>& Piece::GetCounterClockwiseKickData() const
{
	return defaultCounterClockwiseKickData;
}