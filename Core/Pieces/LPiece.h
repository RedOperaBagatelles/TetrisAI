#pragma once

#include "Piece.h"

#include "Utility/Type.h"

class LPiece : public Piece
{
public:
	LPiece(class Tetris& tetris);

	void Initialize() override;
	void Update(float deltaTime) override;

	// 조각의 형태 데이터에 정적으로 접근하기 위한 메소드
	static const low_uint(&GetStaticShape())[4][4][4] { return rotateShape; }

protected:
    const low_uint(&GetRotateShape() const)[4][4][4] override;
	const PieceType GetPieceType() const override;

private:
	static constexpr low_uint rotateShape[4][4][4] =
	{
        {
            {0,0,0,0},
            {0,0,1,0},
            {1,1,1,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {1,0,0,0},
            {0,0,0,0}
        },
        {
            {1,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        }
	};
};

