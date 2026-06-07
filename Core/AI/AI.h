#pragma once

#include "Core/Tetris.h"
#include "Core/AI/AIInput.h"

class AI
{
public:
	AI() = default;

private:
	Tetris tetris;
	AIInput aiInput { tetris };
};