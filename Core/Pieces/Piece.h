#pragma once

#include "Core/GameLoop.h"
#include "Utility/Type.h"

#include <vector>

class Tetris;

enum class PieceType { None, I, J, L, O, S, T, Z };				// 테트리스 조각의 종류를 나타내는 열거형
enum class PieceRotationType { Spawn, Right, Reverse, Left };	// 테트리스 조각의 회전 상태를 나타내는 열거형
enum class MoveDirection { None, Left, Right, Down };           // 조각의 이동 방향을 나타내는 열거형

class Piece : public GameLoop
{
public:
	Piece(map_size x, map_size y, Tetris& tetris);
	virtual ~Piece() = default;

    void Update(float deltaTime) override;

    bool Rotate(PieceType pieceType, low_uint rotateShape[4][4][4], bool isClockwise = true); // 조각을 회전시키는 순수 가상 메소드

    bool IsCollision(const low_uint toRotateShape[4][4], map_size x, map_size y, bool isBeforePlace = true) const;
    void Move(MoveDirection direction); // 조각을 이동시키는 메소드
    void Place();
    Position GetPosition() const { return current; }

    virtual const low_uint(&GetRotateShape() const)[4][4][4] = 0;	// 조각의 회전 형태를 반환하는 순수 가상 메소드

protected:
	static int NormalizeRotation(low_uint value);

	virtual const PieceType GetPieceType() const = 0;               // 조각의 종류를 반환하는 순수 가상 메소드

	void Draw();                        // 조각을 그리는 순수 가상 메소드

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
    Position current;
    low_uint currentRotation = 0;

	static constexpr float dropInterval = 0.8f;	// 조각이 자동으로 떨어지는 간격 (초 단위)
	float dropRemainTime = 0.0f;                // 다음 자동 낙하까지 남은 시간 (초 단위)
};