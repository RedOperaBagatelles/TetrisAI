#include "IPiece.h"

#include "Core/Tetris.h"

IPiece::IPiece(int x, int y) : Piece(x, y)
{

}

void IPiece::Rotate(Tetris& tetris)
{
	// SRS 회전 시스템의 시계 방향 회전 시 이동 데이터 (제자리 회전, 왼쪽으로 2칸, 오른쪽으로 1칸, 왼쪽으로 2칸 + 아래로 1칸, 오른쪽으로 1칸 + 위로 2칸)
    static constexpr std::array<std::array<Position, 5>, 4> clockwiseKickData =
    {
        std::array<Position, 5>{ Position{ 0,  0 }, Position{-2,  0 }, Position{ 1,  0 }, Position{-2, -1 }, Position{ 1,  2 } }, // 0 -> 1
        std::array<Position, 5>{ Position{ 0,  0 }, Position{-1,  0 }, Position{ 2,  0 }, Position{-1,  2 }, Position{ 2, -1 } }, // 1 -> 2
        std::array<Position, 5>{ Position{ 0,  0 }, Position{ 2,  0 }, Position{-1,  0 }, Position{ 2,  1 }, Position{-1, -2 } }, // 2 -> 3
        std::array<Position, 5>{ Position{ 0,  0 }, Position{ 1,  0 }, Position{-2,  0 }, Position{ 1, -2 }, Position{-2,  1 } }  // 3 -> 0
    };

    const int targetRotation = NormalizeRotation(currentRotation + 1);

	// 이동 데이터 순회하며 회전 시도
    for (const auto& kick : clockwiseKickData[currentRotation])
    {
        if (!CanPlace(tetris, centerX + kick.x, centerY + kick.y, rotationData[targetRotation]))
            continue;

        centerX += kick.x;
        centerY += kick.y;
        currentRotation = targetRotation;

        return;
    }
}

void IPiece::BackwardRotate(Tetris& tetris)
{
	// SRS 회전 시스템의 반시계 방향 회전 시 이동 데이터 (제자리 회전, 왼쪽으로 1칸, 오른쪽으로 2칸, 왼쪽으로 1칸 + 아래로 2칸, 오른쪽으로 2칸 + 위로 1칸)
    static constexpr std::array<std::array<Position, 5>, 4> counterClockwiseKickData =
    {
        std::array<Position, 5>{ Position{ 0,  0 }, Position{-1,  0 }, Position{ 2,  0 }, Position{-1,  2 }, Position{ 2, -1 } }, // 0 -> 3
        std::array<Position, 5>{ Position{ 0,  0 }, Position{ 2,  0 }, Position{-1,  0 }, Position{ 2,  1 }, Position{-1, -2 } }, // 1 -> 0
        std::array<Position, 5>{ Position{ 0,  0 }, Position{ 1,  0 }, Position{-2,  0 }, Position{ 1, -2 }, Position{-2,  1 } }, // 2 -> 1
        std::array<Position, 5>{ Position{ 0,  0 }, Position{-2,  0 }, Position{ 1,  0 }, Position{-2, -1 }, Position{ 1,  2 } }  // 3 -> 2
    };

    const int targetRotation = NormalizeRotation(currentRotation - 1);

    for (const auto& kick : counterClockwiseKickData[currentRotation])
    {
        if (!CanPlace(tetris, centerX + kick.x, centerY + kick.y, rotationData[targetRotation]))
            continue;

        centerX += kick.x;
        centerY += kick.y;
        currentRotation = targetRotation;

        return;
    }
}

bool IPiece::CanRotate(const Tetris& tetris, int rotationCount) const
{
    return CanPlace(tetris, centerX, centerY, rotationData[Piece::NormalizeRotation(rotationCount)]);
}

bool IPiece::CanPlace(const Tetris& tetris, int centerX, int centerY, const std::array<Position, 4>& blocks) const
{
    const auto board = tetris.GetBoard();

    for (const auto& block : blocks)
    {
        const int x = centerX + block.x;
        const int y = centerY + block.y;

        if (x < 0 || x >= Tetris::width)
            return false;

        if (y < 0 || y >= Tetris::height + Tetris::topMarginBlock)
            return false;

        if (board[y][x] != 0)
            return false;
    }

    return true;
}