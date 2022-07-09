//-----------------------------------------------------------------------------
// File: Easing.h
//
// イージング 参考: https://easings.net/ja
//-----------------------------------------------------------------------------
#pragma once

//イージング
class Easing
{
public:

	static float Linear(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float QuadIn(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float QuadOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float QuadInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float CubicIn(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float CubicOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float CubicInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float QuartIn(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float QuartOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float QuartInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float QuintIn(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float QuintOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float QuintInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float SineIn(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float SineOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float SineInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float ExpIn(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float ExpOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float ExpInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float CircIn(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float CircOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float CircInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float ElasticIn(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float ElasticOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float ElasticInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float BackIn(float t, float totaltime, float min = 0.0f, float max = 1.0f, float s = 1.7f);
	static float BackOut(float t, float totaltime, float min = 0.0f, float max = 1.0f, float s = 1.7f);
	static float BackInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f, float s = 1.7f);
	static float BounceOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float BounceIn(float t, float totaltime, float min = 0.0f, float max = 1.0f);
	static float BounceInOut(float t, float totaltime, float min = 0.0f, float max = 1.0f);

};
