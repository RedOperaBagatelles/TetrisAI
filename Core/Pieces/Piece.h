#pragma once

#include <array>

struct Position { int x, y; };

class Piece
{
public:
	Piece(int x, int y);

	virtual void Rotate(class Tetris& tetris);
	virtual void BackwardRotate(class Tetris& tetris);

protected:
	virtual bool CanRotate(const Tetris& tetris, int rotationCount) const;
	bool CanPlace(const Tetris& tetris, int centerX, int centerY, const std::array<Position, 4>& blocks) const;

	virtual const std::array<std::array<Position, 4>, 4>& GetRotationData() const = 0;
	virtual const std::array<std::array<Position, 5>, 4>& GetClockwiseKickData() const;
	virtual const std::array<std::array<Position, 5>, 4>& GetCounterClockwiseKickData() const;

	static int NormalizeRotation(int value);

	static constexpr int rotationCount = 4;

	int centerX;
	int centerY;

	int currentRotation = 0;

private:
	static const std::array<std::array<Position, 5>, 4> defaultClockwiseKickData;
	static const std::array<std::array<Position, 5>, 4> defaultCounterClockwiseKickData;
};