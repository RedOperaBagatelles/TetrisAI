#pragma once

#include "Tetris.h"

#include "Core/GameLoop.h"
#include "Core/InputSystem/Keyboard.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <memory>


class Window : public GameLoop
{
public:
	void Initialize() override;
	void Update(float deltaTime = 0.0f) override;
	bool Render() override;

	void KeyBoardInput(float deltaTime);

	// RenderTarget에 그릴 sf::Drawable 객체를 추가하는 메소드 (소유권 없이 포인터만 보관)
	void AddRenderTarget(const sf::Drawable& drawable);

	// 생성할 테트리스 개수를 인자로 받아 창 공간을 최대한 활용하도록 배치하는 메소드
	std::vector<std::reference_wrapper<Tetris>> CreateTetris(int count);
	// 원하는 화면 좌표에 테트리스 한 개를 생성하는 메소드
	Tetris& CreateTetris(int x, int y);

	void ResetAllGames();			// R 키에서 호출하는 메소드 - 모든 게임을 한 번에 초기 상태로 되돌림
	bool IsAllGamesOver() const;	// 모든 게임이 Game Over인지 여부 (모두 종료되면 한 시뮬레이션이 끝난 것)

	// 숫자 키(1~9)로 전체 게임 배속을 변경 (0은 1배속으로 복귀)
	void SetSpeedMultiplier(int multiplier) { speedMultiplier = std::clamp(multiplier, 1, 9); }
	int GetSpeedMultiplier() const { return speedMultiplier; }

	Tetris& GetTetris() { return *games.front(); }
	Tetris& GetTetris(std::size_t index) { return *games.at(index); }
	std::size_t GetGameCount() const { return games.size(); }

	static const sf::Font& GetFont();	// 글꼴 객체를 반환하는 정적 메소드 (한 번만 로드)	

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void FitGamesToWindow();											// 생성된 모든 게임 보드가 창 안에 들어오도록 카메라(View)를 조정하는 메소드

	sf::RenderWindow window{ sf::VideoMode({width, height}), "Tetris" };
	std::vector<const sf::Drawable*> renderTargets;						// 해당 프레임에 그릴 객체들의 포인터 (객체 수명은 Tetris가 관리)
	std::vector<std::unique_ptr<Tetris>> games;							// 창에서 관리하는 테트리스 게임들
	Keyboard keyboard;													// 키보드 입력 시스템 객체

	HWND hWnd = nullptr;
	WNDPROC originalWndProc = nullptr;

	static Window* instance;

	constexpr static int width = 1280;	// 창의 너비
	constexpr static int height = 720;	// 창의 높이

	// 게임 한 개가 화면에서 차지하는 영역 (보드 + 홀드/다음 조각 UI + 점수 텍스트)
	constexpr static int gameAreaPixelWidth = 360;
	constexpr static int gameAreaPixelHeight = 480;

	// 게임 사이 간격 및 창 가장자리 여백
	constexpr static int horizontalGap = 20;
	constexpr static int verticalGap = 20;
	constexpr static int margin = 20;

	int speedMultiplier = 1;			// 숫자 키로 지정한 전체 게임 배속 (1~9)
};
