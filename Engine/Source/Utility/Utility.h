//-----------------------------------------------------------------------------
// File: Utility.h
//
// 便利機能
//-----------------------------------------------------------------------------
#pragma once

//エイリアス
using Microsoft::WRL::ComPtr;

typedef DirectX::SimpleMath::Vector2	float2;
typedef DirectX::SimpleMath::Vector3	float3;
typedef DirectX::SimpleMath::Vector4    float4;

typedef DirectX::SimpleMath::Matrix	    matrix4x4;
typedef DirectX::SimpleMath::Quaternion quaternion;

typedef DirectX::SimpleMath::Color		color4;

//便利マクロ
namespace
{
    template <typename T>
    static std::string ToString(const T& Val) { return std::to_string(Val); }

    static std::string ToStringV(const DirectX::XMFLOAT2& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"); }
    static std::string ToStringV(const DirectX::XMFLOAT3& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")"); }
    static std::string ToStringV(const DirectX::XMFLOAT4& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")"); }

    static std::string ToStringV(const DirectX::XMINT2& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"); }//TODO: Vector2Int -> float2の暗黙のキャスト
    static std::string ToStringV(const DirectX::XMINT3& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")"); }
    static std::string ToStringV(const DirectX::XMINT4& v) { return std::string("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")"); }

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

    //@brief 最大値を"1"最小値を"0"に正規化し返す
    //@param t 正規化する値
    static float GetZeroOneLinear(float min, float max, float t) { return (t - min) / (max - min); }

    //@brief "min"～"max"の値を"newMin"～"newMax"の値に正規化し返す
    //@param t 正規化する値
    static float GetZeroOneLinearEx(float min, float max, float newMin, float newMax, float t) { return (t - min) / (max - min) * (newMax - newMin) + newMin; }

};

//デバッグクラス
class Debug
{
public:

    //--------------------------------------------------
    // ロガー
    //--------------------------------------------------

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

    //@brief ログをクリアする
    static void LogClear();

    //--------------------------------------------------
    // デバッグ線
    //--------------------------------------------------

    //@brief デバッグ線を追加
    static void AddDebugLine(const float3& pos1, const float3& pos2, const float4& color);

    //@brief デバッグ矢印を追加
    static void AddDebugArrow(const float3& pos1, const float3& pos2, const float4& color);

    //@brief デバッグ軸を追加
    static void AddDebugAxisLine(const float3& pos, const float len);

    //@brief デバッグ球を追加
    static void AddDebugSphere(const float3& pos, const float rad, const float4& color);

};
