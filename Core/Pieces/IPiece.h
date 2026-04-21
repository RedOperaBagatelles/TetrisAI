#pragma once

#include "Piece.h"

#include "Utility/Type.h"
#include "Utility/GameTimer.h"


class IPiece : public Piece
{
public:
    IPiece(class Tetris& tetris);
	~IPiece() override = default;

private:
    static constexpr low_uint rotateShape[4][4][4] =
    {
        {
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0}
        },
        {
            {0,0,0,0},
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0}
        }
    };
};