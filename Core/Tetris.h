#pragma once

#include "Core/GameLoop.h"
#include "Core/PiecesQueue.h"
#include "SFML/Graphics.hpp"
#include "Utility/Type.h"

#include <vector>
#include <memory>

class Window;
class Piece;

class Tetris : public GameLoop
{
public:
	Tetris(Window& window);

	void Initialize() override;
	void Update(float deltaTime) override;

	static constexpr map_size height = 20;
	static constexpr map_size width = 10;
	static constexpr map_size topMarginBlock = 2;						// 맵의 위쪽에 보이지 않는 블록 영역 (회전 시 충돌 체크용)
	static constexpr map_size maxHeight = height + topMarginBlock + 1;	// 게임 보드의 최대 높이 (보이지 않는 블록 영역 포함)

	void UpdateScoreText();
	void UpdatePieceUI();
	void Ready(int x, int y);
	bool CreatePiece();
	void RemoveLine();
	void AddRenderPiece(const std::shared_ptr<const sf::Drawable>& piece);					// 렌더링할 조각을 추가하는 메소드
	void AddRenderPieces(const std::vector<std::shared_ptr<const sf::Drawable>>& pieces);	// 렌더링할 조각들을 추가하는 메소드

	map_size(*GetBoard())[width] { return board; }		// 게임 보드의 2차원 배열을 반환하는 메소드
	map_size GetStartX() const { return startX; }		// 게임 보드의 시작 X 좌표를 반환하는 메소드
	map_size GetStartY() const { return startY; }		// 게임 보드의 시작 Y 좌표를 반환하는 메소드

	std::shared_ptr<Piece> GetCurrentPiece() const { return currentPiece; }

private:
	std::vector<std::shared_ptr<const sf::Drawable>> currentRenderPieces;
	sf::Font font;
	std::shared_ptr<sf::Text> scoreText;
	std::shared_ptr<class Piece> currentPiece;					// 현재 조각 객체

	map_size board[maxHeight][width] = { 0 };
	Window& window;												// 테트리스 게임이 속한 창 객체에 대한 참조
	PiecesQueue piecesQueue;									// 다음에 나올 조각들의 종류를 관리하는 객체

	map_size startX = 0;
	map_size startY = 0;

	int score = 0;	// 현재 점수
};