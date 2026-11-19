#include "Window.h"

#include "Pieces/Piece.h"

#include <algorithm>

Window* Window::instance = nullptr;

void Window::Initialize()
{
    hWnd = static_cast<HWND>(window.getNativeHandle());

    // SFML 창의 Win32 WndProc를 서브클래싱해 키보드 메시지를 직접 수신
    instance = this;
    originalWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WndProc));
}

void Window::Update(float deltaTime)
{
    // FPS 측정 및 창 제목 갱신
    float fps = 1.0f / deltaTime;
    window.setTitle("Tetris - FPS: " + std::to_string(static_cast<int>(fps)));

    KeyBoardInput(deltaTime);
    for (const auto& game : games)
        game->Update(deltaTime);
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

std::vector<std::reference_wrapper<Tetris>> Window::CreateTetris(int count)
{
    std::vector<std::reference_wrapper<Tetris>> createdGames;

    if (count <= 0)
        return createdGames;

    // 창 공간을 가장 잘 활용하는 열 개수를 찾음
    // 열 수를 1~N까지 바꿔 보며 보드가 가장 크게 보이는(스케일이 가장 큰) 배치를 선택한다.
    int bestColumns = 1;
    float bestScale = 0.0f;

    for (int columns = 1; columns <= count; ++columns)
    {
        const int rows = (count + columns - 1) / columns;
        const float contentWidth = static_cast<float>(columns * gameAreaPixelWidth + (columns - 1) * horizontalGap + 2 * margin);
        const float contentHeight = static_cast<float>(rows * gameAreaPixelHeight + (rows - 1) * verticalGap + 2 * margin);
        const float scale = std::min(static_cast<float>(width) / contentWidth, static_cast<float>(height) / contentHeight);

		// 현재 열 수에서의 스케일이 이전까지의 최적 스케일보다 크면 최적 배치를 갱신
        if (scale > bestScale)
        {
            bestScale = scale;
            bestColumns = columns;
        }
    }

    // 선택된 배치에 따라 보드를 생성
    createdGames.reserve(count);

    for (int i = 0; i < count; ++i)
    {
		// 현재 보드의 위치 계산
        const int x = margin + (i % bestColumns) * (gameAreaPixelWidth + horizontalGap);
        const int y = margin + (i / bestColumns) * (gameAreaPixelHeight + verticalGap);

        createdGames.push_back(CreateTetris(x, y));
    }

    // 선택된 배치의 전체 보드가 한 창 안에 보이도록 뷰를 조정
    FitGamesToWindow();

    return createdGames;
}

Tetris& Window::CreateTetris(int x, int y)
{
    auto game = std::make_unique<Tetris>(*this, x, y);
    game->Initialize();
    games.push_back(std::move(game));

    return *games.back();
}

void Window::FitGamesToWindow()
{
    if (games.empty())
    {
        window.setView(window.getDefaultView());
        return;
    }

    float left = static_cast<float>(games.front()->GetStartX());
    float top = static_cast<float>(games.front()->GetStartY());
    float right = left + gameAreaPixelWidth;
    float bottom = top + gameAreaPixelHeight;

    for (const auto& game : games)
    {
        left = std::min(left, static_cast<float>(game->GetStartX()));
        top = std::min(top, static_cast<float>(game->GetStartY()));
        right = std::max(right, static_cast<float>(game->GetStartX()) + gameAreaPixelWidth);
        bottom = std::max(bottom, static_cast<float>(game->GetStartY()) + gameAreaPixelHeight);
    }

    // 여백을 포함한 게임 영역 전체가 창 안에 들어오도록 스케일 계산
    left -= margin;
    top -= margin;
    right += margin;
    bottom += margin;

    const float contentWidth = right - left;
    const float contentHeight = bottom - top;
    const float scale = std::min(static_cast<float>(width) / contentWidth,
                                 static_cast<float>(height) / contentHeight);

    sf::View view;
    view.setCenter({ (left + right) / 2.0f, (top + bottom) / 2.0f });
    view.setSize({ static_cast<float>(width) / scale, static_cast<float>(height) / scale });
    window.setView(view);
}

void Window::KeyBoardInput(float deltaTime)
{
	if (games.empty())
		return;

	Tetris& activeGame = *games.front();
	auto currentPiece = activeGame.GetCurrentPiece();

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
				activeGame.HoldPiece();
				printf("Hold\n");
				break;

			case VK_DOWN:
				// 아래쪽 키는 이미 위에서 처리하므로 여기서는 무시
				break;

			case VK_SPACE:
				currentPiece->HardDrop();
				printf("Space\n");
				break;

			// 홀드
			case 'C':
				activeGame.HoldPiece();
				printf("Hold\n");
				break;

            // 반시계 방향 회전
            case 'Z':
                currentPiece->Rotate(false);
                printf("Z\n");
				break;

            // 시계 방향 회전
            case 'X':
                currentPiece->Rotate(true);	
				printf("X\n");
                break;
		}
	}
}
