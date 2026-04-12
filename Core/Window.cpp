#include "Window.h"

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
