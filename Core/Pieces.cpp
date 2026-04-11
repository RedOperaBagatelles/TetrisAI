#include "Pieces.h"

#include <memory>

std::shared_ptr<const sf::RectangleShape> Pieces::GetRenderOnePices(int x, int y, PieceType type)
{
	std::shared_ptr<sf::RectangleShape> shape = std::make_shared<sf::RectangleShape>();
	shape->setSize({ 50, 50 });

	sf::Color color = colors[static_cast<int>(type)];
	shape->setFillColor(color);
	shape->setPosition({ static_cast<float>(x * 50), static_cast<float>(y * 50) });

	return shape;
}