#include "Core/Window.h"

#include "Utility/GameTimer.h"

#include "Core/AI/ConnectAI/TCPClient.h"
#include "Core/AI/ConnectAI/GameStateSender.h"
#include "Core/AI/ConnectAI/PiecePlacementSender.h"

int main()
{
	Window window;
	window.Initialize();

    TCPClient::GetInstance().SetTetris(&window.GetTetris());
    TCPClient::GetInstance().Initialize();

	// 연결 초기화 후 보관된 메시지가 있으면 전송 (게임 상태 → 가능한 배치 순서 유지)
	GameStateSender::Flush();
	PiecePlacementSender::Flush();

    bool isSucceed = true;

    // 게임 타이머 객체
    GameTimer gameTimer;
    
    while (isSucceed)
    {
        gameTimer.Tick();
		const float deltaTime = gameTimer.DeltaTime();
    
        window.Update(deltaTime);
        isSucceed = window.Render();
    }

    return 0;
}