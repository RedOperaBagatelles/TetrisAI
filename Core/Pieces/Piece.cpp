#include "Piece.h"

#include "Core/Tetris.h"

Piece::Piece(map_size x, map_size y, Tetris& tetris) : currentX(x), currentY(y), tetris(tetris)
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

bool Piece::Rotate(PieceType pieceType, low_uint rotateShape[4][4][4], bool isClockwise)
{
	int movement = isClockwise ? 1 : -1;									// 회전 방향에 따라 이동량 결정
	low_uint nextRotation = NormalizeRotation(currentRotation + movement);	// 다음 회전 상태 계산

	// 호출한 클래스가 IPiece인 경우 iKick 데이터를 사용, 그렇지 않으면 normalKick 데이터를 사용
	auto& kick = (pieceType == PieceType::I) ? iKick[currentRotation][nextRotation] : normalKick[currentRotation][nextRotation];

	// 충돌이 발생하지 않는 위치를 찾을 때까지 5가지 킥 데이터를 시도
	for (map_size i = 0; i < 5; i++)
	{
		// 킥 데이터 적용 후 새로운 위치 계산
		map_size nextX = currentX + kick[i].x, nextY = currentY + kick[i].y;

		if (!IsCollision(rotateShape[nextRotation], nextX, nextY))
		{
			currentX = nextX;
			currentY = nextY;
			currentRotation = nextRotation;

			return true;
		}
	}

	return false;
}

bool Piece::IsCollision(const map_size rotateShape[4][4], map_size x, map_size y) const
{
	auto board = tetris.GetBoard();		// 게임 보드 가져오기
	low_uint newRotation = NormalizeRotation(currentRotation + 1);	// 다음 회전 상태 계산

	for (map_size y = 0; y < 4; y++)
	{
		for (map_size x = 0; x < 4; x++)
		{
			map_size newX = currentX + x, newY = currentY + y;

			if (newX < 0 || newX >= tetris.width || newY < 0 || newY >= tetris.height + tetris.topMarginBlock)
				return true;

			if (board[newY][newX])
				return true;
		}
	}

	return false;
}

int Piece::NormalizeRotation(low_uint value)
{
	return (value % rotationCount + rotationCount) % rotationCount;
}

void Piece::Move(MoveDirection moveDirection)
{
	auto board = tetris.GetBoard();				// 게임 보드 가져오기
	Position before = { currentX, currentY };	// 이동하기 전의 위치 저장

	switch (moveDirection)
	{
		case MoveDirection::Left:
			currentX--;
			break;

		case MoveDirection::Right:
			currentX++;
			break;

		case MoveDirection::Down:
			currentY++;
			break;
	}

	Move(before);	// 이동 후 자식 클래스의 Move() 호출

	Draw();	// 이동 후 자식 클래스의 Draw() 호출
}