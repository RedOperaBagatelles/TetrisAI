#include "Window.h"

#include "Pieces/Piece.h"

Window* Window::instance = nullptr;

void Window::Initialize()
{
    hWnd = static_cast<HWND>(window.getNativeHandle());

    // SFML 창의 Win32 WndProc를 서브클래싱해 키보드 메시지를 직접 수신
    instance = this;
    originalWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WndProc));

    tetris.Initialize();
}

void Window::Update(float deltaTime)
{
    // FPS 측정 및 창 제목 갱신
    float fps = 1.0f / deltaTime;
    window.setTitle("Tetris - FPS: " + std::to_string(static_cast<int>(fps)));

    KeyBoardInput(deltaTime);
	tetris.Update(deltaTime);
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!instance)
        return DefWindowProc(hWnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_KILLFOCUS:
        instance->keyboard.ClearState();
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!(lParam & 0x40000000) || instance->keyboard.AutorepeatIsEnabled())
            instance->keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        instance->keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
        break;

    case WM_CHAR:
        instance->keyboard.OnChar(static_cast<unsigned char>(wParam));
        break;
    }

    return CallWindowProc(instance->originalWndProc, hWnd, msg, wParam, lParam);
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

void Window::KeyBoardInput(float deltaTime)
{
	auto currentPiece = tetris.GetCurrentPiece();

	if (currentPiece == nullptr)
		return;

	// 아래쪽 방향키를 누르고 있는지 확인 (소프트 드롭)
	if (keyboard.IsPressed(VK_DOWN))
		currentPiece->SoftDrop(deltaTime);

	// 키를 뗐을 때 타이머 초기화
	else
		currentPiece->StopSoftDrop();

	// 나머지 키 입력 처리 (일반 키 이벤트)
	while (const auto currentKey = keyboard.ReadKey())
	{
		if (!currentKey->IsPress())
			continue;

		switch (currentKey->GetCode())
		{
			case VK_LEFT:
				currentPiece->Move(MoveDirection::Left);
				printf("←\n");
				break;

			case VK_RIGHT:
				currentPiece->Move(MoveDirection::Right);
				printf("→\n");
				break;

			case VK_UP:
				//currentPiece->Move(MoveDirection::Up);
				printf("↑\n");
				break;

			case VK_DOWN:
				// 아래쪽 키는 이미 위에서 처리하므로 여기서는 무시
				break;

			case VK_SPACE:
				currentPiece->HardDrop();
				printf("Space\n");
				break;
		}
	}
}