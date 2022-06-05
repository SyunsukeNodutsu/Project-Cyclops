#include "Button2D.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Button2D::Button2D()
	: m_buttonState(ButtonState::None)
	, m_buttonStatePrev(ButtonState::None)
	, m_textures()
	, m_position(float2::Zero)
	, m_size(float2::Zero)
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Button2D::Update()
{
	const float2& mousePos = Input::GetMousePos();
	
	//SpriteShaderは中心基準なので 左上基準に修正
	//TODO: ウィンドウサイズ定数
	const float width = 1600;
	const float height = 900;

	//解像度に応じて拡縮 ※TODO: クライアントサイズではなくウィンドウサイズ
	const float rateX = m_graphicsDevice->m_viewport.Width / width;
	const float rateY = m_graphicsDevice->m_viewport.Height / height;
	const float2 texturePos = float2((m_position.x + (width * 0.5f)) * rateX, (m_position.y * -1 + (height * 0.5f)) * rateY);

	m_buttonStatePrev = m_buttonState;
	m_buttonState = CheckState(texturePos, mousePos);
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Button2D::Draw()
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_spShaderManager == nullptr) return;

	auto& spriteShader = m_graphicsDevice->m_spShaderManager->m_spriteShader;
	spriteShader.DrawTexture(m_textures[(int)m_buttonState].get(), m_position);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Button2D::IsPush()
{
	if (m_buttonStatePrev == ButtonState::Push) return false;
	return m_buttonState == ButtonState::Push;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Button2D::IsChoice()
{
	if (m_buttonStatePrev == ButtonState::Choice) return false;
	return m_buttonState == ButtonState::Choice;
}

//-----------------------------------------------------------------------------
// 情報設定
//-----------------------------------------------------------------------------
void Button2D::SetInfo(const std::array<std::string, 3> texturePaths, const float2 pos)
{
	for (auto& texture : m_textures)
		texture = std::make_shared<Texture>();

	m_textures[0]->Load(texturePaths[0]);
	m_textures[1]->Load(texturePaths[1]);
	m_textures[2]->Load(texturePaths[2]);

	m_position = pos;
	m_size = float2((float)m_textures[0]->GetWidth(), (float)m_textures[0]->GetHeight());
}

//-----------------------------------------------------------------------------
// ステートの確認更新
//-----------------------------------------------------------------------------
const Button2D::ButtonState Button2D::CheckState(const float2 centerPos, const float2 mousePos)
{
	//if (!(APP.m_window.GetWinInfo().dwWindowStatus & WS_ACTIVECAPTION)) { return ButtonState::eNone; }
	
	//マウスカーソルがボタンの上に存在
	if (mousePos.x <= centerPos.x + (m_size.x * 0.5f) && mousePos.x >= centerPos.x - (m_size.x * 0.5f) &&
		mousePos.y <= centerPos.y + (m_size.y * 0.5f) && mousePos.y >= centerPos.y - (m_size.y * 0.5f))
	{
		//TODO: 左or右
		if (Input::IsMousePressed(MouseButton::Left))
		{
			return ButtonState::Push;
		}
		else return ButtonState::Choice;
	}
	return ButtonState::None;
}
