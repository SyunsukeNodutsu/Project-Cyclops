//-----------------------------------------------------------------------------
// File: RandomSystem.h
//
// 疑似乱数.真の乱数をサポート予定
// TODO: intとrealで関数を分けたくないので調べとく
//-----------------------------------------------------------------------------
#pragma once

//乱数生成システムクラス
class RandomSystem
{
public:

	RandomSystem() = default;
	~RandomSystem() = default;

	// @brief シードのシーケンスを指定してエンジンを初期化
	static void Initialize() { m_engine.seed(m_device()); }

	//@brief 一様分布乱数を返す
	//@param min 最小値
	//@param max 最大値
	template<class T> static constexpr T GetRandomReal(T min, T max)
	{
		if (min > max) { assert(0 && "min max 引数が無効な式です."); return 0; }
		if (max < min) { assert(0 && "min max 引数が無効な式です."); return 0; }

		std::uniform_real_distribution<T> dist(min, max);
		return dist(m_engine);
	}

	//@brief 一様分布乱数を返す
	//@param min 最小値
	//@param max 最大値
	template<class T> static constexpr T GetRandomInt(T min, T max)
	{
		if (min > max) { assert(0 && "min max 引数が無効な式です."); return 0; }
		if (max < min) { assert(0 && "min max 引数が無効な式です."); return 0; }

		std::uniform_int_distribution<T> dist(min, max);
		return dist(m_engine);
	}

private:

	static std::random_device m_device;
	static std::mt19937 m_engine;

};
