#include "Core/Window.h"

#include "Utility/GameTimer.h"

int main()
{
	Window window;
	window.Initialize();

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