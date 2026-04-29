#pragma once

#include "Tetris.h"

#include "Core/GameLoop.h"
#include "Core/InputSystem/Keyboard.h"

#include <windows.h>
#include <vector>
#include <memory>


class Window : public GameLoop
{
public:
	void Initialize() override;
	void Update(float deltaTime = 0.0f) override;
	bool Render() override;

	void KeyBoardInput(float deltaTime);

	// RenderTarget에 그릴 sf::Drawable 객체를 추가하는 메소드
	void AddRenderTarget(const std::shared_ptr<const sf::Drawable>& drawable);
	void AddRenderTargets(const std::vector<std::shared_ptr<const sf::Drawable>>& drawables);

	Tetris& GetTetris() { return tetris; }

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	sf::RenderWindow window{ sf::VideoMode({width, height}), "Tetris" };
	std::vector<std::shared_ptr<const sf::Drawable>> renderTargets;		// 해당 프레임에 그릴 객체들을 저장하는 벡터
	Tetris tetris{ *this };												// 테트리스 게임 객체
	Keyboard keyboard;													// 키보드 입력 시스템 객체

	HWND hWnd = nullptr;
	WNDPROC originalWndProc = nullptr;

	static Window* instance;

	constexpr static int width = 1280;	// 창의 너비
	constexpr static int height = 720;	// 창의 높이
};