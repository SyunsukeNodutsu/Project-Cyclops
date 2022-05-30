//-----------------------------------------------------------------------------
// File: Easing.h
//
// イージング
//-----------------------------------------------------------------------------
#pragma once

//イージング
class Easing
{
public:

	static float Linear(float t, float totaltime, float min, float max);
	static float QuadIn(float t, float totaltime, float min, float max);
	static float QuadOut(float t, float totaltime, float min, float max);
	static float QuadInOut(float t, float totaltime, float min, float max);
	static float CubicIn(float t, float totaltime, float min, float max);
	static float CubicOut(float t, float totaltime, float min, float max);
	static float CubicInOut(float t, float totaltime, float min, float max);
	static float QuartIn(float t, float totaltime, float min, float max);
	static float QuartOut(float t, float totaltime, float min, float max);
	static float QuartInOut(float t, float totaltime, float min, float max);
	static float QuintIn(float t, float totaltime, float min, float max);
	static float QuintOut(float t, float totaltime, float min, float max);
	static float QuintInOut(float t, float totaltime, float min, float max);
	static float SineIn(float t, float totaltime, float min, float max);
	static float SineOut(float t, float totaltime, float min, float max);
	static float SineInOut(float t, float totaltime, float min, float max);
	static float ExpIn(float t, float totaltime, float min, float max);
	static float ExpOut(float t, float totaltime, float min, float max);
	static float ExpInOut(float t, float totaltime, float min, float max);
	static float CircIn(float t, float totaltime, float min, float max);
	static float CircOut(float t, float totaltime, float min, float max);
	static float CircInOut(float t, float totaltime, float min, float max);
	static float ElasticIn(float t, float totaltime, float min, float max);
	static float ElasticOut(float t, float totaltime, float min, float max);
	static float ElasticInOut(float t, float totaltime, float min, float max);
	static float BackIn(float t, float totaltime, float min, float max, float s = 1.7f);
	static float BackOut(float t, float totaltime, float min, float max, float s = 1.7f);
	static float BackInOut(float t, float totaltime, float min, float max, float s = 1.7f);
	static float BounceOut(float t, float totaltime, float min, float max);
	static float BounceIn(float t, float totaltime, float min, float max);
	static float BounceInOut(float t, float totaltime, float min, float max);

};
