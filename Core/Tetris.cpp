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

#include <SFML/Graphics/Text.hpp>

#include <deque>
#include <memory>

Tetris::Tetris(Window& window) : window(window)
{

}

void Tetris::Initialize()
{
	font.openFromFile("C:/Windows/Fonts/CascadiaCode.ttf");
	scoreText = std::make_shared<sf::Text>(font);
	scoreText->setCharacterSize(16);
	scoreText->setFillColor(sf::Color::White);
	scoreText->setPosition({ (float)startX + 300, (float)startY + 445 });
	UpdateScoreText();

	Ready(50, 50);
}

void Tetris::Update(float deltaTime)
{
	if (currentPiece != nullptr)
		currentPiece->Update(deltaTime);

	currentRenderPieces.clear();
	const auto walls = Pieces::GetRenderWalls(startX, startY);
	currentRenderPieces.insert(currentRenderPieces.end(), walls.begin(), walls.end());

	// 맵에 있는 모든 조각들의 렌더링 객체 생성
	for (int i = 0; i < maxHeight; i++)
	{
		for (int j = 0; j < width; j++)
		{
			// 해당 위치에 조각이 없으면 건너뜀
			if (board[i][j] == 0)
				continue;

			currentRenderPieces.push_back(Pieces::GetRenderOnePices(j, height - i - 2, static_cast<PieceType>(board[i][j]), startX, startY));
		}
	}

	currentPiece->ShowGhost();
	UpdateScoreText();
	UpdatePieceUI();
	window.AddRenderTargets(currentRenderPieces);
}

void Tetris::Ready(int x, int y)
{
	startX = x;
	startY = y;

	currentRenderPieces.clear();

	// 게임 보드 초기화
	for (int i = 0; i < maxHeight; i++)
	{
		for (int j = 0; j < width; j++)
			board[i][j] = 0;
	}

	const auto walls = Pieces::GetRenderWalls(x, y);
	currentRenderPieces.insert(currentRenderPieces.end(), walls.begin(), walls.end());

	// 테스트용 맵 데이터 설정
	board[0][0] = 1;	// I
	board[0][1] = 2;	// J
	board[0][2] = 3;	// L
	board[0][3] = 4;	// O
	board[0][4] = 5;	// S
	board[0][5] = 6;	// T
	board[0][6] = 7;	// Z

	CreatePiece();
}

void Tetris::UpdateScoreText()
{
	if (scoreText == nullptr)
		return;

	scoreText->setString("Score : " + std::to_string(score));

	currentRenderPieces.push_back(scoreText);
}

void Tetris::UpdatePieceUI()
{
	sf::RectangleShape holdingPieceUIBackground(sf::Vector2f(85, 85));
	holdingPieceUIBackground.setFillColor(sf::Color(50, 50, 50));
	holdingPieceUIBackground.setPosition(sf::Vector2f(startX + 250, startY));

	// 다음 조각 UI 배경 설정
	sf::RectangleShape nextPieceUIBackground(sf::Vector2f(85, 275));
	nextPieceUIBackground.setFillColor(sf::Color(50, 50, 50));
	nextPieceUIBackground.setPosition(sf::Vector2f(startX + 250, startY + 110));

	currentRenderPieces.push_back(std::make_shared<sf::RectangleShape>(holdingPieceUIBackground));
	currentRenderPieces.push_back(std::make_shared<sf::RectangleShape>(nextPieceUIBackground));
	
	// 다음 조각 큐에서 다음 조각들의 종류를 가져옴
	const std::deque<PieceType>& nextPieces = piecesQueue.GetNextPieces();

	// 다음 조각 UI에 조각들을 렌더링하기 위한 초기 Y 오프셋 설정 (10px)
	float currentYOffset = 10.0f;
	
	// 다음 조각 큐에서 최대 5개의 조각을 렌더링 (UI 공간에 맞게)
	for (size_t i = 0; i < nextPieces.size() && i < 5; ++i)
	{
		PieceType pieceType = nextPieces[i];
		std::shared_ptr<Piece> piece;

		// 다음 조각의 종류에 따라 해당 조각 객체를 생성
		switch (pieceType)
		{
			case PieceType::I: piece = std::make_shared<IPiece>(*this); break;
			case PieceType::J: piece = std::make_shared<JPiece>(*this); break;
			case PieceType::L: piece = std::make_shared<LPiece>(*this); break;
			case PieceType::O: piece = std::make_shared<OPiece>(*this); break;
			case PieceType::S: piece = std::make_shared<SPiece>(*this); break;
			case PieceType::T: piece = std::make_shared<TPiece>(*this); break;
			case PieceType::Z: piece = std::make_shared<ZPiece>(*this); break;
			default: continue;
		}

		sf::Color pieceColor = Pieces::colors[static_cast<int>(pieceType)]; // 조각 종류에 따른 색상 가져오기
		const auto& rotationShape = piece->GetRotateShape()[0];

		int minRow = 4, maxRow = -1;

		// 다음 조각의 회전 형태에서 상단과 하단의 빈 행을 제외하기 위해 최소 행과 최대 행을 계산
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				if (rotationShape[row][col] == 0)
					continue;

				if (row < minRow) 
					minRow = row;

				if (row > maxRow)
					maxRow = row;
			}
		}

		// 만약 모든 행이 빈 행이라면 (즉, 조각이 없는 경우) 렌더링을 건너뜀
		if (maxRow == -1)
			continue;

		// 다음 조각의 회전 형태에서 유효한 블록들만 렌더링 (빈 행 제외)
		for (int row = minRow; row <= maxRow; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				if (rotationShape[row][col] == 0)
					continue;

				// 유효한 블록을 렌더링하기 위한 사각형 객체 생성
				sf::RectangleShape block(sf::Vector2f(15, 15));
				block.setFillColor(pieceColor);

				block.setPosition(sf::Vector2f(
					nextPieceUIBackground.getPosition().x + 10 + col * 17,
					nextPieceUIBackground.getPosition().y + currentYOffset + (row - minRow) * 17)
				);

				currentRenderPieces.push_back(std::make_shared<sf::RectangleShape>(block));
			}
		}

		// 다음 조각의 회전 형태에서 유효한 블록들의 높이에 따라 Y 오프셋을 증가시켜 다음 조각이 UI에 겹치지 않도록 함
		int pieceHeight = (maxRow - minRow + 1);
		currentYOffset += (pieceHeight + 1) * 17.0f;
	}
}

bool Tetris::CreatePiece()
{
	PieceType currentPieceType = piecesQueue.GetPiece();	// 현재 조각의 종류 가져오기

	switch (currentPieceType)
	{
		case PieceType::I: currentPiece = std::make_shared<IPiece>(*this); break;
		case PieceType::J: currentPiece = std::make_shared<JPiece>(*this); break;
		case PieceType::L: currentPiece = std::make_shared<LPiece>(*this); break;
		case PieceType::O: currentPiece = std::make_shared<OPiece>(*this); break;
		case PieceType::S: currentPiece = std::make_shared<SPiece>(*this); break;
		case PieceType::T: currentPiece = std::make_shared<TPiece>(*this); break;
		case PieceType::Z: currentPiece = std::make_shared<ZPiece>(*this); break;
		default: break;
	}

	// 현재 조각이 배치할 수 있는지 확인
	auto& rotationShape = currentPiece->GetRotateShape();
	Position position = currentPiece->GetPosition();

	if (currentPiece->IsCollision(rotationShape[0], position.x, position.y, false))
		return false;

	currentPiece->Initialize();	// 현재 조각 초기화

	return true;
}

void Tetris::RemoveLine()
{
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

		score += linesRemoved;	// 점수 증가
		UpdateScoreText();
	}
}

void Tetris::AddRenderPiece(const std::shared_ptr<const sf::Drawable>& piece)
{
	currentRenderPieces.push_back(piece);
}

void Tetris::AddRenderPieces(const std::vector<std::shared_ptr<const sf::Drawable>>& pieces)
{
	currentRenderPieces.insert(currentRenderPieces.end(), pieces.begin(), pieces.end());
}
