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

		// AIAction::Holding은 현재 조각을 홀드하는 동작이므로, currentPiece를 사용하지 않고 Tetris 객체의 HoldPiece() 메서드를 호출합니다.
		case AIAction::Holding:
			tetris.HoldPiece();
			break;
	}
}
