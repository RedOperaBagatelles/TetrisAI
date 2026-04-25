#pragma once

#include "Core/GameLoop.h"
#include "Utility/Type.h"

#include <vector>

class Tetris;

enum class PieceType { None, I, J, L, O, S, T, Z };				// 테트리스 조각의 종류를 나타내는 열거형
enum class PieceRotationType { Spawn, Right, Reverse, Left };	// 테트리스 조각의 회전 상태를 나타내는 열거형
enum class MoveDirection { None, Left, Right, Down };           // 조각의 이동 방향을 나타내는 열거형

#define PIECE_DRAW(pieceType) \
    auto map = tetris.GetBoard(); \
    for (int i = 0; i < 4; i++) \
    { \
        for (int j = 0; j < 4; j++) \
            map[Tetris::maxHeight - (currentY + i + 1)][currentX + j] = rotateShape[currentRotation][i][j] * (low_uint)pieceType; /* 조각의 블록을 게임 보드에 배치 */ \
    } \
\
    /* 이전에 있는 조각의 블록을 게임 보드에서 제거 */ \
    while (!removeBlockTarget.empty()) \
    { \
        auto& position = removeBlockTarget.back(); \
        map[Tetris::maxHeight - (position.y + 1)][position.x] = 0; \
        removeBlockTarget.pop_back(); \
    }

#define PIECE_MOVE(beforePosition) \
    const int deltaX = static_cast<int>(currentX) - static_cast<int>(beforePosition.x); \
    const int deltaY = static_cast<int>(currentY) - static_cast<int>(beforePosition.y); \
\
    for (int i = 0; i < 4; i++) \
    { \
        for (int j = 0; j < 4; j++) \
        { \
            if (rotateShape[currentRotation][i][j] == 0) \
                continue; \
\
            const int beforeX = static_cast<int>(beforePosition.x) + j; \
            const int beforeY = static_cast<int>(beforePosition.y) + i; \
\
            if (beforeX < 0 || beforeX >= Tetris::width || beforeY < 0 || beforeY >= Tetris::maxHeight) \
                continue; \
\
            const int overlapLocalX = j - deltaX; \
            const int overlapLocalY = i - deltaY; \
\
            const bool isOverlappedByCurrent = \
                overlapLocalX >= 0 && overlapLocalX < 4 && \
                overlapLocalY >= 0 && overlapLocalY < 4 && \
                rotateShape[currentRotation][overlapLocalY][overlapLocalX] != 0; \
\
            if (!isOverlappedByCurrent) \
                removeBlockTarget.emplace_back(beforeX, beforeY); /* 조각이 이동하거나 회전할 때 제거해야 하는 블록들의 위치를 저장 */ \
        } \
    } \
\

class Piece : public GameLoop
{
public:
	Piece(map_size x, map_size y, Tetris& tetris);
	virtual ~Piece() = default;

    void Update(float deltaTime) override;

    bool Rotate(PieceType pieceType, low_uint rotateShape[4][4][4], bool isClockwise = true); // 조각을 회전시키는 순수 가상 함수

    bool IsCollision( const map_size rotateShape[4][4], map_size x, map_size y) const;

protected:
	static int NormalizeRotation(low_uint value);

    virtual void Draw() = 0;	// 조각을 그리는 순수 가상 함수

	virtual void Move(Position beforePosition) = 0;	// 조각을 이동시키는 순수 가상 함수
    void Move(MoveDirection direction);

    static constexpr Position iKick[4][4][5] =
    {
        // from 0
        {
            {{0,0},{-2,0},{1,0},{-2,-1},{1,2}}, // 0->R
            {}, {}, {}
        },

        // from R
        {
            {}, {{0,0},{-1,0},{2,0},{-1,2},{2,-1}}, // R->2
            {}, {}
        },

        // from 2
        {
            {}, {}, {{0,0},{2,0},{-1,0},{2,1},{-1,-2}}, // 2->L
            {}
        },

        // from L
        {
            {{0,0},{1,0},{-2,0},{1,-2},{-2,1}}, // L->0
            {}, {}, {}
        }
    };

    static constexpr Position normalKick[4][4][5] =
    {
        // from 0
        {
            {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}}, // 0->R
            {}, {}, {}
        },

        // from R
        {
            {}, {{0,0},{1,0},{1,-1},{0,2},{1,2}}, // R->2
            {}, {}
        },

        // from 2
        {
            {}, {}, {{0,0},{1,0},{1,1},{0,-2},{1,-2}}, // 2->L
            {}
        },
        // from L
        {
            {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}}, // L->0
            {}, {}, {}
        }
	};

    static constexpr low_uint rotationCount = 4;

	std::vector<Position> removeBlockTarget;	// 조각이 이동하거나 회전할 때 제거해야 하는 블록들의 위치를 저장하는 벡터
    Tetris& tetris;	                            // 조각이 속한 테트리스 게임 객체에 대한 참조
    map_size currentX;
    map_size currentY;
    low_uint currentRotation = 0;

	static constexpr float dropInterval = 0.8f;	// 조각이 자동으로 떨어지는 간격 (초 단위)
	float dropRemainTime = 0.0f;                // 다음 자동 낙하까지 남은 시간 (초 단위)
};