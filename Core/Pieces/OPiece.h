#pragma once

#include "Piece.h"

#include <array>

class OPiece : public Piece
{
public:
	OPiece(int x, int y);

	void Rotate(class Tetris& tetris) override;
	void BackwardRotate(class Tetris& tetris) override;

protected:
	const std::array<std::array<Position, 4>, 4>& GetRotationData() const override;

private:
	static const std::array<std::array<Position, 4>, 4> rotationData;
};

