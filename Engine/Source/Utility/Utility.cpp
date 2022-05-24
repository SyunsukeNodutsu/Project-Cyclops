#include "Utility.h"
//#include "../Profile/ImGuiProfile.h"

//-----------------------------------------------------------------------------
// 完全パスからファイル名(拡張子含む)を返す
//-----------------------------------------------------------------------------
const std::string& Utility::GetFilenameFromFullpath(const std::string& fullpath)
{
	static std::string result = "";

	char name[_MAX_FNAME] = "", ext[_MAX_EXT] = "";
	_splitpath_s(fullpath.c_str(), nullptr, 0, nullptr, 0, name, _countof(name), ext, _countof(ext));
	result = std::string(name) + std::string(ext);
	return result;
}
const std::wstring& Utility::GetFilenameFromFullpath(const std::wstring& fullpath)
{
	static std::wstring result = L"";

	wchar_t name[_MAX_FNAME] = L"", ext[_MAX_EXT] = L"";
	_wsplitpath_s(fullpath.c_str(), nullptr, 0, nullptr, 0, name, _countof(name), ext, _countof(ext));
	result = std::wstring(name) + std::wstring(ext);
	return result;
}

//-----------------------------------------------------------------------------
// ダイアログからファイルのパスを取得
//-----------------------------------------------------------------------------
bool Utility::OpenFileDialog(std::string& filepath, HWND hwnd, const std::string& title, const char* filters, const bool relative)
{
	auto current = std::filesystem::current_path();
	auto filename = std::filesystem::path(filepath).filename();

	static char fname[1000];
	strcpy_s(fname, sizeof(fname), filename.string().c_str());

	//デフォルトフォルダ
	std::string dir;
	if (filepath.size() == 0)
	{
		dir = current.string() + "\\";
	}
	else
	{
		auto path = std::filesystem::absolute(filepath);
		dir = path.parent_path().string() + "\\";
	}

	OPENFILENAMEA o{};
	ZeroMemory(&o, sizeof(o));

	o.lStructSize = sizeof(o);
	o.hwndOwner = hwnd;//親ウィンドウのハンドル
	o.lpstrInitialDir = dir.c_str();
	o.lpstrFile = fname;
	o.nMaxFile = sizeof(fname);
	o.lpstrFilter = filters;//拡張子でフィルター
	o.lpstrDefExt = "";
	o.lpstrTitle = title.c_str();
	o.nFilterIndex = 1;
	if (GetOpenFileNameA(&o))
	{
		//カレントディレクトリを元に戻す
		std::filesystem::current_path(current);
		//相対パスへ変換
		if (relative) filepath = std::filesystem::relative(fname).string();
		else filepath = std::filesystem::absolute(fname).string();
		return true;
	}
	std::filesystem::current_path(current);
	return false;
}
bool Utility::OpenFileDialog(std::wstring& filepath, HWND hwnd, const std::string& title, const char* filters, const bool relative)
{
	auto current = std::filesystem::current_path();
	auto filename = std::filesystem::path(filepath).filename();

	static char fname[1000];
	strcpy_s(fname, sizeof(fname), filename.string().c_str());

	//デフォルトフォルダ
	std::string dir;
	if (filepath.size() == 0)
	{
		dir = current.string() + "\\";
	}
	else
	{
		auto path = std::filesystem::absolute(filepath);
		dir = path.parent_path().string() + "\\";
	}

	OPENFILENAMEA o{};
	ZeroMemory(&o, sizeof(o));

	o.lStructSize = sizeof(o);
	o.hwndOwner = hwnd;//親ウィンドウのハンドル
	o.lpstrInitialDir = dir.c_str();
	o.lpstrFile = fname;
	o.nMaxFile = sizeof(fname);
	o.lpstrFilter = filters;//拡張子でフィルター
	o.lpstrDefExt = "";
	o.lpstrTitle = title.c_str();
	o.nFilterIndex = 1;
	if (GetOpenFileNameA(&o))//dllのエラー表示はWin10のせい 無視でok
	{
		//カレントディレクトリを元に戻す
		std::filesystem::current_path(current);
		//相対パスへ変換
		if (relative) filepath = std::filesystem::relative(fname).wstring();
		else filepath = std::filesystem::absolute(fname).wstring();

		return true;
	}
	std::filesystem::current_path(current);
	return false;
}


//=============================================================================
// 
// Debug
// 
//=============================================================================

//-----------------------------------------------------------------------------
// ログを投げる
//-----------------------------------------------------------------------------
void Debug::Log(const std::string& log, const std::source_location& location)
{
#if _DEBUG
	const std::string& file_name = location.file_name();
	const std::string& func_name = location.function_name();
	const UINT32& line = location.line();
	const UINT32& column = location.column();

	const auto& final_log = std::string(log + "\n" + "  ->" + Utility::GetFilenameFromFullpath(file_name) + ": " + func_name + "(" + ToString(line) + ", " + ToString(column) + ") : \n\n");
	OutputDebugStringA(final_log.c_str());

	ImGuiProfile::AddLog(final_log);

#endif
}
void Debug::Log(const std::wstring& log, const std::source_location& location)
{
#if _DEBUG
	Log(wide_to_sjis(log), location);
#endif
}

//-----------------------------------------------------------------------------
// 警告ログを投げる
//-----------------------------------------------------------------------------
void Debug::LogWarning(const std::string& log, const std::source_location& location)
{
#if _DEBUG
	const std::string& file_name = location.file_name();
	const std::string& func_name = location.function_name();
	const UINT32& line = location.line();
	const UINT32& column = location.column();

	const auto& final_log = std::string(log + "\n" + "  ->" + Utility::GetFilenameFromFullpath(file_name) + ": " + func_name + "(" + ToString(line) + ", " + ToString(column) + ") : \n\n");
	OutputDebugStringA(final_log.c_str());

	ImGuiProfile::AddLog(final_log, Vector3(1, 1, 0));
#endif
}
void Debug::LogWarning(const std::wstring& log, const std::source_location& location)
{
#if _DEBUG
	LogWarning(wide_to_sjis(log), location);
#endif
}

//-----------------------------------------------------------------------------
// エラーログを投げる
//-----------------------------------------------------------------------------
void Debug::LogError(const std::string& log, const std::source_location& location)
{
#if _DEBUG
	const std::string& file_name = location.file_name();
	const std::string& func_name = location.function_name();
	const UINT32& line = location.line();
	const UINT32& column = location.column();

	const auto& final_log = std::string(log + "\n" + "  ->" + Utility::GetFilenameFromFullpath(file_name) + ": " + func_name + "(" + ToString(line) + ", " + ToString(column) + ") : \n\n");
	OutputDebugStringA(final_log.c_str());

	ImGuiProfile::AddLog(final_log, Vector3(1, 0, 0));
#endif
}
void Debug::LogError(const std::wstring& log, const std::source_location& location)
{
#if _DEBUG
	LogError(wide_to_sjis(log), location);
#endif
}
