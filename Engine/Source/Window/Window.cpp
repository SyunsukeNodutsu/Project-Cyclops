#include "Window.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Window::Window(WINDOE_CREATE_PARAM createParam)
	: m_hWnd(nullptr)
	, m_className(createParam.ClassName)
	, m_titleName(createParam.TitleName)
	, m_clientWidth(createParam.ClientWidth)
	, m_clientHeight(createParam.ClientHeight)
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
}

//-----------------------------------------------------------------------------
// メッセージ処理
//-----------------------------------------------------------------------------
bool Window::ProcessMessage()
{
	if (m_hWnd == nullptr) return false;

	//入力情報更新
	//TODO: OnXXX()のようなイベント関数を取得し、InputDevice側で処理を行う
	// そうすればstatic/pubkicにしなくて済む
	Input::Refresh();

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
	HINSTANCE hInstance = GetModuleHandle(0);

	DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME/* & ~WS_MAXIMIZEBOX*/;

	m_hWnd = CreateWindowW(
		m_className.c_str(), m_titleName.c_str(),
		style, CW_USEDEFAULT, 0,
		m_clientWidth, m_clientHeight,
		nullptr, nullptr, hInstance, this);

	return (m_hWnd != nullptr);
}

//-----------------------------------------------------------------------------
// ウィンドウ関数
//-----------------------------------------------------------------------------
LRESULT Window::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	//入力情報解析
	Input::ParseInputData(message, wparam, lparam);

	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		static TCHAR szText[] = TEXT("表示テスト.");
		TextOut(hdc, 0, 0, szText, lstrlen(szText));
		SetTextColor(hdc, RGB(0, 255, 255));
		//SetBkColor(hdc, RGB(255, 0, 0));

		EndPaint(hwnd, &ps);
		break;
	}

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
