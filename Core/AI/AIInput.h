#pragma once

// AI가 수행할 수 있는 행동을 나타내는 열거형
enum class AIAction { None, MoveLeft, MoveRight, MoveDown, RotateClockwise, RotateCounterClockwise, HardDrop };	

class AIInput
{
public:
	AIInput(class Tetris& tetris);

	void Action(AIAction action);

private:
	Tetris& tetris;
};