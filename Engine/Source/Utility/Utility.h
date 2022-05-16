//-----------------------------------------------------------------------------
// File: Utility.h
//
// 便利機能
//-----------------------------------------------------------------------------
#pragma once

//便利マクロ
#define ToString(Val) std::to_string(Val)//std::to_stringをエイリアス

//便利機能クラス
class Utility
{
public:

    //@brief 完全パスからファイル名(拡張子含む)を返す
    static const std::string& GetFilenameFromFullpath(const std::string& fullpath);

    //@brief 完全パスからファイル名(拡張子含む)を返す
    static const std::wstring& GetFilenameFromFullpath(const std::wstring& fullpath);

};

//デバッグクラス
class Debug
{
public:

    //@brief ログを投げる
    //@param location ログに出力するロケーション情報 この値は設定しない
    static void Log(std::string log, std::source_location location = std::source_location::current());

    //@brief ログを投げる
    //@param location ログに出力するロケーション情報 この値は設定しない
    static void Log(std::wstring log, std::source_location location = std::source_location::current());

};
