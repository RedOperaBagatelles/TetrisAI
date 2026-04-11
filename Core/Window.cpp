#include "Window.h"

void Window::Ready()
{
	tetris.Ready(0, 0);
}

void Window::Update()
{
	tetris.Update(*this);
}

void Window::Render()
{
    if (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();

        for (const auto& renderTarget : renderTargets)
			window.draw(*renderTarget);

        window.display();
    }
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
