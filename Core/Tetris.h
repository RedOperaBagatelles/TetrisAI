#pragma once

#include "SFML/Graphics.hpp"

#include <vector>
#include <memory>

class Tetris
{
public:
	void Ready(int x, int y);
	void Update(class Window& window);

	static constexpr int height = 20;
	static constexpr int width = 10;

private:
	static constexpr int topMarginBlock = 2;	// 맵의 위쪽에 보이지 않는 블록 영역 (회전 시 충돌 체크용)

	int startX = 0;
	int startY = 0;

	short board[height + topMarginBlock][width] = { 0 };
	std::vector<std::shared_ptr<const sf::Drawable>> currentRenderPieces;
};