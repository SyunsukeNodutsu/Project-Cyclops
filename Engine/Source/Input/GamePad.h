//-----------------------------------------------------------------------------
// File: GamePad.cpp
//
// XInput対応のコントローラ操作
// TODO: 思考 現在はクラスひとつにつき4つのコントローラーを管理する設計
//-----------------------------------------------------------------------------
#pragma once

//ゲームパッドクラス
class GamePad
{
public:

	GamePad() = default;
	~GamePad() = default;

	static void Update();

	static Vector2 Left(int index = 0);
	static Vector2 Right(int index = 0);

private:

	static constexpr int GAMEPAD_MAX = 4;
	static constexpr float GAMEPAD_DEADZONE = 7849.0f;

	static XINPUT_STATE	m_state[GAMEPAD_MAX];	//コントローラの状態
	static bool m_connected[GAMEPAD_MAX];		//コントローラが接続されているか

private:

	static Vector2 GetFinalStickTilt(const Vector2& stick, int index);

};
