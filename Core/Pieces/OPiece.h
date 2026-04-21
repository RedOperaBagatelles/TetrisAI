#pragma once

#include "Piece.h"

#include "Utility/Type.h"

class OPiece : public Piece
{
public:
    OPiece(Tetris& tetris);
    ~OPiece() override = default;

private:
    static constexpr low_uint rotateShape[4][4][4] =
    {
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        }
    };
};