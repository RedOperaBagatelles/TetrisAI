#include "Tetris.h"
#include "Pieces.h"
#include "Window.h"

#include "Pieces/IPiece.h"
#include "Pieces/JPiece.h"
#include "Pieces/OPiece.h"
#include "Pieces/LPiece.h"
#include "Pieces/SPiece.h"
#include "Pieces/TPiece.h"
#include "Pieces/ZPiece.h"

#include "Pieces/Piece.h"

#include "AI/ConnectAI/GameStateSender.h"
#include "AI/ConnectAI/PiecePlacementSender.h"

#include <SFML/Graphics/Text.hpp>

#include <algorithm>
#include <deque>
#include <memory>
#include <stdexcept>

std::vector<PiecePlacement> Tetris::initialPlace;	// 모든 보드는 Ready() 직후 상태가 동일하므로 첫 보드의 BFS 결과를 재사용한다.
bool Tetris::hasInitialPlace = false;
std::atomic<GameCapacity> Tetris::nextGameID{ 1 };

void Tetris::CreateBlock(sf::VertexArray& vertices, float x, float y, float width, float height, const sf::Color& color)
{
	const float right = x + width;
	const float bottom = y + height;

	vertices.append(sf::Vertex{ { x, y }, color });
	vertices.append(sf::Vertex{ { x, bottom }, color });
	vertices.append(sf::Vertex{ { right, bottom }, color });
	vertices.append(sf::Vertex{ { x, y }, color });
	vertices.append(sf::Vertex{ { right, bottom }, color });
	vertices.append(sf::Vertex{ { right, y }, color });
}

void Tetris::CreateBlock(sf::VertexArray& vertices, int boardX, int boardY, const sf::Color& color, int offsetX, int offsetY)
{
	constexpr int size = Pieces::blockSize - Pieces::blockPadding;
	float x = static_cast<float>(offsetX + (boardX + 1) * Pieces::blockSize);
	float y = static_cast<float>(offsetY + (boardY + 1) * Pieces::blockSize);

	CreateBlock(vertices, x, y, static_cast<float>(size), static_cast<float>(size), color);
}

Tetris::Tetris(Window& window, int x, int y)
	: window(window), startPixelX(x), startPixelY(y), gameID(AllocateGameID())
{
	// clear()는 메모리를 해제하지 않으므로 미리 충분한 크기를 확보해 둔다.
	renderVertices = std::make_shared<sf::VertexArray>(sf::PrimitiveType::Triangles);
	renderVertices->resize(1024);
	renderVertices->clear();

	wallVertices = std::make_shared<sf::VertexArray>(sf::PrimitiveType::Triangles);
	wallVertices->resize(512);
	wallVertices->clear();
}

void Tetris::Initialize()
{
	// 폰트 로드는 GetFont() 안에서 딱 한 번만 수행된다.
	scoreText = std::make_shared<sf::Text>(Window::GetFont());
	scoreText->setCharacterSize(16);
	scoreText->setFillColor(sf::Color::White);

	gameOverText = std::make_shared<sf::Text>(Window::GetFont());
	gameOverText->setString("GAME OVER");
	gameOverText->setCharacterSize(28);
	gameOverText->setFillColor(sf::Color(255, 80, 80));

	Ready(startPixelX, startPixelY);
	UpdateScoreText();
}

void Tetris::Reset()
{
	// 진행 중이던 조각을 버리고 초기 상태에서 다시 시작 (R 키로 모든 게임을 한꺼번에 리셋할 때 사용)
	currentPiece.reset();
	possiblePlacements.clear();

	Initialize();
}

void Tetris::GameOver()
{
	if (isGameOver)
		return;

	isGameOver = true;
	possiblePlacements.clear();
}

void Tetris::Update(float deltaTime)
{
	if (!isGameOver && currentPiece != nullptr)
		currentPiece->Update(deltaTime);

	// 객체를 새로 만들지 않고 재사용하는 정점 버퍼만 비운다.
	renderVertices->clear();

	const auto board = boardState.GetBoard();

	// 맵에 있는 조각들을 정점 버퍼에 직접 기록
	for (int i = 0; i < maxHeight; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (board[i][j] == 0)
				continue;

			CreateBlock(*renderVertices, j, height - i - 2, Pieces::colors[static_cast<int>(board[i][j])], startPixelX, startPixelY);
		}
	}

	// Game Over가 된 게임은 더 이상 조각을 움직이지 않고 마지막 보드 위에 반투명 덮개와 문구를 그린다.
	if (isGameOver)
	{
		// 블록들을 먼저 채운 뒤 그 위에 검은색 반투명 사각형을 덧그린다. (정점 버퍼는 추가 순서대로 그려짐)
		AddRenderRect(static_cast<float>(startPixelX + Pieces::blockSize),
		              static_cast<float>(startPixelY + Pieces::blockSize),
		              static_cast<float>(width * Pieces::blockSize - Pieces::blockPadding),
		              static_cast<float>((height - 1) * Pieces::blockSize - Pieces::blockPadding),
		              sf::Color(0, 0, 0, 170));

		window.AddRenderTarget(*wallVertices);
		window.AddRenderTarget(*renderVertices);

		if (gameOverText != nullptr)
			window.AddRenderTarget(*gameOverText);	// 덮개 위에 GAME OVER 문구

		return;
	}

	if (currentPiece != nullptr)
		currentPiece->ShowGhost();

	// 보드가 작으면 읽을 수 없는 HUD/텍스트는 아예 만들지 않는다.
	if (showDetails)
	{
		UpdateScoreText();
		UpdatePieceUI();
	}

	// 벽(고정), 블록/고스트/HUD(매 프레임), 점수 텍스트 → 보드당 2~3 draw call
	window.AddRenderTarget(*wallVertices);
	window.AddRenderTarget(*renderVertices);
	if (showDetails && scoreText != nullptr)
		window.AddRenderTarget(*scoreText);
}

void Tetris::Ready(int x, int y)
{
	startPixelX = x;
	startPixelY = y;

	isGameOver = false;
	currentPiece.reset();
	holdPieceType = PieceType::None;
	hasUsedHold = false;
	piecesQueue.Reset();
	boardState.Reset();

	if (scoreText != nullptr)
		scoreText->setPosition({ (float)startPixelX + 250, (float)startPixelY + 395 });

	if (gameOverText != nullptr)
	{
		const sf::FloatRect textBounds = gameOverText->getLocalBounds();
		gameOverText->setOrigin({ textBounds.size.x / 2.0f, textBounds.size.y / 2.0f });
		gameOverText->setPosition({ static_cast<float>(startPixelX + (width / 2 + 1) * Pieces::blockSize),
		                            static_cast<float>(startPixelY + (height + 1) * Pieces::blockSize / 2) });
	}

	// 벽은 위치가 바뀔 때만 다시 만든다.
	RebuildWallVertices();
	renderVertices->clear();

	const auto board = boardState.GetBoard();

	// 테스트용 맵 데이터 설정
	board[0][0] = 1;	// I
	board[0][1] = 2;	// J
	board[0][2] = 3;	// L
	board[0][3] = 4;	// O
	board[0][4] = 5;	// S
	board[0][5] = 6;	// T
	board[0][6] = 7;	// Z
	boardState.RecalculateBoardHeight();

	CreatePiece(true);
}

void Tetris::UpdateScoreText()
{
	if (scoreText == nullptr)
		return;

	scoreText->setString("Score : " + std::to_string(boardState.GetScore()) +
		"\nLines : " + std::to_string(boardState.GetLineClearCount()) +
		"\nPieces : " + std::to_string(boardState.GetPiecesPlaceCount()) +
		"\nHeight : " + std::to_string(boardState.GetBoardHeight()));
}

void Tetris::UpdatePieceUI()
{
	// 조각 미리보기는 객체를 만들지 않고 정적 형태 데이터만 읽어 그린다.
	const auto getSpawnShape = [](PieceType type) -> const low_uint (*)[4][4]
	{
		switch (type)
		{
			case PieceType::I: return &IPiece::GetStaticShape()[0];
			case PieceType::J: return &JPiece::GetStaticShape()[0];
			case PieceType::L: return &LPiece::GetStaticShape()[0];
			case PieceType::O: return &OPiece::GetStaticShape()[0];
			case PieceType::S: return &SPiece::GetStaticShape()[0];
			case PieceType::T: return &TPiece::GetStaticShape()[0];
			case PieceType::Z: return &ZPiece::GetStaticShape()[0];
			default: return nullptr;
		}
	};

	const float holdX = static_cast<float>(startPixelX + 250);
	const float holdY = static_cast<float>(startPixelY);
	const float nextX = holdX;
	const float nextY = static_cast<float>(startPixelY + 110);

	CreateBlock(*renderVertices, holdX, holdY, 85, 85, sf::Color(50, 50, 50));
	CreateBlock(*renderVertices, nextX, nextY, 85, 275, sf::Color(50, 50, 50));

	// 홀드 중인 조각 렌더링
	if (const auto holdShape = getSpawnShape(holdPieceType); holdShape != nullptr)
	{
		sf::Color holdColor = Pieces::colors[static_cast<int>(holdPieceType)];

		if (hasUsedHold)
			holdColor = sf::Color(holdColor.r / 2, holdColor.g / 2, holdColor.b / 2);

		int minRow = 4, maxRow = -1;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				if ((*holdShape)[row][col] == 0)
					continue;

				minRow = std::min(minRow, row);
				maxRow = std::max(maxRow, row);
			}
		}

		if (maxRow != -1)
		{
			for (int row = minRow; row <= maxRow; row++)
			{
				for (int col = 0; col < 4; col++)
				{
					if ((*holdShape)[row][col] == 0)
						continue;

					CreateBlock(*renderVertices, holdX + 10 + col * 17, holdY + 10 + (row - minRow) * 17, 15, 15, holdColor);
				}
			}
		}
	}

	// 다음 조각 큐 렌더링
	const std::deque<PieceType>& nextPieces = piecesQueue.GetNextPieces();
	float currentYOffset = 10.0f;

	for (size_t i = 0; i < nextPieces.size() && i < 5; ++i)
	{
		const auto shape = getSpawnShape(nextPieces[i]);
		if (shape == nullptr)
			continue;

		int minRow = 4, maxRow = -1;
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				if ((*shape)[row][col] == 0)
					continue;

				minRow = std::min(minRow, row);
				maxRow = std::max(maxRow, row);
			}
		}

		if (maxRow == -1)
			continue;

		const sf::Color pieceColor = Pieces::colors[static_cast<int>(nextPieces[i])];
		for (int row = minRow; row <= maxRow; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				if ((*shape)[row][col] == 0)
					continue;

				CreateBlock(*renderVertices,
					nextX + 10 + col * 17,
					nextY + currentYOffset + (row - minRow) * 17,
					15, 15, pieceColor);
			}
		}

		currentYOffset += (maxRow - minRow + 2) * 17.0f;
	}
}

void Tetris::RebuildWallVertices()
{
	wallVertices->clear();

	constexpr int size = Pieces::blockSize - Pieces::blockPadding;

	// 왼쪽 벽
	for (int i = 0; i <= height; i++)
		CreateBlock(*wallVertices, (float)startPixelX, (float)(startPixelY + i * Pieces::blockSize), (float)size, (float)size, Pieces::wallColor);

	// 오른쪽 벽
	const int rightWallX = startPixelX + (width + 1) * Pieces::blockSize;
	for (int i = 0; i <= height; i++)
		CreateBlock(*wallVertices, (float)rightWallX, (float)(startPixelY + i * Pieces::blockSize), (float)size, (float)size, Pieces::wallColor);

	// 바닥 벽
	const int bottomWallY = startPixelY + height * Pieces::blockSize;
	for (int i = 1; i <= width; i++)
		CreateBlock(*wallVertices, (float)(startPixelX + i * Pieces::blockSize), (float)bottomWallY, (float)size, (float)size, Pieces::wallColor);
}

GameCapacity Tetris::AllocateGameID()
{
	// nextGameID로부터 
	GameCapacity current = nextGameID.load();

	while (current != invalidGameID)
	{
		// 65535에서 1을 더하면 0(InvalidGameID)이 되므로 그 다음부터는 할당을 막는다.
		const GameCapacity next = static_cast<GameCapacity>(current + 1);

		// nextGameID를 current에서 next로 바꾸는 데 성공하면 current를 반환하고, 실패하면 current를 다시 읽어온다.
		if (nextGameID.compare_exchange_weak(current, next))
			return current;
	}

	throw std::overflow_error("No more gameID available (uint16 supports IDs 1..65535).");
}

bool Tetris::CreatePieceWithType(PieceType type)
{
	switch (type)
	{
		case PieceType::I: currentPiece = std::make_shared<IPiece>(*this); break;
		case PieceType::J: currentPiece = std::make_shared<JPiece>(*this); break;
		case PieceType::L: currentPiece = std::make_shared<LPiece>(*this); break;
		case PieceType::O: currentPiece = std::make_shared<OPiece>(*this); break;
		case PieceType::S: currentPiece = std::make_shared<SPiece>(*this); break;
		case PieceType::T: currentPiece = std::make_shared<TPiece>(*this); break;
		case PieceType::Z: currentPiece = std::make_shared<ZPiece>(*this); break;
		default: return false;
	}

	auto& rotationShape = currentPiece->GetRotateShape();
	Position position = currentPiece->GetPosition();

	if (currentPiece->IsCollision(rotationShape[0], position.x, position.y, false))
		return false;

	currentPiece->Initialize();

	// 조각이 생성된 시점에만 현재 보드 상태를 서버로 전송
	GameStateSender::Send(*this);

	return true;
}

bool Tetris::CreatePiece(bool isInitialPlace)
{
	hasUsedHold = false;

	// 다음 조각 큐에서 다음 조각의 종류를 가져와 해당 조각을 생성
	if (!CreatePieceWithType(piecesQueue.GetPiece()))
		return false;
	
	// 현재 조각이 생성된 시점에만 가능한 배치 목록을 갱신
	if (isInitialPlace && hasInitialPlace)
	{
		possiblePlacements = initialPlace;
		PiecePlacementSender::Send(*this, possiblePlacements);
	}

	else
	{
		UpdatePossiblePlacements();

		if (isInitialPlace)
		{
			initialPlace = possiblePlacements;
			hasInitialPlace = true;
		}
	}

	return true;
}

void Tetris::HoldPiece()
{
	if (hasUsedHold || currentPiece == nullptr)
		return;

	PieceType currentType = currentPiece->GetPieceType();
	currentPiece->RemovePieceFromBoard();

	PieceType toSpawn = holdPieceType;
	holdPieceType = currentType;

	bool success = (toSpawn == PieceType::None) ? CreatePiece() : CreatePieceWithType(toSpawn);

	if (!success)
		GameOver();		// 홀드한 조각도 생성할 수 없으면 게임 오버 상태로 진입

	hasUsedHold = true;

	// 홀드 사용 여부가 갱신된 상태를 반영하기 위해 다시 계산
	UpdatePossiblePlacements();
}

void Tetris::UpdatePossiblePlacements()
{
	// 게임이 종료되었거나 조각이 생성되지 않은 상태라면 경우의 수도 없음
	if (isGameOver)
	{
		possiblePlacements.clear();
		return;
	}

	if (currentPiece == nullptr)
	{
		possiblePlacements.clear();
		return;
	}

	// 현재 조각의 상태를 기반으로 가능한 모든 배치 목록을 계산
	possiblePlacements = PiecePlacementExplorer::GetAllPossiblePlacements(*this);

	// 현재 조각의 상태와 가능한 배치 목록을 서버로 전송
	PiecePlacementSender::Send(*this, possiblePlacements);
}

void Tetris::SendStateAndPlacements()
{
	// Game Over가 된 보드의 상태는 서버에서 더 이상 선택하지 않도록 전송하지 않는다.
	if (isGameOver || currentPiece == nullptr)
		return;

	GameStateSender::Send(*this);
	PiecePlacementSender::Send(*this, possiblePlacements);
}

void Tetris::RemoveLine()
{
	const auto board = boardState.GetBoard();
	int linesRemoved = 0;

	// 아래에서 위로 올라가면서 줄을 검사
	for (int i = 0; i < height; i++)
	{
		bool isFull = true;

		for (int j = 0; j < width; j++)
		{
			if (board[i][j] == 0)
			{
				isFull = false;
				break;
			}
		}

		if (isFull)
			linesRemoved++;

		else if (linesRemoved > 0)
		{
			// 삭제된 줄 수만큼 위의 줄을 아래로 내림
			for (int j = 0; j < width; j++)
				board[i - linesRemoved][j] = board[i][j];
		}
	}

	if (linesRemoved > 0)
	{
		// 상단의 비워진 줄들을 초기화
		for (int i = height - linesRemoved; i < height; i++)
		{
			for (int j = 0; j < width; j++)
				board[i][j] = 0;
		}
	}

	boardState.AddLineClearCount(linesRemoved);
	boardState.RecalculateBoardHeight();
	UpdateScoreText();
}

void Tetris::AddRenderRect(float x, float y, float rectWidth, float rectHeight, const sf::Color& color)
{
	CreateBlock(*renderVertices, x, y, rectWidth, rectHeight, color);
}

std::shared_ptr<Piece> Tetris::GetCurrentPiece() const
{
	return currentPiece;
}

PieceType Tetris::GetHoldPieceType() const
{
	return holdPieceType;
}

const std::deque<PieceType>& Tetris::GetNextPieces() const
{
	return piecesQueue.GetNextPieces();
}
