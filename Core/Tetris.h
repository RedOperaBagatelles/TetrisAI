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
	Tetris(Window& window, int x = 50, int y = 50);

	void Initialize() override;
	void Update(float deltaTime) override;

	static constexpr map_size height = BoardState::height;
	static constexpr map_size width = BoardState::width;
	static constexpr map_size topMarginBlock = BoardState::topMargin;
	static constexpr map_size maxHeight = BoardState::maxHeight;

	void UpdateScoreText();
	void UpdatePieceUI();
	void Ready(int x, int y);
	bool CreatePiece(bool isInitialPlacement = false);		// isInitialPlacement : 모든 보드가 동일한 시작 상태일 때 첫 보드의 BFS 결과를 재사용
	void HoldPiece();
	void RemoveLine();
	void RecordPiecePlaced() { boardState.RecordPiecePlaced(); }

	void AddRenderRect(float x, float y, float width, float height, const sf::Color& color);	// 고스트 블록 등을 정점 버퍼에 직접 추가하는 메소드 (매 프레임 객체 생성 없음)
	void SetDetailsVisible(bool visible) { showDetails = visible; }								// 보드가 화면에서 충분히 크지 않으면 HUD/점수 텍스트를 생략하는데 사용

	map_size(*GetBoard())[width] { return boardState.GetBoard(); }		// 게임 보드의 2차원 배열을 반환하는 메소드
	const BoardState& GetBoardState() const { return boardState; }
	int GetStartX() const { return startPixelX; }			// 게임 보드의 시작 X 좌표(픽셀)를 반환하는 메소드
	int GetStartY() const { return startPixelY; }			// 게임 보드의 시작 Y 좌표(픽셀)를 반환하는 메소드

	std::shared_ptr<Piece> GetCurrentPiece() const;
	PieceType GetHoldPieceType() const;						// 현재 홀드 중인 조각의 종류를 반환하는 메소드
	const std::deque<PieceType>& GetNextPieces() const;		// 대기 중인 모든 조각의 종류를 반환하는 메소드
	const bool HasUsedHold() const { return hasUsedHold; }	// 해당 턴에 홀드를 사용했는지 여부를 반환하는 메소드

	const std::vector<PiecePlacement>& GetPossiblePlacements() const { return possiblePlacements; }	// 현재 조각으로 둘 수 있는 모든 경우의 수를 반환하는 메소드

	// 현재 보드 상태와 현재 조각의 가능한 배치를 서버로 전송하는 메소드
	void SendStateAndPlacements();

private:
	bool CreatePieceWithType(PieceType type);
	void UpdatePossiblePlacements();				// 현재 생성된 조각의 상태를 보고 둘 수 있는 모든 경우의 수를 갱신하는 메소드
	void RebuildWallVertices();						// 벽은 위치가 고정되어 있으므로 한 번만 정점 버퍼를 만든다

	static void CreateBlock(sf::VertexArray& vertices, float x, float y, float width, float height, const sf::Color& color);		// 픽셀 좌표를 기준으로 사각형을 정점 버퍼에 추가
	static void CreateBlock(sf::VertexArray& vertices, int boardX, int boardY, const sf::Color& color, int offsetX, int offsetY);	// 보드 좌표를 기준으로 사각형을 정점 버퍼에 추가

	static std::vector<PiecePlacement> initialPlace;
	static bool hasInitialPlace;

	// 재사용하는 정점 버퍼 (매 프레임 clear() 후 다시 채움, 할당 없음)
	std::shared_ptr<sf::VertexArray> wallVertices;
	std::shared_ptr<sf::VertexArray> renderVertices;
	bool showDetails = true;						// 보드가 작으면 HUD/텍스트를 그리지 않음

	std::shared_ptr<sf::Text> scoreText;
	std::shared_ptr<class Piece> currentPiece;		// 현재 조각 객체

	BoardState boardState;							// 현재 게임 보드 상태를 관리하는 객체
	Window& window;									// 테트리스 게임이 속한 창 객체에 대한 참조
	PiecesQueue piecesQueue;						// 다음에 나올 조각들의 종류를 관리하는 객체

	int startPixelX = 0;							// 보드의 화면상 시작 X 좌표 (픽셀 단위이므로 map_size를 사용하지 않음)
	int startPixelY = 0;							// 보드의 화면상 시작 Y 좌표 (픽셀 단위이므로 map_size를 사용하지 않음)

	PieceType holdPieceType = PieceType::None;		// 현재 hold한 조각의 종류
	bool hasUsedHold = false;						// 해당 턴에 홀드를 사용했는지 여부 (홀드는 한 턴에 한 번만 사용할 수 있음)

	std::vector<PiecePlacement> possiblePlacements;	// 현재 조각으로 둘 수 있는 모든 경우의 수
};
