//-----------------------------------------------------------------------------
// File: GraphicsDevice.h
//
// エンジンのルート機能 TODO: nullチェック
//-----------------------------------------------------------------------------
#pragma once

//抽象基底アプリケーション
class ApplicationBase
{
public:

	virtual ~ApplicationBase() = default;

	virtual void OnStart() = 0;//サブシステム初期化直後
	virtual void OnEnd() = 0;//サブシステム終了直前

	virtual void OnUpdate() = 0;//更新
	virtual void OnDraw3D() = 0;//3D描画
	virtual void OnDraw2D() = 0;//2D描画
	virtual void OnLateUpdate() = 0;//描画後更新

	//TODO: 隠ぺい
	static void SetGraphicsDevice(GraphicsDevice* pDevice) { m_pGraphicsDevice = pDevice; }
	static void SetAudioDevice(AudioDevice* pDevice) { m_pAudioDevice = pDevice; }
	static GraphicsDevice* m_pGraphicsDevice;
	static AudioDevice* m_pAudioDevice;

};

//エンジンのルート(スーパー)クラス
class CyclopsRoot
{
public:

	CyclopsRoot(ApplicationBase* app);
	~CyclopsRoot() = default;

	void Run();

private:

	ApplicationBase*		m_pApplicationBase;
	int						m_windowWidth;
	int						m_windowHeight;

	static Window*			m_pWindow;
	static FpsTimer*		m_pFpsTimer;

	static GraphicsDevice*	m_pGraphicsDevice;
	static AudioDevice*		m_pAudioDevice;
	static ImGuiProfile*	m_pImGuiProfile;

private:

	void Initialize();
	void Finalize();
	void Update();
	void Draw();
	void LateUpdate();

};
