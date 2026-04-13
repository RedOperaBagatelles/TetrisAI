#pragma once

#include <array>

struct Position { int x, y; };

class Piece
{
public:
	Piece(int x, int y);

	virtual void Rotate(class Tetris& tetris) = 0;
	virtual void BackwardRotate(class Tetris& tetris) = 0;

protected:
	virtual bool CanRotate(const Tetris& tetris, int rotationCount) const = 0;
	virtual bool CanPlace(const Tetris& tetris, int centerX, int centerY, const std::array<Position, 4>& blocks) const = 0;

	static int NormalizeRotation(int value);

	static constexpr int rotationCount = 4;

	int centerX;
	int centerY;

	int currentRotation = 0;

};