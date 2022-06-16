#include "GameDemo.h"

bool Shell(const std::string& cmd = "svn help")
{
	//コマンドを記憶したbatファイルの作成
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "C:/Windows/Temp/test.bat", "w");
		if (fp)
		{
			fprintf(fp, cmd.c_str());
			fclose(fp);
		}
		else
		{
			OutputDebugStringA("バッチファイルの読み込み失敗.");
			return false;
		}
	}

	//コマンド実行
	{
		SHELLEXECUTEINFO info{ 0 };
		info.cbSize			= sizeof(SHELLEXECUTEINFO);
		info.fMask			= SEE_MASK_NOCLOSEPROCESS;//プロセス終了待つ用
		info.hwnd			= NULL;
		info.lpVerb			= L"open";
		info.lpFile			= L"test.bat";
		info.lpParameters	= L" 1> cmd_ret_log.txt 2> cmd_err_log.txt";
		info.lpDirectory	= L"C:/Windows/Temp/";
		info.nShow			= SW_HIDE;
		info.hInstApp		= NULL;

		if (ShellExecuteEx(&info) == 0 || (int)(info.hInstApp) <= 32)
		{
			OutputDebugStringA("ShellExecuteEx失敗.");
			return false;
		}

		//TODO: 拡張エラー情報(WAIT_OBJECT_0までループとか？)
		WaitForSingleObject(info.hProcess, INFINITE);

		if (CloseHandle(info.hProcess) == 0)
		{
			OutputDebugStringA("CloseHandle失敗.");
			return false;
		}

		//バッチファイルの削除
		if (DeleteFileA("C:/Windows/Temp/test.bat") == 0)
		{
			OutputDebugStringA("DeleteFileA失敗.");
			return false;
		}
	}

	//コマンドの実行結果をファイルで取得
	//通常ログ
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "C:/Windows/Temp/cmd_ret_log.txt", "r");
		if (fp)
		{
			OutputDebugStringA("\n正常ログここから  ------------------->\n");

			char line[128] = "";
			while (fgets(line, _countof(line), fp) != NULL)
				OutputDebugStringA(line);//TODO: とりあえず出力に表示

			OutputDebugStringA("\n<---------------------正常ログここまで\n");

			fclose(fp);
		}
		else
		{
			OutputDebugStringA("ログファイルの読み込み失敗.");
			return false;
		}

		//ログファイル削除
		if (DeleteFileA("C:/Windows/Temp/cmd_ret_log.txt") == 0)
		{
			OutputDebugStringA("DeleteFileA失敗.");
			return false;
		}
	}
	//エラーログ
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "C:/Windows/Temp/cmd_err_log.txt", "r");
		if (fp)
		{
			OutputDebugStringA("\nエラーログここから------------------->\n");

			char line[128] = "";
			while (fgets(line, _countof(line), fp) != NULL)
				OutputDebugStringA(line);//TODO: とりあえず出力に表示

			OutputDebugStringA("\n<-------------------エラーログここまで\n");

			fclose(fp);
		}
		else
		{
			OutputDebugStringA("ログファイルの読み込み失敗.");
			return false;
		}

		//ログファイル削除
		if (DeleteFileA("C:/Windows/Temp/cmd_err_log.txt") == 0)
		{
			OutputDebugStringA("DeleteFileA失敗.");
			return false;
		}
	}

	return true;
}

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
		Shell("svn commit");

		std::shared_ptr<Sound> new_sound = std::make_shared<Sound>("../Assets/Enter.wav", false, false);
		m_pAudioDevice->SetFadeSoundList(1.0f, 0.1f);
		if (new_sound) { new_sound->Play(); m_pAudioDevice->AddSound(new_sound); }
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
