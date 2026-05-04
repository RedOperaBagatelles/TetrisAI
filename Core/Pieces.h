#pragma once

#include "Pieces/Piece.h"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Drawable.hpp"

#include <array>
#include <vector>
#include <memory>

class Pieces
{
public:
	// 벽을 나타내는 사각형들을 반환하는 메소드
	static std::vector<std::shared_ptr<const sf::Drawable>> GetRenderWalls(int x, int y);

	// 해당 위치에 해당하는 PieceType의 렌더링용 사각형을 반환
	static std::shared_ptr<const sf::RectangleShape> GetRenderOnePices(int x, int y, PieceType type, int offsetX, int offsetY);

private:
	// 각 PieceType에 해당하는 색상을 저장하는 배열
	static constexpr std::array<sf::Color, 9> colors =
	{
		sf::Color(197, 15, 31),
		sf::Color(97, 214, 214),
		sf::Color(0, 55, 218),
		sf::Color(255, 165, 0), // Orange
		sf::Color::Yellow,
		sf::Color(19, 161, 14),
		sf::Color(136, 23, 152),
		sf::Color(197, 15, 31),
		sf::Color(255, 255, 255, 50) // 고스트 조각의 색상 (반투명 흰색)
	};

	static constexpr sf::Color wallColor = sf::Color(118, 118, 118);
	static constexpr int blockPadding = 2;	// 각 블록 사이의 간격 (픽셀 단위)
	static constexpr int blockSize = 20;	// 블록 사이즈
};