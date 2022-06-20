#include "GameDemo.h"

//-----------------------------------------------------------------------------
// サブシステム初期化直後
//-----------------------------------------------------------------------------
void GameDemo::OnStart()
{
	//カメラ
	m_spCamera = std::make_shared<FPSCamera>();
	if (m_spCamera)
	{
		m_spCamera->m_name = "Camera Main";
		m_spCamera->m_priority = 10.0f;
		m_spCamera->SetCameraMatrix(matrix4x4::CreateTranslation(float3(0, 0, -1)));
		m_spCamera->SetLocalPos(float3::Zero);
		m_pCameraManager->AddCameraList(m_spCamera);
	}

	m_profile.Start("Load texture.");
	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Load("../Assets/100Alligator.jpg");
	m_profile.End();

	std::array<std::string, 3> paths = { "../Assets/A.png", "../Assets/B.png" , "../Assets/C.png" };
	m_button.SetInfo(paths, float2(800 - 100, 450 - 50));
}

//-----------------------------------------------------------------------------
// サブシステム終了直前
//-----------------------------------------------------------------------------
void GameDemo::OnEnd()
{
	Debug::Log("終了.");
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameDemo::OnUpdate()
{
	m_spCamera->SetCameraMatrix(matrix4x4::CreateTranslation(0, 0, -10));

	if (m_button.IsPush())
	{
		ShellSystem::RegistBatfile("dir", "test.bat");
		ShellSystem::ExecuteShell(L"test.bat", "D:", ShellExecuteMode::ShowCmdPrompt);

		std::ifstream ifs("../Assets/test.json");
		if (ifs.good())
		{
			nlohmann::json j;
			ifs >> j;

			if (!j["project name"].is_null())
			{
				if (j["project name"].is_string())
				{
					const auto& str = static_cast<std::string>(j["project name"]);
					OutputDebugStringA(std::string("project name: " + str + "\n").c_str());
				}
			}

			const auto& str2 = static_cast<std::string>(j["window"]["name"]);
			OutputDebugStringA(std::string("window name: " + str2 + "\n").c_str());
		}

		ifs.close();
	}

	m_button.Update();
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GameDemo::OnDraw3D()
{
}

//-----------------------------------------------------------------------------
// 2D描画
//-----------------------------------------------------------------------------
void GameDemo::OnDraw2D(SpriteShader& spriteShader)
{
	spriteShader.DrawTexture(m_spTexture.get(), float2( 100, -80), float2(0.2f, 0.2f));
	spriteShader.DrawTexture(m_spTexture.get(), float2( 800, 400), float2(0.2f, 0.2f));
	spriteShader.DrawTexture(m_spTexture.get(), float2(-500, 200), float2(0.2f, 0.2f));

	spriteShader.DrawLine(float2(0, 400), float2(400, 400), color4::Green);

	m_button.Draw();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameDemo::OnLateUpdate()
{
}
