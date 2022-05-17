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

	GamePad();
	~GamePad() = default;

	void Update();

	Vector2 Left(int index = 0);
	Vector2 Right(int index = 0);

private:

	static constexpr int GAMEPAD_MAX = 4;
	static constexpr float GAMEPAD_DEADZONE = 7849.0f;

	XINPUT_STATE	m_state[GAMEPAD_MAX];		//コントローラの状態
	bool			m_connected[GAMEPAD_MAX];	//コントローラが接続されているか

private:

	Vector2 GetFinalStickTilt(const Vector2& stick, int index);

};
