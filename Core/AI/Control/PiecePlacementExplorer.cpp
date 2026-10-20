#include "PiecePlacementExplorer.h"

#include "Core/Tetris.h"
#include "Core/Pieces/Piece.h"
#include "Core/Pieces/IPiece.h"
#include "Core/Pieces/JPiece.h"
#include "Core/Pieces/LPiece.h"
#include "Core/Pieces/OPiece.h"
#include "Core/Pieces/SPiece.h"
#include "Core/Pieces/TPiece.h"
#include "Core/Pieces/ZPiece.h"
#include "Core/AI/AIInput.h"

#include <algorithm>
#include <queue>
#include <memory>
#include <unordered_set>

std::vector<PiecePlacement> PiecePlacementExplorer::GetAllPossiblePlacements(Tetris& tetris)
{
	std::vector<PiecePlacement> results;

	std::vector<Node> nodes;
	std::queue<int> next;
	std::unordered_set<StateKey, StateKeyHash> visited;

	std::shared_ptr<Piece> currentPiece = tetris.GetCurrentPiece();

	// 탐색을 위한 초기 상태 설정
	State start;
	start.currentPiece = currentPiece->GetPieceType();
	start.position = currentPiece->GetPosition();
	start.rotation = currentPiece->GetRotation();
	start.holdPiece = tetris.GetHoldPieceType();
	start.hasUsedHold = tetris.HasUsedHold();

	nodes.push_back({ start, -1, AIAction::None });
	next.push(0);
	visited.insert(CreateKey(start));

	// 현재 조각은 보드에 그려져 있으므로 충돌 검사 시 start 조각이 차지하는 칸은 제외 대상
	constexpr AIAction actions[] = { AIAction::MoveLeft, AIAction::MoveRight, AIAction::MoveDown, AIAction::RotateClockwise, AIAction::RotateCounterClockwise, AIAction::Holding, AIAction::HardDrop };

	while (!next.empty())
	{
		const int currentIndex = next.front();
		next.pop();

		// 노드가 추가되면 참조가 무효화될 수 있으므로 상태를 복사해 둠
		const State current = nodes[currentIndex].state;

		// 하드 드랍으로 조각이 고정된 상태는 더 이상 탐색하지 않고 결과로만 수집
		if (current.isPlaced)
		{
			results.push_back({ current.position, current.rotation, BuildActions(nodes, currentIndex) });
			continue;
		}

		for (int i = 0; i < sizeof(actions) / sizeof(actions[0]); i++)
		{
			State nextState = current;
			const AIAction action = actions[i];

			switch (action)
			{
				case AIAction::MoveLeft:
					nextState.position.x--;
					break;

				case AIAction::MoveRight:
					nextState.position.x++;
					break;

				case AIAction::MoveDown:
					nextState.position.y++;
					break;

				case AIAction::RotateClockwise:
				case AIAction::RotateCounterClockwise:
				{
					const low_uint nextRotation = (nextState.rotation + (action == AIAction::RotateClockwise ? 1 : 3)) % 4;
					const Position* kicks = Piece::GetKick(nextState.currentPiece, nextState.rotation, nextRotation);
					bool isRotated = false;

					// 게임과 동일하게 킥 데이터를 순서대로 시도해 처음 성공한 킥만 적용
					for (low_uint k = 0; k < 5; k++)
					{
						State kicked = nextState;
						kicked.rotation = nextRotation;
						kicked.position.x += kicks[k].x;
						kicked.position.y += kicks[k].y;

						if (IsValidState(tetris, kicked, start))
						{
							nextState = kicked;
							isRotated = true;
							break;
						}
					}

					// 모든 킥이 실패하면 회전할 수 없는 상태이므로 다음 액션으로 넘어감
					if (!isRotated)
						continue;

					break;
				}

				case AIAction::Holding:
				{
					if (nextState.hasUsedHold)
						continue;	// 이미 홀드를 사용했으면 무시

					// 홀드 슬롯이 비어 있으면 게임과 동일하게 다음 조각을 꺼내어 스폰
					if (nextState.holdPiece == PieceType::None && tetris.GetNextPieces().empty())
						continue;

					const PieceType swappedOut = nextState.currentPiece;

					nextState.currentPiece = (nextState.holdPiece == PieceType::None)
						? tetris.GetNextPieces().front()
						: nextState.holdPiece;

					nextState.holdPiece = swappedOut;
					nextState.hasUsedHold = true;
					nextState.rotation = 0;
					nextState.position = GetSpawnPosition(nextState.currentPiece);

					break;
				}

				case AIAction::HardDrop:
				{
					// 조각이 더 이상 아래로 내려갈 수 없을 때까지 이동한 위치가 하드 드랍 결과
					for (;;)
					{
						State below = nextState;
						below.position.y++;

						if (!IsValidState(tetris, below, start))
							break;

						nextState = below;
					}

					nextState.isPlaced = true;
					break;
				}
			}

			// 액션을 수행할 수 없는 상태라면 큐에 넣지 않음
			if (!IsValidState(tetris, nextState, start))
				continue;

			const StateKey key = CreateKey(nextState);

			// 이미 방문한 상태인지 확인
			if (visited.find(key) == visited.end())
			{
				nodes.push_back({ nextState, currentIndex, action });
				next.push(static_cast<int>(nodes.size() - 1));
				visited.insert(key);
			}
		}
	}

	return results;
}

const low_uint(&PiecePlacementExplorer::GetShape(PieceType type))[4][4][4]
{
	switch (type)
	{
		case PieceType::I: return IPiece::GetStaticShape();
		case PieceType::J: return JPiece::GetStaticShape();
		case PieceType::L: return LPiece::GetStaticShape();
		case PieceType::O: return OPiece::GetStaticShape();
		case PieceType::S: return SPiece::GetStaticShape();
		case PieceType::T: return TPiece::GetStaticShape();
		case PieceType::Z: return ZPiece::GetStaticShape();

		default:
		{
			static constexpr low_uint emptyShape[4][4][4] = {};
			return emptyShape;
		}
	}
}

Position PiecePlacementExplorer::GetSpawnPosition(PieceType type)
{
	// 조각 생성자와 동일한 초기 위치 유지 (O, S는 x = 4, 나머지는 x = 3)
	if (type == PieceType::O || type == PieceType::S)
		return { 4, 0 };

	return { 3, 0 };
}

bool PiecePlacementExplorer::IsValidState(Tetris& tetris, const State& state, const State& boardPiece)
{
	if (state.currentPiece == PieceType::None)
		return false;

	const auto board = tetris.GetBoard();
	const auto& shape = GetShape(state.currentPiece);
	const auto& boardShape = GetShape(boardPiece.currentPiece);

	for (map_size i = 0; i < 4; i++)
	{
		for (map_size j = 0; j < 4; j++)
		{
			if (shape[state.rotation][i][j] == 0)
				continue;

			const map_size x = state.position.x + j;
			const map_size y = state.position.y + i;

			if (x < 0 || x >= Tetris::width || y < 0 || y > Tetris::height + Tetris::topMarginBlock)
				return false;

			const int localX = static_cast<int>(x) - static_cast<int>(boardPiece.position.x);
			const int localY = static_cast<int>(y) - static_cast<int>(boardPiece.position.y);

			// 보드에 그려져 있는 현재 조각이 차지하는 칸은 자신의 블록이므로 충돌에서 제외
			if (localX >= 0 && localX < 4 && localY >= 0 && localY < 4 &&
				boardShape[boardPiece.rotation][localY][localX] != 0)
				continue;

			if (board[Tetris::maxHeight - (y + 1)][x])
				return false;
		}
	}

	return true;
}

std::vector<AIAction> PiecePlacementExplorer::BuildActions(const std::vector<Node>& nodes, int index)
{
	std::vector<AIAction> actions;

	for (int i = index; i > 0; i = nodes[i].parent)
		actions.push_back(nodes[i].action);

	std::reverse(actions.begin(), actions.end());

	return actions;
}

PiecePlacementExplorer::StateKey PiecePlacementExplorer::CreateKey(const State& state)
{
	return StateKey{ state };
}
