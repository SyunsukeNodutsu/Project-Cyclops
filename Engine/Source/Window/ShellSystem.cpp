#include "ShellSystem.h"

const std::string_view ShellSystem::m_stdRedirectFiepath = "C:/Windows/Temp/CmdLogStd.txt";
const std::string_view ShellSystem::m_errRedirectFiepath = "C:/Windows/Temp/CmdLogErr.txt";
const std::string_view ShellSystem::m_operationDir = "C:/Windows/Temp/";
std::list<std::string> ShellSystem::m_batfileList;

//-----------------------------------------------------------------------------
// バッチファイルの作成
//-----------------------------------------------------------------------------
bool ShellSystem::RegistBatfile(std::string_view cmd, std::string_view batFilename, bool addList)
{
	std::string filepath = m_operationDir.data();
	filepath.append(batFilename);

	FILE* fp = nullptr;
	fopen_s(&fp, filepath.c_str(), "w");
	if (fp)
	{
		fprintf(fp, cmd.data());
		fclose(fp);

		if (addList) m_batfileList.push_back(filepath);

		return true;
	}
	else
	{
		Debug::LogError("バッチファイルの読み込み失敗.");
		return false;
	}
}

//-----------------------------------------------------------------------------
// 外部アプリケーションの実行
//-----------------------------------------------------------------------------
bool ShellSystem::ExecuteShell(std::string_view batFilename, std::string_view dir, ShellExecuteMode exeMode)
{
	ChangeExecuteDirectory(batFilename, dir, exeMode);

	//なぜか"info.lpFile"に直接渡すと文字化けする(□□□...)
	const std::wstring& lpFileTmp = sjis_to_wide(batFilename.data());

	SHELLEXECUTEINFO info{ 0 };
	info.cbSize			= sizeof(SHELLEXECUTEINFO);
	info.fMask			= SEE_MASK_NOCLOSEPROCESS;
	info.hwnd			= NULL;
	info.lpVerb			= L"open";
	info.lpFile			= lpFileTmp.c_str();
	info.lpParameters	= NULL;
	info.lpDirectory	= L"C:/Windows/Temp/";
	info.nShow			= SW_HIDE;
	info.hInstApp		= NULL;

	//実行Modeによってパラメータ設定
	if (exeMode == ShellExecuteMode::HideRedirect) info.lpParameters = L"1> CmdLogStd.txt 2> CmdLogErr.txt";
	if (exeMode == ShellExecuteMode::ShowCmdPrompt) info.nShow = SW_SHOWNORMAL;

	if (ShellExecuteEx(&info) == 0 || reinterpret_cast<unsigned long long>(info.hInstApp) <= 32)
	{
		CloseHandle(info.hProcess);
		return false;
	}

	WaitForSingleObject(info.hProcess, INFINITE);

	if (CloseHandle(info.hProcess) == 0)
	{
		Debug::LogError("CloseHandle失敗."); return false;
	}

	Debug::Log("Done ExecuteShell.");
	return true;
}

//-----------------------------------------------------------------------------
// このアプリケーションで作成したbatファイルをすべて削除する
//-----------------------------------------------------------------------------
void ShellSystem::DeleteBatFileAll()
{
	if (m_batfileList.size() == 0)
	{
		Debug::Log("このアプリケーションで作成したbatファイルはありませんでした."); return;
	}

	for (const auto& batfile : m_batfileList)
	{
		DeleteFileA(batfile.c_str());
	}
}

//-----------------------------------------------------------------------------
// batファイルの実行ディレクトリを変更
//-----------------------------------------------------------------------------
bool ShellSystem::ChangeExecuteDirectory(std::string_view batFilename, std::string_view dir, ShellExecuteMode exeMode)
{
	std::string filepath = m_operationDir.data();
	filepath.append(batFilename.data());

	//既存のバッチファイルを読み込み ->変数に保存
	char batcmd[256] = "";
	FILE* fp = nullptr;
	fopen_s(&fp, filepath.c_str(), "r");
	if (fp)
	{
		//TODO: バッチファイルの作成の際に 複数行を許可しない
		while (fgets(batcmd, 256, fp) != NULL);
		fclose(fp);
	}
	else
	{
		Debug::LogError("バッチファイルの読み込み失敗.");
		return false;
	}

	//ディレクトリ移動/終了後のふるまいを設定して再作成
	std::string cmd = "cd /d ";
	cmd.append(dir);
	cmd.append((exeMode == ShellExecuteMode::ShowCmdPrompt) ? " & cmd /K " : " & cmd /C ");
	cmd.append(batcmd);

	return RegistBatfile(cmd, batFilename.data(), false);
}

/*
■リダイレクトモードで実行したバッチファイルの出力受け取り例
FILE* fp = nullptr;
fopen_s(&fp, ShellSystem::m_stdRedirectFiepath.data(), "r");
if (fp)
{
	char line[256] = "";
	while (fgets(line, _countof(line), fp) != NULL)
	{
		OutputDebugStringA(line);
		ImGuiProfile::AddLog(line);
	}
	fclose(fp);
}
else
{
	Debug::LogError("ログファイルの読み込み失敗.");
}
*/