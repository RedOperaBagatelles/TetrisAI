#include "Core/Window.h"

#include "Utility/GameTimer.h"

#include "Core/AI/ConnectAI/TCPClient.h"
#include "Core/AI/AIOutput.h"

int main()
{
	Window window;
	window.Initialize();

    TCPClient::GetInstance().Initialize();

    AIOutput aiOutput(window.GetTetris());

    bool isSucceed = true;

    // 게임 타이머 객체
    GameTimer gameTimer;
    float sendTimer = 0.0f;
    
    while (isSucceed)
    {
        gameTimer.Tick();
		const float deltaTime = gameTimer.DeltaTime();
    
        window.Update(deltaTime);
        isSucceed = window.Render();

        sendTimer += deltaTime;
        if (sendTimer >= 0.1f && TCPClient::GetInstance().IsConnected())
        {
            TCPClient::GetInstance().SendMessage(aiOutput.ToByte());
            sendTimer = 0.0f;
        }
    }

    return 0;
}