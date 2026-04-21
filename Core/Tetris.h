#pragma once

#include "Core/PiecesQueue.h"
#include "Core/Pieces/Piece.h"
#include "SFML/Graphics.hpp"
#include "Utility/Type.h"

#include <vector>
#include <memory>

class Window;

class Tetris
{
public:
	static constexpr map_size height = 20;
	static constexpr map_size width = 10;
	static constexpr map_size topMarginBlock = 2;						// 맵의 위쪽에 보이지 않는 블록 영역 (회전 시 충돌 체크용)
	static constexpr map_size maxHeight = height + topMarginBlock + 1;	// 게임 보드의 최대 높이 (보이지 않는 블록 영역 포함)

	void Ready(int x, int y);
	void Update(class Window& window);

	map_size (*GetBoard())[width] { return board; }	// 게임 보드의 2차원 배열을 반환하는 메소드

private:
	void CreatePiece();

	std::vector<std::shared_ptr<const sf::Drawable>> currentRenderPieces;
	std::unique_ptr<class Piece> currentPiece;					// 현재 조각 객체

	map_size board[maxHeight][width] = { 0 };
	PiecesQueue piecesQueue;									// 다음에 나올 조각들의 종류를 관리하는 객체

	map_size startX = 0;
	map_size startY = 0;
};