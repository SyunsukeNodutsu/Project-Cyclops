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

	//@brief コンストラクタ
	//@param createParam 作成情報
	Window(WINDOE_CREATE_PARAM createParam);

	//@brief デストラクタ
	~Window() = default;

	//@brief 初期化
	//@return 成功...true 失敗...false
	bool Initialize();

	//@brief 終了処理
	void Finalize();

	//@brief メッセージ処理
	//@return false ... WM_QUITメッセージが来た
	bool ProcessMessage();

	//@brief ウィンドウをデスクトップの中心に移動
	//@return 成功...true 失敗...false
	static bool MoveDesktopCenterWindow(HWND hwnd);

	//@brief ウィンドウハンドルを返す
	HWND GetHwnd() const { return m_hWnd; }

private:

	HWND			m_hWnd;			//ウィンドウハンドル
	std::wstring	m_className;	//ウィンドウクラス名
	std::wstring	m_titleName;	//ウィンドウのタイトルバー名
	int				m_clientWidth;	//クライアントサイズ: 幅
	int				m_clientHeight;	//クライアントサイズ: 高さ

private:

	//ウィンドウクラスの登録
	ATOM RegisterWindowClass();
	//ウィンドウプロシージャ
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	//ウィンドウプロシージャ(call back)
	static LRESULT CALLBACK callWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};
