#include "ShellSystem.h"

const std::string		ShellSystem::m_operationDir		=  "C:/Windows/Temp/";
const std::wstring		ShellSystem::m_operationDirW	= L"C:/Windows/Temp/";
std::list<std::string>	ShellSystem::m_batfileList;

//-----------------------------------------------------------------------------
// バッチファイルの作成
//-----------------------------------------------------------------------------
bool ShellSystem::MakeBatfile(std::string_view cmd, std::string_view batFilename)
{
	FILE* fp = nullptr;
	fopen_s(&fp, std::string(m_operationDir + batFilename.data()).c_str(), "w");
	if (fp)
	{
		fprintf(fp, cmd.data());
		fclose(fp);

		m_batfileList.push_back(std::string(m_operationDir + batFilename.data()));

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
bool ShellSystem::ExecuteShell(std::wstring_view batFilename, bool showLog, bool showWindow)
{
	SHELLEXECUTEINFO info{ 0 };
	info.cbSize			= sizeof(SHELLEXECUTEINFO);
	info.fMask			= SEE_MASK_NOCLOSEPROCESS;//プロセス終了待つ用
	info.hwnd			= NULL;
	info.lpVerb			= L"open";
	info.lpFile			= batFilename.data();
	info.lpParameters	= L" 1> CmdLogStd.txt 2> CmdLogErr.txt";
	info.lpDirectory	= m_operationDirW.c_str();
	info.nShow			= showWindow ? SW_SHOWNORMAL : SW_HIDE;
	info.hInstApp		= NULL;

	if (ShellExecuteEx(&info) == 0 || reinterpret_cast<unsigned long long>(info.hInstApp) <= 32)
	{
		Debug::LogError("ShellExecuteEx失敗."); return false;
	}

	//TODO: 拡張エラー情報(WAIT_OBJECT_0までループとか？)
	WaitForSingleObject(info.hProcess, INFINITE);

	if (CloseHandle(info.hProcess) == 0)
	{
		Debug::LogError("CloseHandle失敗."); return false;
	}

	if (showLog)
	{
		ShowCommandLog("CmdLogStd.txt");
		ShowCommandLog("CmdLogErr.txt");
	}

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
// リダイレクトから受け取ったログを表示
//-----------------------------------------------------------------------------
bool ShellSystem::ShowCommandLog(std::string_view filename, bool error)
{
	FILE* fp = nullptr;
	fopen_s(&fp, std::string(m_operationDir + filename.data()).c_str(), "r");
	if (fp)
	{
		char line[128] = "";
		while (fgets(line, _countof(line), fp) != NULL)
		{
			OutputDebugStringA(line);
		}

		fclose(fp);
	}
	else
	{
		Debug::LogError("ログファイルの読み込み失敗.");
		return false;
	}

	//ログファイル削除
	if (DeleteFileA(std::string(m_operationDir + filename.data()).c_str()) == 0)
	{
		Debug::LogError("DeleteFileA失敗.");
		return false;
	}

	return true;
}
