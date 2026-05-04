#include "Piece.h"

#include "Core/Tetris.h"
#include "Core/Pieces.h"

#include <memory>

Piece::Piece(map_size x, map_size y, Tetris& tetris) : current{x, y}, tetris(tetris)
{

}

void Piece::Update(float deltaTime)
{
	dropRemainTime -= deltaTime;

	if (dropRemainTime <= 0.0f)
	{
		dropRemainTime = dropInterval;	// 다음 자동 낙하까지 남은 시간 초기화

		// 조각을 한 칸 아래로 이동
		Move(MoveDirection::Down);	
	}
}

void Piece::Rotate(bool isClockwise)
{
	PieceType pieceType = GetPieceType();
	auto& rotateShape = GetRotateShape();

	int movement = isClockwise ? 1 : -1;									// 회전 방향에 따라 이동량 결정
	low_uint nextRotation = NormalizeRotation(currentRotation + movement);	// 다음 회전 상태 계산

	// 호출한 클래스가 IPiece인 경우 iKick 데이터를 사용, 그렇지 않으면 normalKick 데이터를 사용
	const Position* kick = nullptr;

	if (pieceType == PieceType::I)
		kick = iKick[currentRotation][nextRotation];

	else
		kick = normalKick[currentRotation][nextRotation];

	// 충돌이 발생하지 않는 위치를 찾을 때까지 5가지 킥 데이터를 시도
	for (map_size i = 0; i < 5; i++)
	{
		// 킥 데이터 적용 후 새로운 위치 계산
		map_size nextX = current.x + kick[i].x, nextY = current.y + kick[i].y;

		if (!IsCollision(rotateShape[nextRotation], nextX, nextY))
		{
			Position before = current;
			current.x = nextX;
			current.y = nextY;

			const int deltaX = static_cast<int>(current.x) - static_cast<int>(before.x);
			const int deltaY = static_cast<int>(current.y) - static_cast<int>(before.y);

			// 회전 전후 형태 변화와 위치 변화를 모두 고려하여 제거할 블록 추적
			for (int row = 0; row < 4; row++)
			{
				for (int col = 0; col < 4; col++)
				{
					if (rotateShape[currentRotation][row][col] == 0)
						continue;

					const int beforeX = static_cast<int>(before.x) + col;
					const int beforeY = static_cast<int>(before.y) + row;

					if (beforeX < 0 || beforeX >= Tetris::width || beforeY < 0 || beforeY >= Tetris::maxHeight)
						continue;

					// 회전 후 같은 위치에 블록이 있는지 확인
					const int overlapLocalX = col - deltaX;
					const int overlapLocalY = row - deltaY;

					const bool isOverlappedByNext =
						overlapLocalX >= 0 && overlapLocalX < 4 &&
						overlapLocalY >= 0 && overlapLocalY < 4 &&
						rotateShape[nextRotation][overlapLocalY][overlapLocalX] != 0;

					// 회전 후에도 블록이 있는 같은 위치라면 제거 대상에서 제외
					if (!isOverlappedByNext)
						removeBlockTarget.emplace_back(beforeX, beforeY);
				}
			}

			currentRotation = nextRotation;
			Draw();	// 회전 후 그려줌
			return;
		}
	}
}

bool Piece::IsCollision(const low_uint toRotateShape[4][4], map_size x, map_size y, bool isBeforePlace) const
{
	auto board = tetris.GetBoard();				// 게임 보드 가져오기
	auto fromRotateShape = GetRotateShape();	// 현재 조각의 회전 형태 가져오기

	for (map_size i = 0; i < 4; i++)
	{
		for (map_size j = 0; j < 4; j++)
		{
			if (toRotateShape[i][j] == 0)	// 조각의 빈 공간은 충돌에서 제외
				continue;

			map_size newX = x + j, newY = y + i;

			if (newX < 0 || newX >= Tetris::width || newY < 0 || newY > Tetris::height + Tetris::topMarginBlock)
				return true;

			bool isSelf = false;

			// 기존 조각이 원래 차지하고 있던 공간인지 확인하여 자신과의 충돌은 무시
			if (isBeforePlace)
			{
				int localX = static_cast<int>(newX) - static_cast<int>(current.x);
				int localY = static_cast<int>(newY) - static_cast<int>(current.y);

				if (localX >= 0 && localX < 4 && localY >= 0 && localY < 4)
				{
					if (fromRotateShape[currentRotation][localY][localX] != 0)
						isSelf = true;
				}
			}

			// 보드 저장 로직과 동일하게 인덱스 변환 후 충돌 검사
			if (!isSelf && board[Tetris::maxHeight - (newY + 1)][newX])
				return true;
		}
	}

	return false;
}

int Piece::NormalizeRotation(low_uint value)
{
	return (value % rotationCount + rotationCount) % rotationCount;
}

void Piece::Draw()
{
	auto& rotateShape = GetRotateShape();
	auto map = tetris.GetBoard();
	PieceType pieceType = GetPieceType();

	for (int i = 0; i < 4; i++)
	{
		map_size newY = Tetris::maxHeight - (current.y + i + 1);

		for (int j = 0; j < 4; j++)
		{
			map_size newX = current.x + j;

			if (rotateShape[currentRotation][i][j] != 0)
				map[newY][newX] = (low_uint)pieceType;	// 조각의 블록을 게임 보드에 배치
		}
	}

	// 이전에 있는 조각의 블록을 게임 보드에서 제거
	while (!removeBlockTarget.empty())
	{
		auto& position = removeBlockTarget.back();
		map[Tetris::maxHeight - (position.y + 1)][position.x] = 0;
		removeBlockTarget.pop_back();
	}
}

void Piece::Move(MoveDirection moveDirection)
{
	auto board = tetris.GetBoard();	// 게임 보드 가져오기
	Position next = current;		// 이동할 위치를 계산하기 위해 현재 위치 복사

	switch (moveDirection)
	{
		case MoveDirection::Left:
			next.x--;
			break;

		case MoveDirection::Right:
			next.x++;
			break;

		case MoveDirection::Down:
			next.y++;
			break;
	}

	auto& rotateShape = GetRotateShape();

	// current가 변경되기 전에 다음 위치인 next를 이용해 충돌 검사
	if (IsCollision(rotateShape[currentRotation], next.x, next.y))
	{
		if (moveDirection == MoveDirection::Down)
			Place();	// 아래로 이동할 때 충돌이 발생하면 조각을 고정

		return;	// 충돌이 발생하면 이동을 취소하고 리턴
	}
		

	Position before = current;
	current = next;

	const int deltaX = static_cast<int>(current.x) - static_cast<int>(before.x);
	const int deltaY = static_cast<int>(current.y) - static_cast<int>(before.y);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (rotateShape[currentRotation][i][j] == 0)
				continue;

			const int beforeX = static_cast<int>(before.x) + j;
			const int beforeY = static_cast<int>(before.y) + i;

			if (beforeX < 0 || beforeX >= Tetris::width || beforeY < 0 || beforeY >= Tetris::maxHeight)
				continue;

			const int overlapLocalX = j - deltaX;
			const int overlapLocalY = i - deltaY;

			const bool isOverlappedByCurrent =
				overlapLocalX >= 0 && overlapLocalX < 4 &&
				overlapLocalY >= 0 && overlapLocalY < 4 &&
				rotateShape[currentRotation][overlapLocalY][overlapLocalX] != 0;

			// 조각이 이동하거나 회전할 때 제거해야 하는 블록들의 위치를 저장
			if (!isOverlappedByCurrent)
				removeBlockTarget.emplace_back(beforeX, beforeY);
		}
	}

	Draw();	// 이동 후 자식 클래스의 Draw() 호출
}

void Piece::HardDrop()
{
	auto& rotateShape = GetRotateShape();

	while (!IsCollision(rotateShape[currentRotation], current.x, current.y + 1))
		Move(MoveDirection::Down);	// 조각이 더 이상 아래로 이동할 수 없을 때까지 아래로 이동

	Place();	// 조각을 고정
}

void Piece::SoftDrop(float deltaTime)
{
	softDropTimer += deltaTime;

	if (softDropTimer >= softDropInterval)
	{
		Move(MoveDirection::Down);
		softDropTimer = 0.0f;
	}
}

void Piece::StopSoftDrop()
{
	softDropTimer = 0.0f;
}

void Piece::Place()
{
	Draw();	// 조각을 게임 보드에 고정

	// 줄이 완성되었는지 검사하여 제거
	tetris.RemoveLine();	

	std::shared_ptr<Piece> currentPiece = tetris.GetCurrentPiece();
	currentPiece = nullptr;	// 현재 조각이 고정되었으므로 Tetris 객체의 currentPiece를 nullptr로 설정하여 새로운 조각이 생성될 수 있도록 함

	if (!tetris.CreatePiece())
		tetris.Initialize(); // 블록 생성을 할 수 없는 경우(게임 오버) 게임 초기화
}

void Piece::ShowGhost()
{
	auto& rotateShape = GetRotateShape();
	Position ghostPos = current;

	while (!IsCollision(rotateShape[currentRotation], ghostPos.x, ghostPos.y + 1, true))
		ghostPos.y++;	// 고스트 조각이 더 이상 아래로 이동할 수 없을 때까지 아래로 이동

	// 현재 조각 위치 기준으로 고스트 조각의 위치에 고스트 블록을 그려줌
	std::vector<std::shared_ptr<const sf::Drawable>> ghostBlocks;
	for (int i = 0; i < 4; i++)
	{
		map_size newY = ghostPos.y + i - 4;

		for (int j = 0; j < 4; j++)
		{
			map_size newX = ghostPos.x + j;

			if (rotateShape[currentRotation][i][j] != 0)
			{
				auto ghostBlock = Pieces::GetRenderOnePices(newX, newY, PieceType::Ghost, tetris.GetStartX(), tetris.GetStartY());
				ghostBlocks.push_back(ghostBlock);
			}
		}
	}

	tetris.AddRenderPieces(ghostBlocks);	// 고스트 블록들을 Tetris 객체에 추가하여 렌더링되도록 함
}