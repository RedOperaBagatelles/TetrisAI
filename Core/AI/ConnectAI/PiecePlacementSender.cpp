#include "PiecePlacementSender.h"

#include "Core/Tetris.h"
#include "Core/Pieces/Piece.h"

// TCPClient는 windows.h(SFML)보다 마지막에 포함해야 하며 내부의 #undef SendMessage로 매크로 충돌을 방지함
#include "TCPClient.h"

#include <memory>

std::vector<uint8_t> PiecePlacementSender::pendingMessage;

// 전송되는 바이트 ex (총 21바이트) :
// 02 03 00 00 02 00		헤더  : type = 0x02, x = 3, y = 0, rotation = 0, 배치개수 = 2 (u16)
// 04 12 00 02 00 02 06		배치1 : x = 4, y = 18(0x12), rot = 0, 액션개수 = 2, [MoveRight, HardDrop]
// 05 13 01 03 00 02 04 06	배치2 : x = 5, y = 19(0x13), rot = 1, 액션개수 = 3, [MoveRight, RotateCW, HardDrop]
void PiecePlacementSender::Send(const Tetris& tetris, const std::vector<PiecePlacement>& placements)
{
	std::shared_ptr<Piece> currentPiece = tetris.GetCurrentPiece();

	// 조각이 없는 상태라면 보관 중인 메시지도 유효하지 않으므로 제거
	if (currentPiece == nullptr)
	{
		pendingMessage.clear();
		return;
	}

	// 전송할 메시지 크기 계산 및 바이트 벡터 준비
	std::vector<uint8_t> bytes;
	bytes.reserve(headerSize + placements.size() * (placementSize + 4));

	const Position position = currentPiece->GetPosition();

	// 메시지 타입, 현재 조각의 위치와 회전 상태, 배치 개수 추가
	bytes.push_back(messageType);
	bytes.push_back(static_cast<uint8_t>(static_cast<int>(position.x)));
	bytes.push_back(static_cast<uint8_t>(static_cast<int>(position.y)));
	bytes.push_back(static_cast<uint8_t>(currentPiece->GetRotation()));

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

	TCPClient& client = TCPClient::GetInstance();

	// 서버 연결 전이면 마지막 메시지를 보관해 두었다가 연결 성공 후 Flush()에서 전송
	if (!client.IsConnected())
	{
		pendingMessage = std::move(bytes);
		return;
	}

	client.SendMessage(bytes);
}

void PiecePlacementSender::Flush()
{
	if (pendingMessage.empty())
		return;

	TCPClient& client = TCPClient::GetInstance();

	if (!client.IsConnected())
		return;

	client.SendMessage(pendingMessage);
	pendingMessage.clear();
}
