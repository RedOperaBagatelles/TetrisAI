#include "Core/Window.h"

int main()
{
	Window window;
	window.Ready();
    
    while (true)
    {
        window.Update();
        window.Render();
    }

    return 0;
}