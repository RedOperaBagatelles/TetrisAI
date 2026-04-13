#include "Piece.h"

#include "Core/Tetris.h"

Piece::Piece(int x, int y) : centerX(x), centerY(y)
{

}

int Piece::NormalizeRotation(int value)
{
    return (value % rotationCount + rotationCount) % rotationCount;
}