#include "GameStateSender.h"

#include "Core/AI/AIOutput.h"
#include "Core/Tetris.h"

// TCPClient는 windows.h(SFML)보다 마지막에 포함해야 하며 내부의 #undef SendMessage로 매크로 충돌을 방지함
#include "TCPClient.h"

#include <utility>

std::map<GameCapacity, std::vector<uint8_t>> GameStateSender::pendingMessages;

void GameStateSender::Send(Tetris& tetris)
{
	TCPClient& client = TCPClient::GetInstance();

	// 서버가 선택한 액션 시퀀스를 실행하는 동안에는 전송하지 않음
	if (client.IsExecutingActions())
		return;

	AIOutput aiOutput(tetris);
	std::vector<uint8_t> bytes = aiOutput.ToByte();

	// 서버 연결 전이면 게임별 마지막 메시지를 보관해 두었다가 연결 성공 후 Flush()에서 전송
	if (!client.IsConnected())
	{
		pendingMessages[tetris.GetGameID()] = std::move(bytes);
		return;
	}

	client.SendMessage(bytes);
}

void GameStateSender::Flush()
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
