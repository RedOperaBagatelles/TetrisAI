#include "Window.h"
#include <Utility/GameTimer.h>

void Window::Ready()
{
	tetris.Ready(50, 50);
}

void Window::Update()
{
	tetris.Update(*this);
}

bool Window::Render()
{
    if (!window.isOpen())
        return false;

    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
    }

    window.clear();

    for (const auto& renderTarget : renderTargets)
        window.draw(*renderTarget);

    window.display();

	// 이번 프레임에 그릴 객체들을 저장하는 벡터 초기화
    renderTargets.clear();

    // FPS 측정 및 창 제목 갱신
    gameTimer.Tick();
    float fps = 1.0f / gameTimer.DeltaTime();
    window.setTitle("Tetris - FPS: " + std::to_string(static_cast<int>(fps)));

    return true;
}

void Window::AddRenderTarget(const std::shared_ptr<const sf::Drawable>& drawable)
{
    renderTargets.push_back(drawable);
}

void Window::AddRenderTargets(const std::vector<std::shared_ptr<const sf::Drawable>>& drawables)
{
    for (const auto& drawable : drawables)
		renderTargets.push_back(drawable);
}
