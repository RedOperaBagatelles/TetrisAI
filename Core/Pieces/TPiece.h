#pragma once

#include "Piece.h"

#include "Utility/Type.h"

class TPiece : public Piece
{
public:
    TPiece(class Tetris& tetris);
    ~TPiece() override = default;

private:
    static constexpr low_uint rotateShape[4][4][4] =
    {
        {
            {0,0,0,0},
            {0,1,0,0},
            {1,1,1,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {1,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    };
};
