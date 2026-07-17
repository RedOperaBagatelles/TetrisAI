#pragma once

#include "Core/GameLoop.h"
#include "Core/Tetris.h"
#include "Core/AI/AIInput.h"
#include "Core/AI/AIOutput.h"

class AI : public GameLoop
{
public:
	AI() = default;

	void Update(float deltaTime) override;	// 매 프레임 현재 게임 상태를 직렬화하여 Python 서버로 전송

private:
	Tetris tetris;
	AIInput aiInput { tetris };
	AIOutput aiOutput{ tetris };	// 현재 게임 상태를 바이트로 직렬화하는 역할
};