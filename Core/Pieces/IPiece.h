#pragma once
#pragma once

#include "Piece.h"

#include <array>

class IPiece : public Piece
{
public:
	IPiece(int x, int y);

	void Rotate(class Tetris& tetris) override;
	void BackwardRotate(class Tetris& tetris) override;

private:
	bool CanRotate(const Tetris& tetris, int rotationCount) const override;
	bool CanPlace(const Tetris& tetris, int centerX, int centerY, const std::array<Position, 4>& blocks) const override;

	static constexpr std::array<std::array<Position, 4>, 4> rotationData =
	{
		std::array<Position, 4>{ Position{-1,  0}, Position{ 0,  0}, Position{ 1,  0}, Position{ 2,  0} },	// 0도
		std::array<Position, 4>{ Position{ 1,  1}, Position{ 1,  0}, Position{ 1, -1}, Position{ 1,  2} },	// 90도
		std::array<Position, 4>{ Position{-1,  1}, Position{ 0,  1}, Position{ 1,  1}, Position{ 2,  1} },	// 180도
		std::array<Position, 4>{ Position{ 0,  1}, Position{ 0,  0}, Position{ 0, -1}, Position{ 0,  2} }	// 270도
	};
};