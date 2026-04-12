#include "Pieces.h"
#include "Tetris.h"

#include <memory>

std::vector<std::shared_ptr<const sf::Drawable>> Pieces::GetRenderWalls(int x, int y)
{
	std::vector<std::shared_ptr<const sf::Drawable>> walls;

	static constexpr int size = blockSize - blockPadding;
	const int leftWallX = x;
	const int rightWallX = x + (Tetris::width + 1) * blockSize;
	const int bottomWallY = y + Tetris::height * blockSize;

	// 왼쪽 벽
	for (int i = 0; i <= Tetris::height; i++)
	{
		std::shared_ptr<sf::RectangleShape> leftWall = std::make_shared<sf::RectangleShape>();
		leftWall->setSize({ size, size });
		leftWall->setFillColor(wallColor);
		leftWall->setPosition({ static_cast<float>(leftWallX), static_cast<float>(y + i * blockSize) });

		walls.push_back(leftWall);
	}

	// 오른쪽 벽
	for (int i = 0; i <= Tetris::height; i++)
	{
		std::shared_ptr<sf::RectangleShape> rightWall = std::make_shared<sf::RectangleShape>();
		rightWall->setSize({ size, size });
		rightWall->setFillColor(wallColor);
		rightWall->setPosition({ static_cast<float>(rightWallX), static_cast<float>(y + i * blockSize) });
		walls.push_back(rightWall);
	}

	// 바닥 벽
	for (int i = 1; i <= Tetris::width; i++)
	{
		std::shared_ptr<sf::RectangleShape> bottomWall = std::make_shared<sf::RectangleShape>();
		bottomWall->setSize({ size, size });
		bottomWall->setFillColor(wallColor);
		bottomWall->setPosition({ static_cast<float>(x + i * blockSize), static_cast<float>(bottomWallY) });

		walls.push_back(bottomWall);
	}

	return walls;
}

std::shared_ptr<const sf::RectangleShape> Pieces::GetRenderOnePices(int x, int y, PieceType type, int offsetX, int offsetY)
{
	std::shared_ptr<sf::RectangleShape> shape = std::make_shared<sf::RectangleShape>();
	shape->setSize({ blockSize - blockPadding, blockSize - blockPadding });

	// 테투리로 인한 간격을 고려하여 위치 조정
	x++;
	y++;

	sf::Color color = colors[static_cast<int>(type)];
	shape->setFillColor(color);
   shape->setPosition({ static_cast<float>(offsetX + x * blockSize), static_cast<float>(offsetY + y * blockSize) });

	return shape;
}