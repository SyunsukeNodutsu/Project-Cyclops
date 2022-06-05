﻿#include "Window.h"

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

HWND Window::m_hWnd = nullptr;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Window::Window(WINDOE_CREATE_PARAM createParam)
	: m_hInstance(nullptr)
	, m_className(createParam.ClassName)
	, m_titleName(createParam.TitleName)
	, m_clientWidth(createParam.ClientWidth)
	, m_clientHeight(createParam.ClientHeight)
	, m_windowWidth(0)
	, m_windowHeight(0)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool Window::Initialize()
{
	if (RegisterWindowClass() == 0)
		return false;

	if (CreateWindowInstance() == false)
		return false;

	SetClientSize(m_clientWidth, m_clientHeight);
	MoveDesktopCenterWindow(m_hWnd);

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	/*
	UINT timer_resolution = 1;
	if (timeBeginPeriod(timer_resolution) != TIMERR_NOERROR)
		return false;
	*/

	return true;
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void Window::Finalize()
{
	if (m_hWnd == nullptr) return;

	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;
	UnregisterClass(m_className.c_str(), m_hInstance);
}

//-----------------------------------------------------------------------------
// メッセージ処理
//-----------------------------------------------------------------------------
bool Window::ProcessMessage()
{
	if (m_hWnd == nullptr) return false;

	//入力情報更新
	//TODO: OnXXX()のようなイベント関数を取得し、InputDevice側で処理を行う
	// そうすればstatic/publicにしなくて済む
	Input::Refresh();
	GamePad::Update();

	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Window::SetClientSize(int width, int height)
{
	RECT rcWindow, rcClient;

	GetWindowRect(m_hWnd, &rcWindow);
	GetClientRect(m_hWnd, &rcClient);

	int nWidth = width + (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
	int nHeight = height + (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);

	//ウィンドウの余白を考え クライアントのサイズを指定サイズに設定
	MoveWindow(m_hWnd, rcWindow.left, rcWindow.top, nWidth, nHeight, TRUE);
}

//-----------------------------------------------------------------------------
// ウィンドウをデスクトップの中心に移動
//-----------------------------------------------------------------------------
bool Window::MoveDesktopCenterWindow(HWND hwnd)
{
	//デスクトップサイズ取得
	RECT desktopRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, 0);

	//ウィンドウ情報取得
	WINDOWINFO winfo;
	winfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(hwnd, &winfo);

	//矩形取得
	RECT windowRect = winfo.rcWindow;
	long width = windowRect.right - windowRect.left;
	long height = windowRect.bottom - windowRect.top;

	//座標確定
	INT posX = (((desktopRect.right - desktopRect.left) - width)  / 2 + desktopRect.left);
	INT posY = (((desktopRect.bottom - desktopRect.top) - height) / 2 + desktopRect.top);

	return SetWindowPos(hwnd, nullptr, posX, posY, 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER));
}

//-----------------------------------------------------------------------------
// ウィンドウクラスの登録
//-----------------------------------------------------------------------------
ATOM Window::RegisterWindowClass()
{
	WNDCLASSEXW wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= &Window::callWindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= GetModuleHandle(0);
	wc.hIcon			= LoadIcon(wc.hInstance, nullptr);//TODO: アイコン設定したい
	wc.hIconSm			= LoadIcon(wc.hInstance, nullptr);
	wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= m_className.c_str();

	return RegisterClassExW(&wc);
}

//-----------------------------------------------------------------------------
// ウィンドウ作成しウィンドウハンドルを取得する
//-----------------------------------------------------------------------------
bool Window::CreateWindowInstance()
{
	m_hInstance = GetModuleHandle(0);

	DWORD style = WS_OVERLAPPEDWINDOW;// WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME/* & ~WS_MAXIMIZEBOX*/;

	m_hWnd = CreateWindowW(
		m_className.c_str(), m_titleName.c_str(),
		style, CW_USEDEFAULT, 0,
		m_clientWidth, m_clientHeight,
		nullptr, nullptr, m_hInstance, this);

	return (m_hWnd != nullptr);
}

//-----------------------------------------------------------------------------
// ウィンドウ関数
//-----------------------------------------------------------------------------
LRESULT Window::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	//ImGuiにイベント通知
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return true;

	//入力情報解析
	Input::ParseInputData(message, wparam, lparam);

	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		//HDC使わん
		EndPaint(hwnd, &ps);
		break;
	}

	case WM_SIZE:
		if (wparam != SIZE_MINIMIZED)
		{
			if (GraphicsDeviceChild::GetDevice())
				GraphicsDeviceChild::GetDevice()->Resize(wparam, LOWORD(lparam), HIWORD(lparam));

			ApplicationBase::m_clientSize.x = LOWORD(lparam);
			ApplicationBase::m_clientSize.y = HIWORD(lparam);
		}
		break;

	case WM_CLOSE:
		Finalize();
		break;

	case WM_DESTROY:
		RemoveProp(hwnd, L"WindowInstance");
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// ウィンドウ関数(Static関数)
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window::callWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//ウィンドウプロパティから、GameWindowクラスのインスタンスを取得
	Window* instance = (Window*)GetProp(hWnd, L"WindowInstance");

	//nullptrの場合は、デフォルト処理を実行
	if (instance == nullptr)
	{
		switch (message)
		{
		case WM_CREATE:
		{
			//CreateWindow()で渡したパラメータを取得
			CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
			Window* window = (Window*)createStruct->lpCreateParams;

			//ウィンドウプロパティにこのクラスのインスタンスアドレスを埋め込んでおく
			//次回から、instance->WindowProcの方へ処理が流れていく
			SetProp(hWnd, L"WindowInstance", window);
		}
		return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	//インスタンス側のWindow関数を実行する
	return instance->WindowProc(hWnd, message, wParam, lParam);
}
