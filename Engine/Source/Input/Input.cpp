#include "Input.h"

WPARAM Input::m_mouseState;
Vector2Int Input::m_mousePos;
int Input::m_mouseWheelDelta;
std::array<bool, 256> Input::m_keyDownArray;
std::array<bool, 256> Input::m_keyUpArray;
std::array<bool, 3> Input::m_mouseDownArray;
std::array<bool, 3> Input::m_mouseUpArray;

//-----------------------------------------------------------------------------
// 入力情報解析
//-----------------------------------------------------------------------------
void Input::ParseInputData(UINT message, WPARAM wparam, LPARAM lparam)
{
	int key_code = static_cast<int>(wparam);

	switch (message)
	{
		//キー
	case WM_KEYDOWN: if ((HIWORD(lparam) & KF_REPEAT) == 0) { m_keyDownArray[key_code] = true; } break;//直前のキー状態を参照し 瞬間か判定
	case WM_KEYUP: m_keyUpArray[key_code] = true; break;
		//マウス
	case WM_MOUSEWHEEL: m_mouseWheelDelta = (short)HIWORD(wparam); break;
	case WM_MOUSEMOVE: { m_mouseState = wparam; m_mousePos = Vector2Int(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)); } break;
		//マウスボタン
	case WM_LBUTTONDOWN:	m_mouseDownArray[(int)MouseButton::Left] = true;	break;
	case WM_LBUTTONUP:		m_mouseUpArray[(int)MouseButton::Left] = true;		break;
	case WM_MBUTTONDOWN:	m_mouseDownArray[(int)MouseButton::Middle] = true;	break;
	case WM_MBUTTONUP:		m_mouseUpArray[(int)MouseButton::Middle] = true;	break;
	case WM_RBUTTONDOWN:	m_mouseDownArray[(int)MouseButton::Right] = true;	break;
	case WM_RBUTTONUP:		m_mouseUpArray[(int)MouseButton::Right] = true;		break;
	}
}

//-----------------------------------------------------------------------------
// 入力情報更新
//-----------------------------------------------------------------------------
void Input::Refresh()
{
	m_keyDownArray.fill(false);
	m_keyUpArray.fill(false);
	m_mouseDownArray.fill(false);
	m_mouseUpArray.fill(false);
	m_mouseWheelDelta = 0;
}

//-----------------------------------------------------------------------------
// マウス座標を設定する
//-----------------------------------------------------------------------------
void Input::SetMousePos(const Vector2Int& pos)
{
	//TODO: ClipCursor設定いるかも
	while (SetCursorPos(pos.x, pos.y) != 0) {}
}

//-----------------------------------------------------------------------------
// マウス表示を設定
//-----------------------------------------------------------------------------
void Input::SetMouseHide(bool show)
{
	if (show) { while (ShowCursor(false) >= 0) {}; }
	else { while (ShowCursor(true) < 0) {}; }
}
