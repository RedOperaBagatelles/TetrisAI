#pragma once

#include "Core/AI/AIInput.h"
#include "Core/Pieces/Piece.h"

#include "Utility/Type.h"

#include <vector>

// AI가 조작할 수 있는 조각의 위치와 회전 상태를 나타내는 구조체
struct PiecePlacement
{
	Position position;				// 놓을 위치의 좌표
	low_uint rotation;				// 놓을 조각의 회전 상태

	std::vector<AIAction> actions;	// 현재 조각을 해당 위치에 놓기 위해 필요한 행동들의 시퀀스
};

// AI가 키보드를 통해서 가능한 모든 조작을 탐색하는 클래스
class PiecePlacementExplorer
{
public:
	PiecePlacementExplorer() = delete;

	static std::vector<PiecePlacement> GetAllPossiblePlacements(class Tetris& tetris);

private:
	// AI가 탐색할 상태를 나타내는 구조체
	struct State
	{
		PieceType currentPiece;
		Position position;
		low_uint rotation;

		PieceType holdPiece;
		bool hasUsedHold;
		bool isPlaced = false;			// 하드 드랍으로 조각이 고정된 상태인지 여부
	};

	// AI가 탐색할 상태를 해시맵에서 키로 사용할 수 있도록 정의한 구조체
	struct StateKey
	{
		State state;

		bool operator==(const StateKey& other) const
		{
			return state.currentPiece == other.state.currentPiece &&
				state.position == other.state.position &&
				state.rotation == other.state.rotation &&
				state.holdPiece == other.state.holdPiece &&
				state.hasUsedHold == other.state.hasUsedHold &&
				state.isPlaced == other.state.isPlaced;
		}
	};

	struct StateKeyHash
	{
		std::size_t operator()(const StateKey& key) const
		{
			std::size_t hash = 0;

			hash = hash * 31 + static_cast<int>(key.state.currentPiece);
			hash = hash * 31 + key.state.position.x;
			hash = hash * 31 + key.state.position.y;
			hash = hash * 31 + key.state.rotation;
			hash = hash * 31 + static_cast<int>(key.state.holdPiece);
			hash = hash * 31 + static_cast<int>(key.state.hasUsedHold);
			hash = hash * 31 + static_cast<int>(key.state.isPlaced);

			return hash;
		}
	};

	struct Node
	{
		State state;

		int parent;
		AIAction action;
	};

	// 조각 종류에 해당하는 회전 형태 데이터를 반환하는 메소드
	static const low_uint(&GetShape(PieceType type))[4][4][4];

	// 조각 종류에 해당하는 스폰 위치를 반환하는 메소드
	static Position GetSpawnPosition(PieceType type);

	// 해당 상태의 조각이 보드 위에서 충돌 없이 존재할 수 있는지 확인하는 메소드 (boardPiece는 보드에 그려져 있는 현재 조각)
	static bool IsValidState(class Tetris& tetris, const State& state, const State& boardPiece);

	// 부모 노드들을 따라가면서 해당 노드까지의 액션 시퀀스를 만드는 메소드
	static std::vector<AIAction> BuildActions(const std::vector<Node>& nodes, int index);

	static StateKey CreateKey(const State& state);
};