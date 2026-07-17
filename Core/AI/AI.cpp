#include "AI.h"

#include "ConnectAI/TCPClient.h"

void AI::Update(float deltaTime)
{
	// 현재 게임 상태(보드 + 현재 조각 + 홀드 + 다음 조각들)를 직렬화하여 Python 서버로 전송
	TCPClient::GetInstance().SendMessage(aiOutput.ToByte());
}
