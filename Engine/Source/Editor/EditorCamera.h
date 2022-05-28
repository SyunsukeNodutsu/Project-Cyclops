//-----------------------------------------------------------------------------
// File: EditorCamera.h
//
// 編集カメラ
//-----------------------------------------------------------------------------
#pragma once

//編集カメラクラス
class EditorCamera : public Camera
{
public:

	EditorCamera();
	~EditorCamera() = default;

	void Update();

private:


	Vector3 m_position;
	Vector3 m_rotation;

	Vector2Int m_mousePosOld;// 前フレーム
	Vector2Int m_mousePosNow;

};