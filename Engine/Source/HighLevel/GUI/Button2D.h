//-----------------------------------------------------------------------------
// File: Button2D.h
//
// 2Dのボタンクラス
//-----------------------------------------------------------------------------
#pragma once

//2Dのボタンクラス
class Button2D : public GraphicsDeviceChild
{
public:

	//ボタン情報
	enum class ButtonState
	{
		None,	//なにもしない
		Choice,	//ボタン上にマウスカーソルが存在
		Push	//ボタン上にマウスカーソルが存在 なおかつ押された
	};

public:

	Button2D();
	~Button2D() = default;

	void Update();
	void Draw();

	bool IsPush();
	bool IsChoice();

	void SetInfo(const std::array<std::string, 3> texturePaths, const float2 pos);

private:

	ButtonState								m_buttonState;
	ButtonState								m_buttonStatePrev;
	std::array<std::shared_ptr<Texture>, 3> m_textures;
	float2									m_position;
	float2									m_size;

private:

	const ButtonState CheckState(const float2 centerPos, const float2 mousePos);

};
