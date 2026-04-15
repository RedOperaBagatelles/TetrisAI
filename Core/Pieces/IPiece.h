#pragma once

#include "Piece.h"

#include <array>

class IPiece : public Piece
{
public:
	IPiece(int x, int y);

protected:
	const std::array<std::array<Position, 4>, 4>& GetRotationData() const override;
	const std::array<std::array<Position, 5>, 4>& GetClockwiseKickData() const override;
	const std::array<std::array<Position, 5>, 4>& GetCounterClockwiseKickData() const override;

private:
	static const std::array<std::array<Position, 4>, 4> rotationData;
	static const std::array<std::array<Position, 5>, 4> iClockwiseKickData;
	static const std::array<std::array<Position, 5>, 4> iCounterClockwiseKickData;
};
