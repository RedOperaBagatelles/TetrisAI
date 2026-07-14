#pragma once

#include "Core/GameLoop.h"
#include "Core/Tetris.h"
#include "Core/AI/AIInput.h"

#include <string>

class AI : public GameLoop
{
public:
	AI() = default;

private:
	Tetris tetris;
	AIInput aiInput { tetris };
};