#include "GamePad.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GamePad::GamePad()
    : m_state()
    , m_connected()
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GamePad::Update()
{
    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
    {
        ZeroMemory(&m_state[i], sizeof(XINPUT_STATE));

        //コントローラの状態を取得
        m_connected[i] = (XInputGetState(i, &m_state[i]) == ERROR_SUCCESS);
    }
}

//-----------------------------------------------------------------------------
// 左スティックの入力を取得
//-----------------------------------------------------------------------------
Vector2 GamePad::Left(int index)
{
    index = std::clamp(index, 0, GAMEPAD_MAX - 1);
    if (!m_connected[index]) return Vector2(FLT_MIN);
    return GetFinalStickTilt(Vector2(m_state[index].Gamepad.sThumbLX, m_state[index].Gamepad.sThumbLY), index);
}

//-----------------------------------------------------------------------------
// 右スティックの入力を取得
//-----------------------------------------------------------------------------
Vector2 GamePad::Right(int index)
{
    index = std::clamp(index, 0, GAMEPAD_MAX - 1);
    if (!m_connected[index]) return Vector2(FLT_MIN);
    return GetFinalStickTilt(Vector2(m_state[index].Gamepad.sThumbRX, m_state[index].Gamepad.sThumbRY), index);
}

//-----------------------------------------------------------------------------
// 最終的なスティックの傾きを返す ※正規化&デッドゾーンを加味
//-----------------------------------------------------------------------------
Vector2 GamePad::GetFinalStickTilt(const Vector2& stick, int index)
{
    //傾き
    float magnitude = sqrt(stick.x * stick.x + stick.y * stick.y);
    float magnitude_normalized = 0.0f;

    Vector2 result = stick / magnitude;

    //円形のデッドゾーンの外
    if (magnitude > GAMEPAD_DEADZONE)
    {
        magnitude = std::clamp(magnitude, -32767.0f, 32767.0f);
        magnitude_normalized = magnitude / 32767.0f;
    }
    //デッドゾーンの中
    else
    {
        magnitude = magnitude_normalized = 0.0f;
    }

    return (result * magnitude_normalized);
}
