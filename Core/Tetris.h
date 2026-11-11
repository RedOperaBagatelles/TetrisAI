#pragma once

#include "Core/GameLoop.h"
#include "Core/BoardState.h"
#include "Core/PiecesQueue.h"
#include "Core/AI/Control/PiecePlacementExplorer.h"
#include "SFML/Graphics.hpp"
#include "Utility/Type.h"

#include <deque>
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

	static constexpr map_size height = BoardState::height;
	static constexpr map_size width = BoardState::width;
	static constexpr map_size topMarginBlock = BoardState::topMargin;
	static constexpr map_size maxHeight = BoardState::maxHeight;

	void UpdateScoreText();
	void UpdatePieceUI();
	void Ready(int x, int y);
	bool CreatePiece();
	void HoldPiece();
	void RemoveLine();
	void RecordPiecePlaced() { boardState.RecordPiecePlaced(); }
	void AddRenderPiece(const std::shared_ptr<const sf::Drawable>& piece);					// 렌더링할 조각을 추가하는 메소드
	void AddRenderPieces(const std::vector<std::shared_ptr<const sf::Drawable>>& pieces);	// 렌더링할 조각들을 추가하는 메소드

	map_size(*GetBoard())[width] { return boardState.GetBoard(); }		// 게임 보드의 2차원 배열을 반환하는 메소드
	const BoardState& GetBoardState() const { return boardState; }
	map_size GetStartX() const { return startX; }		// 게임 보드의 시작 X 좌표를 반환하는 메소드
	map_size GetStartY() const { return startY; }		// 게임 보드의 시작 Y 좌표를 반환하는 메소드

	std::shared_ptr<Piece> GetCurrentPiece() const;
	PieceType GetHoldPieceType() const;						// 현재 홀드 중인 조각의 종류를 반환하는 메소드
	const std::deque<PieceType>& GetNextPieces() const;		// 대기 중인 모든 조각의 종류를 반환하는 메소드
	const bool HasUsedHold() const { return hasUsedHold; }	// 해당 턴에 홀드를 사용했는지 여부를 반환하는 메소드

	const std::vector<PiecePlacement>& GetPossiblePlacements() const { return possiblePlacements; }	// 현재 조각으로 둘 수 있는 모든 경우의 수를 반환하는 메소드

	// 현재 보드 상태와 현재 조각의 가능한 배치를 서버로 전송하는 메소드
	void SendStateAndPlacements();

private:
	std::vector<std::shared_ptr<const sf::Drawable>> currentRenderPieces;
	sf::Font font;
	std::shared_ptr<sf::Text> scoreText;
	std::shared_ptr<class Piece> currentPiece;					// 현재 조각 객체

	BoardState boardState;										// 현재 게임 보드 상태를 관리하는 객체
	Window& window;												// 테트리스 게임이 속한 창 객체에 대한 참조
	PiecesQueue piecesQueue;									// 다음에 나올 조각들의 종류를 관리하는 객체

	map_size startX = 0;
	map_size startY = 0;

	PieceType holdPieceType = PieceType::None;		// 현재 hold한 조각의 종류
	bool hasUsedHold = false;						// 해당 턴에 홀드를 사용했는지 여부 (홀드는 한 턴에 한 번만 사용할 수 있음)

	std::vector<PiecePlacement> possiblePlacements;	// 현재 조각으로 둘 수 있는 모든 경우의 수

	bool CreatePieceWithType(PieceType type);

	// 현재 생성된 조각의 상태를 보고 둘 수 있는 모든 경우의 수를 갱신하는 메소드
	void UpdatePossiblePlacements();
};
