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

#include <memory>

void Tetris::Ready(int x, int y)
{
	startX = x;
	startY = y;

	currentRenderPieces.clear();

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

void Tetris::Update(Window& window)
{
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

			Pieces pieces;
            currentRenderPieces.push_back(pieces.GetRenderOnePices(j, height - i - 2, static_cast<PieceType>(board[i][j]), startX, startY));
		}
	}

	window.AddRenderTargets(currentRenderPieces);
}

void Tetris::CreatePiece()
{
	PieceType currentPieceType = piecesQueue.GetPiece();	// 현재 조각의 종류 가져오기

	switch (currentPieceType)
	{
		case PieceType::I: currentPiece = std::make_unique<IPiece>(*this); break;
		case PieceType::J: currentPiece = std::make_unique<JPiece>(*this); break;
		case PieceType::L: currentPiece = std::make_unique<LPiece>(*this); break;
		case PieceType::O: currentPiece = std::make_unique<OPiece>(*this); break;
		case PieceType::S: currentPiece = std::make_unique<SPiece>(*this); break;
		case PieceType::T: currentPiece = std::make_unique<TPiece>(*this); break;
		case PieceType::Z: currentPiece = std::make_unique<ZPiece>(*this); break;
		default: break;
	}
}