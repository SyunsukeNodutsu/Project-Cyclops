//-----------------------------------------------------------------------------
// File: Utility.h
//
// 便利機能
//-----------------------------------------------------------------------------
#pragma once

//エイリアス
using Microsoft::WRL::ComPtr;

//便利マクロ
namespace
{
    template <typename T>
    static std::string ToString(const T& Val) { return std::to_string(Val); }

    static std::string ToStringV(const DirectX::XMFLOAT4& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")"); }
    static std::string ToStringV(const DirectX::XMFLOAT3& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")"); }
    static std::string ToStringV(const DirectX::XMFLOAT2& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"); }

    static std::string ToStringV(const DirectX::XMINT2& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"); }//TODO: Vector2Int -> Vector2の暗黙のキャスト

    static constexpr float PI = 3.141592654f;
    static constexpr float DegToRad(const float deg) { return deg * (PI / 180.0f); }
    static constexpr float RadToDeg(const float rad) { return rad * (180.0f / PI); }
}

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
