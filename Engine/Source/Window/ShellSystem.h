//-----------------------------------------------------------------------------
// File: ShellSystem.h
//
// シェルAPIを使用したシステム
// batファイルの管理はWindowsのTempフォルダで行う
// TODO: コマンド使用のディレクトリを指定したい
//-----------------------------------------------------------------------------
#pragma once

//batファイルの実行方法
enum class ShellExecuteMode
{
	HideImmediate,	//何も表示せず裏で実行
	ShowDebugLog,	//リダイレクト受け取り VisualStadioの出力に表示
	ShowCmdPrompt,	//コマンドプロンプトを表示(/K コマンド)
};

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
	//@param dir コマンドを使用するディレクトリ(cdコマンド実行)
	//@param exeMode batファイルの実行モード
	static bool ExecuteShell(std::wstring_view batFilename, std::string_view dir = "D:", ShellExecuteMode exeMode = ShellExecuteMode::HideImmediate);

	//@brief このアプリケーションで作成したbatファイルをすべて削除する
	static void DeleteBatFileAll();

private:

	static const std::string		m_operationDir;		//操作ディレクトリ
	static const std::wstring		m_operationDirW;	//操作ディレクトリ(ワイドver)
	static std::list<std::string>	m_batfileList;		//作成したbatファイルを保存しておく

private:

	static bool ShowCommandLog(std::string_view filename, bool error = false);
	static bool ChangeExecuteDirectory(std::wstring_view batFilename, std::string_view dir);

};
