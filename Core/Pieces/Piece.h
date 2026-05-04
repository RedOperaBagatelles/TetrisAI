#pragma once

#include "Core/GameLoop.h"
#include "Utility/Type.h"

#include <vector>

class Tetris;

enum class PieceType { None, I, J, L, O, S, T, Z, Ghost };		// 테트리스 조각의 종류를 나타내는 열거형
enum class PieceRotationType { Spawn, Right, Reverse, Left };	// 테트리스 조각의 회전 상태를 나타내는 열거형
enum class MoveDirection { None, Left, Right, Down };           // 조각의 이동 방향을 나타내는 열거형

class Piece : public GameLoop
{
public:
	Piece(map_size x, map_size y, Tetris& tetris);
	virtual ~Piece() = default;

    void Update(float deltaTime) override;

    bool IsCollision(const low_uint toRotateShape[4][4], map_size x, map_size y, bool isBeforePlace = true) const;

    void Rotate(bool isClockwise = true);   // 조각을 회전시키는 메소드
    void Move(MoveDirection direction);     // 조각을 이동시키는 메소드
	void HardDrop();                        // 조각을 즉시 아래로 이동시키는 메소드
	void SoftDrop(float deltaTime);         // 조각을 한 칸 아래로 이동시키는 메소드
    void StopSoftDrop();                    // 소프트 드롭 타이머를 초기화하는 메소드
    void Place();

	void ShowGhost();                       // 고스트 조각을 표시하는 메소드

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
			{{0,0},{-2,0},{1,0},{-2,-1},{1,2}},   // 0->1 (시계)
			{{0,0},{-1,0},{2,0},{-1,2},{2,-1}},   // 0->3 (반시계)
			{}, {}
		},

		// from 1
		{
			{{0,0},{-1,0},{2,0},{-1,2},{2,-1}},   // 1->2 (시계)
			{{0,0},{2,0},{-1,0},{2,1},{-1,-2}},   // 1->0 (반시계)
			{}, {}
		},

		// from 2
		{
			{{0,0},{2,0},{-1,0},{2,1},{-1,-2}},   // 2->3 (시계)
			{{0,0},{1,0},{-2,0},{1,-2},{-2,1}},   // 2->1 (반시계)
			{}, {}
		},

		// from 3
		{
			{{0,0},{1,0},{-2,0},{1,-2},{-2,1}},   // 3->0 (시계)
			{{0,0},{-2,0},{1,0},{-2,-1},{1,2}},   // 3->2 (반시계)
			{}, {}
		}
	};

	static constexpr Position normalKick[4][4][5] =
	{
		// from 0
		{
			{{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}}, // 0->1 (시계)
			{{0,0},{1,0},{1,1},{0,-2},{1,-2}},    // 0->3 (반시계)
			{}, {}
		},

		// from 1
		{
			{{0,0},{1,0},{1,-1},{0,2},{1,2}},     // 1->2 (시계)
			{{0,0},{1,0},{1,1},{0,-2},{1,-2}},    // 1->0 (반시계)
			{}, {}
		},

		// from 2
		{
			{{0,0},{1,0},{1,1},{0,-2},{1,-2}},    // 2->3 (시계)
			{{0,0},{-1,0},{-1,-1},{0,2},{-1,2}},  // 2->1 (반시계)
			{}, {}
		},
		// from 3
		{
			{{0,0},{-1,0},{-1,-1},{0,2},{-1,2}},  // 3->0 (시계)
			{{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}}, // 3->2 (반시계)
			{}, {}
		}
	};

    static constexpr low_uint rotationCount = 4;

	std::vector<Position> removeBlockTarget;	// 조각이 이동하거나 회전할 때 제거해야 하는 블록들의 위치를 저장하는 벡터
    Tetris& tetris;	                            // 조각이 속한 테트리스 게임 객체에 대한 참조
    Position current;
    low_uint currentRotation = 0;

	static constexpr float dropInterval = 0.8f;	// 조각이 자동으로 떨어지는 간격 (초 단위)
	float dropRemainTime = 0.0f;                // 다음 자동 낙하까지 남은 시간 (초 단위)

    // 소프트 드롭 관련 변수
    float softDropTimer = 0.0f;							// 소프트 드롭 타이머
    constexpr static float softDropInterval = 0.05f;	// 소프트 드롭 간격 (초)
};