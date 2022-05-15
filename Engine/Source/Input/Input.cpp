#include "Input.h"

std::array<bool, 256> Input::m_keyDownArray;
std::array<bool, 256> Input::m_keyUpArray;

//-----------------------------------------------------------------------------
// 入力情報解析
//-----------------------------------------------------------------------------
void Input::ParseInputData(UINT message, WPARAM wparam, LPARAM lparam)
{
	int key_code = static_cast<int>(wparam);

	switch (message)
	{
	case WM_KEYUP:
		Input::m_keyUpArray[key_code] = true;
		break;

	case WM_KEYDOWN:
		//直前のキー状態を参照し 瞬間か判定
		if ((HIWORD(lparam) & KF_REPEAT) == 0)
			Input::m_keyDownArray[key_code] = true;
		break;
	}
}

//-----------------------------------------------------------------------------
// 入力情報更新
//-----------------------------------------------------------------------------
void Input::Refresh()
{
	m_keyDownArray.fill(false);
	m_keyUpArray.fill(false);
}
