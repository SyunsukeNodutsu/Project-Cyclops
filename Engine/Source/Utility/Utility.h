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

    template<class T>
    void SafeRelease(T*& p) { if (p) { p->Release(); p = nullptr; } }

    template <class T>
    void SafeRelease(T** pp) { if (*pp) { (*pp)->Release(); *pp = nullptr; } }

    template<class T>
    void SafeDelete(T*& p) { if (p) { delete p; p = nullptr; } }

    static constexpr double ByteToGB(double size) { return size / 1073741824; }
}

//便利機能クラス
class Utility
{
public:

    //@brief 文字列をハッシュ化
    static constexpr size_t StringToHash(const char* input)
    {
        // https://stackoverflow.com/questions/2111667/compile-time-string-hashing
        size_t hash = sizeof(size_t) == 8 ? 0xcbf29ce484222325 : 0x811c9dc5;
        const size_t prime = sizeof(size_t) == 8 ? 0x00000100000001b3 : 0x01000193;

        while (*input)
        {
            hash ^= static_cast<size_t>(*input);
            hash *= prime;
            ++input;
        }

        return hash;
    }

    //@brief 完全パスからファイル名(拡張子含む)を返す
    static const std::string& GetFilenameFromFullpath(const std::string& fullpath);

    //@brief 完全パスからファイル名(拡張子含む)を返す
    static const std::wstring& GetFilenameFromFullpath(const std::wstring& fullpath);

    //@brief ダイアログからファイルのパスを取得
    //@param filepath 受け取るパス
    //@param filters 拡張子フィルター
    //@param relative 相対パスに変換を行うかどうか
    static bool OpenFileDialog(std::string& filepath, HWND hwnd = nullptr, const std::string& title = "select file", const char* filters = nullptr, const bool relative = false);

    //@brief ダイアログからファイルのパスを取得
    //@param filepath 受け取るパス
    //@param filters 拡張子フィルター
    //@param relative 相対パスに変換を行うかどうか
    static bool OpenFileDialog(std::wstring& filepath, HWND hwnd = nullptr, const std::string& title = "select file", const char* filters = nullptr, const bool relative = false);

};

//デバッグクラス
class Debug
{
public:

    //@brief ログを投げる
    //@param location ログに出力するロケーション情報 この値は設定しない
    static void Log(const std::string& log, const std::source_location& location = std::source_location::current());

    //@brief ログを投げる
    //@param location ログに出力するロケーション情報 この値は設定しない
    static void Log(const std::wstring& log, const std::source_location& location = std::source_location::current());

    //@brief 警告ログを投げる
    //@param location ログに出力するロケーション情報 この値は設定しない
    static void LogWarning(const std::string& log, const std::source_location& location = std::source_location::current());

    //@brief 警告ログを投げる
    //@param location ログに出力するロケーション情報 この値は設定しない
    static void LogWarning(const std::wstring& log, const std::source_location& location = std::source_location::current());

    //@brief エラーログを投げる
    //@param location ログに出力するロケーション情報 この値は設定しない
    static void LogError(const std::string& log, const std::source_location& location = std::source_location::current());

    //@brief エラーログを投げる
    //@param location ログに出力するロケーション情報 この値は設定しない
    static void LogError(const std::wstring& log, const std::source_location& location = std::source_location::current());

};
