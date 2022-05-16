#include "Utility.h"

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


//=============================================================================
// 
// Debug
// 
//=============================================================================

//-----------------------------------------------------------------------------
// ログを投げる
//-----------------------------------------------------------------------------
void Debug::Log(std::string log, std::source_location location)
{
#if _DEBUG
	const std::string& file_name = location.file_name();
	const std::string& func_name = location.function_name();
	const UINT32& line = location.line();
	const UINT32& column = location.column();

	OutputDebugStringA(std::string(log + "\n" + "  ->" + Utility::GetFilenameFromFullpath(file_name) + ": " + func_name + "(" + ToString(line) + ", " + ToString(column) + ") : \n\n").c_str());
#endif
}
void Debug::Log(std::wstring log, std::source_location location)
{
#if _DEBUG
	const std::string& file_name = location.file_name();
	const std::string& func_name = location.function_name();
	const UINT32& line = location.line();
	const UINT32& column = location.column();

	OutputDebugStringW(std::wstring(log + L"\n").c_str());
	OutputDebugStringA(std::string("  ->" + Utility::GetFilenameFromFullpath(file_name) + ": " + func_name + "(" + ToString(line) + ", " + ToString(column) + ")\n\n").c_str());
#endif
}
