#pragma once

#include "Piece.h"

#include <map>

class JPiece : public Piece
{
public:
	JPiece();

private:
	static constexpr low_uint rotateShape[4][4][4] =
	{
        {
            {0,0,0,0},
            {1,0,0,0},
            {1,1,1,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {1,1,0,0},
            {0,0,0,0}
        }
	};
};

