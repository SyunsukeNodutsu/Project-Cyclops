//-----------------------------------------------------------------------------
// File: Window.h
//
// ウィンドウ
//-----------------------------------------------------------------------------
#pragma once

//ウィンドウ作成情報
typedef struct WINDOE_CREATE_PARAM
{
	std::wstring TitleName = L"TitleName";
	std::wstring ClassName = L"ClassName";
	int ClientWidth = 1280;
	int ClientHeight = 720;
}WINDOE_CREATE_PARAM;

//ウィンドウクラス
class Window
{
public:

	Window(WINDOE_CREATE_PARAM createParam);
	~Window() = default;

	bool Initialize();
	void Finalize();
	bool ProcessMessage();
	void SetClientSize(int width, int height);
	bool MoveDesktopCenterWindow(HWND hwnd);

	HWND GetHwnd() const { return m_hWnd; }

private:

	HWND			m_hWnd;			//ウィンドウハンドル
	HINSTANCE		m_hInstance;
	std::wstring	m_className;	//ウィンドウクラス名
	std::wstring	m_titleName;	//ウィンドウのタイトルバー名
	int				m_clientWidth;	//クライアントサイズ: 幅
	int				m_clientHeight;	//クライアントサイズ: 高さ

private:

	ATOM RegisterWindowClass();
	bool CreateWindowInstance();
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK callWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};
