#pragma once

#include "Piece.h"

#include "Utility/Type.h"

class JPiece : public Piece
{
public:
	JPiece(class Tetris& tetris);
	~JPiece() override = default;

	void Initialize() override;
	void Update(float deltaTime) override;

protected:
    const low_uint(&GetRotateShape() const)[4][4][4] override;
	const PieceType GetPieceType() const override;

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

