//-----------------------------------------------------------------------------
// File: ShellSystem.h
//
// シェルAPIを使用したシステム
// batファイルの管理はWindowsのTempフォルダで行う
//-----------------------------------------------------------------------------
#pragma once

//batファイルの実行方法
enum class ShellExecuteMode
{
	HideImmediate,	//表示なし 裏で実行
	HideRedirect,	//表示なし リダイレクト受け取り
	ShowCmdPrompt,	//CMD表示(/K コマンド)
};

//シェルシステムクラス
class ShellSystem
{
public:

	ShellSystem() = default;
	~ShellSystem() = default;

	//@brief batファイルを作成/登録する
	//@param cmd 記憶させるコマンド
	//@param batFilename 作成したbatファイルにつける名前(拡張子含む)
	//@param addList 作成したbatファイルを管理リストに追加するかどうか
	//@return 成功...true, 失敗...false
	static bool RegistBatfile(std::string_view cmd, std::string_view batFilename, bool addList = true);

	//@brief batファイルを実行する
	//@param batFilename 実行するbatファイルの名前(拡張子含む)
	//@param dir コマンドを使用するディレクトリ(cdコマンド実行)
	//@param exeMode batファイルの実行モード
	//@return 成功...true, 失敗...false
	static bool ExecuteShell(std::string_view batFilename, std::string_view dir = "D:", ShellExecuteMode exeMode = ShellExecuteMode::HideImmediate);

	//@brief このアプリケーションで作成したbatファイルをすべて削除する
	static void DeleteBatFileAll();

public:

	//リダイレクトのファイルパス(1.標準出力)
	static const std::string_view	m_stdRedirectFiepath;
	//リダイレクトのファイルパス(2.エラー出力)
	static const std::string_view	m_errRedirectFiepath;

private:

	static const std::string_view	m_operationDir; //操作ディレクトリ
	static std::list<std::string>	m_batfileList;	//作成したbatファイルを保存しておく

private:

	static bool ChangeExecuteDirectory(std::string_view batFilename, std::string_view dir, ShellExecuteMode exeMode);

};
