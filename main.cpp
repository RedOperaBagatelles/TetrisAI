#include "Core/Window.h"

int main()
{
	Window window;
	window.Initialize();

    bool isSucceed = true;

    while (isSucceed)
    {
        window.Update();
        isSucceed = window.Render();
    }

    return 0;
}