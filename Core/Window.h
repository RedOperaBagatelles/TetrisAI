#pragma once

#include "Tetris.h"

#include "SFML/Graphics.hpp"

#include <vector>
#include <memory>

class Window
{
public:
	void Ready();
	void Update();
	void Render();

	// RenderTarget에 그릴 sf::Drawable 객체를 추가하는 메소드
	void AddRenderTarget(const std::shared_ptr<const sf::Drawable>& drawable);
	void AddRenderTargets(const std::vector<std::shared_ptr<const sf::Drawable>>& drawables);

private:
	sf::RenderWindow window{ sf::VideoMode({width, height}), "Tetris" };
	std::vector<std::shared_ptr<const sf::Drawable>> renderTargets;		// 해당 프레임에 그릴 객체들을 저장하는 벡터
	Tetris tetris;														// 테트리스 게임 객체

	constexpr static int width = 1280;	// 창의 너비
	constexpr static int height = 720;	// 창의 높이
};