#include "PiecePlacementSender.h"

#include "Core/Tetris.h"
#include "Core/Pieces/Piece.h"

// TCPClient는 windows.h(SFML)보다 마지막에 포함해야 하며 내부의 #undef SendMessage로 매크로 충돌을 방지함
#include "TCPClient.h"

#include <memory>

std::map<GameCapacity, std::vector<uint8_t>> PiecePlacementSender::pendingMessages;

// 전송되는 바이트 ex (gameID = 1, 총 25바이트) :
// 02 01 00 03 00 00 01 06 02 00	헤더 : type = 0x02, gameID = 1(u16), x = 3, y = 0, rotation = 0, current = I(1), hold = T(6), 배치개수 = 2(u16)
// 04 12 00 02 00 02 06					배치1 : x = 4, y = 18(0x12), rot = 0, 액션개수 = 2, [MoveRight, HardDrop]
// 05 13 01 03 00 02 04 06				배치2 : x = 5, y = 19(0x13), rot = 1, 액션개수 = 3, [MoveRight, RotateCW, HardDrop]
void PiecePlacementSender::Send(const Tetris& tetris, const std::vector<PiecePlacement>& placements)
{
	TCPClient& client = TCPClient::GetInstance();

	// 서버가 선택한 액션 시퀀스를 실행하는 동안에는 전송하지 않음
	if (client.IsExecutingActions())
		return;

	std::shared_ptr<Piece> currentPiece = tetris.GetCurrentPiece();

	// 조각이 없는 상태라면 해당 게임의 보관 메시지도 유효하지 않으므로 제거
	if (currentPiece == nullptr)
	{
		pendingMessages.erase(tetris.GetGameID());
		return;
	}

	// 전송할 메시지 크기 계산 및 바이트 벡터 준비
	std::vector<uint8_t> bytes;
	bytes.reserve(headerSize + placements.size() * (placementSize + 4));

	const Position position = currentPiece->GetPosition();

	// 메시지 타입, gameID, 현재 조각의 위치와 회전 상태, 현재 조각과 홀드 조각의 종류, 배치 개수 추가
	bytes.push_back(messageType);

	// 대상 게임을 구분하는 gameID (리틀엔디안)
	for (size_t i = 0; i < gameIDSize; i++)
		bytes.push_back(static_cast<uint8_t>((tetris.GetGameID() >> (i * 8)) & 0xFF));

	bytes.push_back(static_cast<uint8_t>(static_cast<int>(position.x)));
	bytes.push_back(static_cast<uint8_t>(static_cast<int>(position.y)));
	bytes.push_back(static_cast<uint8_t>(currentPiece->GetRotation()));
	bytes.push_back(static_cast<uint8_t>(currentPiece->GetPieceType()));
	bytes.push_back(static_cast<uint8_t>(tetris.GetHoldPieceType()));

	// 배치 개수 (리틀엔디안 2바이트)
	bytes.push_back(static_cast<uint8_t>(placements.size() & 0xFF));
	bytes.push_back(static_cast<uint8_t>((placements.size() >> 8) & 0xFF));

	for (const PiecePlacement& placement : placements)
	{
		bytes.push_back(static_cast<uint8_t>(static_cast<int>(placement.position.x)));
		bytes.push_back(static_cast<uint8_t>(static_cast<int>(placement.position.y)));
		bytes.push_back(static_cast<uint8_t>(placement.rotation));

		// 해당 배치에 필요한 액션 개수 (리틀엔디안 2바이트)
		const size_t actionCount = placement.actions.size();

		bytes.push_back(static_cast<uint8_t>(actionCount & 0xFF));
		bytes.push_back(static_cast<uint8_t>((actionCount >> 8) & 0xFF));

		for (AIAction action : placement.actions)
			bytes.push_back(static_cast<uint8_t>(action));
	}

	// 서버 연결 전이면 게임별 마지막 메시지를 보관해 두었다가 연결 성공 후 Flush()에서 전송
	if (!client.IsConnected())
	{
		pendingMessages[tetris.GetGameID()] = std::move(bytes);
		return;
	}

	client.SendMessage(bytes);
}

void PiecePlacementSender::Flush()
{
	if (pendingMessages.empty())
		return;

	TCPClient& client = TCPClient::GetInstance();

	if (!client.IsConnected())
		return;

	for (const auto& [gameID, message] : pendingMessages)
		client.SendMessage(message);

	pendingMessages.clear();
}
