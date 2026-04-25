#pragma once

#include "Piece.h"

#include "Utility/Type.h"

class JPiece : public Piece
{
public:
	JPiece(class Tetris& tetris);
	~JPiece() override = default;

	void Update(float deltaTime) override;

private:
    void Move(Position beforePosition) override;
    void Draw() override;

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

