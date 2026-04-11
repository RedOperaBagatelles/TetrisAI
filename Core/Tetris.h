#pragma once

#include "SFML/Graphics.hpp"

#include <vector>
#include <memory>

class Tetris
{
public:
	void Ready(int x, int y);
	void Update(class Window& window);

private:
	static constexpr int height = 20;
	static constexpr int width = 10;
	
	int startX = 0;
	int startY = 0;

	short board[height][width] = { 0 };
	std::vector<std::shared_ptr<const sf::Drawable>> currentRenderPieces;
};