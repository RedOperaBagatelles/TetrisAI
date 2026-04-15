#pragma once

#include "Piece.h"

#include <array>

class ZPiece : public Piece
{
public:
	ZPiece(int x, int y);

protected:
	const std::array<std::array<Position, 4>, 4>& GetRotationData() const override;

private:
	static const std::array<std::array<Position, 4>, 4> rotationData;
};

