#pragma once

#include "Piece.h"

#include <array>

class TPiece : public Piece
{
public:
	TPiece(int x, int y);

protected:
	const std::array<std::array<Position, 4>, 4>& GetRotationData() const override;

private:
	static const std::array<std::array<Position, 4>, 4> rotationData;
};

