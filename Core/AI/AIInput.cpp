#include "AIInput.h"

#include "Core/Pieces/Piece.h"
#include "Core/Tetris.h"

AIInput::AIInput(Tetris& tetris) : tetris(tetris)
{
	
}

void AIInput::Action(AIAction action)
{
	std::shared_ptr<Piece> currentPiece = tetris.GetCurrentPiece();

	if (currentPiece == nullptr)
		return;

	switch (action)
	{
	case AIAction::MoveLeft:
		currentPiece->Move(MoveDirection::Left);
		break;

	case AIAction::MoveRight:
		currentPiece->Move(MoveDirection::Right);
		break;

	case AIAction::MoveDown:
		currentPiece->Move(MoveDirection::Down);
		break;

	case AIAction::RotateClockwise:
		currentPiece->Rotate(true);
		break;

	case AIAction::RotateCounterClockwise:
		currentPiece->Rotate(false);
		break;

	case AIAction::HardDrop:
		currentPiece->HardDrop();
		break;
	}
}
