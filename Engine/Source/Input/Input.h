//-----------------------------------------------------------------------------
// File: Input.h
//
// 入力 対応予定の入力機器(Keyboard,Mouse,XInput,RowInput)
//-----------------------------------------------------------------------------
#pragma once

#pragma region enums
//仮想キーコード
enum class KeyCode : UINT
{
	Backspace	= 8,
	Tab			= 9,
	Enter		= 13,
	Shift		= 16,
	Ctrl		= 17,
	Alt			= 18,
	Pause		= 19,
	CapsLock	= 20,
	ESC			= 27,
	Space		= 32,
	PageUp		= 33,
	PageDown	= 34,
	End			= 35,
	Home		= 36,
	LeftArrow	= 37,
	UpArrow		= 38,
	RightArrow	= 39,
	DownArrow	= 40,
	SnapShot	= 44,
	Insert		= 45,
	Delete		= 46,

	Keyboard0 = 48,
	Keyboard1 = 49,
	Keyboard2 = 50,
	Keyboard3 = 51,
	Keyboard4 = 52,
	Keyboard5 = 53,
	Keyboard6 = 54,
	Keyboard7 = 55,
	Keyboard8 = 56,
	Keyboard9 = 57,

	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,

	LWindows = 91,
	RWindows = 92,

	Application = 93,

	Numpad0 =  96,
	Numpad1 =  97,
	Numpad2 =  98,
	Numpad3 =  99,
	Numpad4 = 100,
	Numpad5 = 101,
	Numpad6 = 102,
	Numpad7 = 103,
	Numpad8 = 104,
	Numpad9 = 105
};

//仮想マウスボタン TOOD: 拡張ボタン対応
enum class MouseButton
{
	Left = 0,
	Middle,
	Right,
};
#pragma endregion

//入力クラス
class Input : public ISubsystem
{
public:

	Input() = default;
	~Input() = default;

	static void ParseInputData(UINT message, WPARAM wparam, LPARAM lparam);//入力情報解析
	static void Refresh();//入力情報更新 ゲームループの最後に呼び出す
	static void SetWindowHwnd(HWND hwnd) { m_hwnd = hwnd; }

	static bool IsKeyDown(KeyCode keyCode) { return m_keyDownArray[(int)keyCode]; }
	static bool IsKeyPressed(KeyCode keyCode) { return GetAsyncKeyState((int)keyCode); }
	static bool IsKeyUp(KeyCode keyCode) { return m_keyUpArray[(int)keyCode]; }

	static bool IsMouseDown(MouseButton mbutton) { return m_mouseDownArray[(int)mbutton]; }
	static bool IsMousePressed(MouseButton mbutton) { return m_mousePressArray[(int)mbutton]; }
	static bool IsMouseUp(MouseButton mbutton) { return m_mouseUpArray[(int)mbutton]; }

	static float2 GetMousePos() { return m_mousePos; }
	static int GetMouseWheelDelta();

	static void SetMousePos(const float2& pos);
	static void SetMouseHide(bool show);

private:

	static WPARAM m_mouseState;
	static float2 m_mousePos;
	static int m_mouseWheelDelta;

	static std::array<bool, 256> m_keyDownArray;
	static std::array<bool, 256> m_keyUpArray;
	static std::array<bool, 3> m_mouseDownArray;
	static std::array<bool, 3> m_mouseUpArray;
	static std::array<bool, 3> m_mousePressArray;

};
