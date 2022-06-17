//-----------------------------------------------------------------------------
// File: ShellSystem.h
//
// シェルAPIを使用したシステム
// batファイルの管理はWindowsのTempフォルダで行う
//-----------------------------------------------------------------------------
#pragma once

//シェルシステムクラス
class ShellSystem
{
public:

	ShellSystem() = default;
	~ShellSystem() = default;

	//@brief batファイルを作成する
	//@param cmd 記憶させるコマンド
	//@param batFilename 作成したbatファイルにつける名前(拡張子含む)
	static bool MakeBatfile(std::string_view cmd, std::string_view batFilename);

	//@brief batファイルを実行する
	//@param batFilename 実行するbatファイルの名前(拡張子含む)
	//@param showLog ログを表示するかどうか
	//@param showWindow ウィンドウを表示するかどうか
	static bool ExecuteShell(std::wstring_view batFilename, bool showLog = false, bool showWindow = false);

	//@brief このアプリケーションで作成したbatファイルをすべて削除する
	static void DeleteBatFileAll();

private:

	static const std::string		m_operationDir;		//操作ディレクトリ
	static const std::wstring		m_operationDirW;	//操作ディレクトリ(ワイドver)
	static std::list<std::string>	m_batfileList;		//作成したbatファイルを保存しておく

private:

	static bool ShowCommandLog(std::string_view filename, bool error = false);

};
