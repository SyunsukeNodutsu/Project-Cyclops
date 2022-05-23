//-----------------------------------------------------------------------------
// File: GraphicsDevice.h
//
// エンジンのルート機能 サブシステムの生成、ゲームループの定義を行う
// アプリケーションに適切なコールバック関数を渡す 例)OnUpdate()みたいな
//-----------------------------------------------------------------------------
#pragma once

//エンジンのルート(スーパー)クラス
class CyclopsRoot
{
public:

	CyclopsRoot() = default;
	~CyclopsRoot() = default;

	virtual void OnAwake() = 0;//サブシステム初期化前
	virtual void OnStart() = 0;//サブシステム初期化後

	virtual void OnUpdate() = 0;//更新
	virtual void OnDraw3D() = 0;//3D描画
	virtual void OnDraw2D() = 0;//2D描画
	virtual void OnLateUpdate() = 0;//描画後更新

protected:

	static Window*			m_pWindow;
	static FpsTimer*		m_pFpsTimer;

	static GraphicsDevice*	m_pGraphicsDevice;
	static AudioDevice*		m_pAudioDevice;
	static ImGuiProfile*	m_pImGuiProfile;

};
