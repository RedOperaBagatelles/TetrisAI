#include "Core/Window.h"

#include "Utility/GameTimer.h"

#include "Core/AI/ConnectAI/TCPClient.h"
#include "Core/AI/ConnectAI/GameStateSender.h"
#include "Core/AI/ConnectAI/PiecePlacementSender.h"

int main()
{
	Window window;
	window.Initialize();

	// 생성할 테트리스 개수만 지정하면 한 창 안에 모든 보드가 배치된다.
	// 각 보드는 생성 순서대로 gameID(1부터 시작)를 부여받는다.
	auto games = window.CreateTetris(100);

    TCPClient::GetInstance().SetGames(games);
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
