#pragma once

#include "Core/Tetris.h"
#include "Utility/Type.h"

enum class PieceType { None, I, J, L, O, S, T, Z };				// 테트리스 조각의 종류를 나타내는 열거형
enum class PieceRotationType { Spawn, Right, Reverse, Left };	// 테트리스 조각의 회전 상태를 나타내는 열거형

class Piece
{
public:
	Piece(map_size x, map_size y);

    bool Rotate(class Tetris& tetris, PieceType pieceType, low_uint rotateShape[4][4][4], bool isClockwise = true); // 조각을 회전시키는 순수 가상 함수

    bool IsCollision(const Tetris& tetris, const map_size rotateShape[4][4], map_size x, map_size y) const;

protected:
	static int NormalizeRotation(low_uint value);

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

	map_size currentX;
    map_size currentY;
	low_uint currentRotation = 0;
};