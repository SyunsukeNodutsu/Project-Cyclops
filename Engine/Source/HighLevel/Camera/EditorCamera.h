//-----------------------------------------------------------------------------
// File: EditorCamera.h
//
// 編集カメラ UnityのEditorカメラの動作を参考
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

	Vector3		m_position;		//座標
	Vector3		m_rotation;		//回転

	float		m_moveSpeed;	//移動速度
	float		m_zoomSpeed;	//ズーム移動速度
	float		m_rotSpeed;		//回転速度

	Vector2Int	m_mousePosNow;	//現フレームのマウス座標
	Vector2Int	m_mousePosOld;	//前フレームのマウス座標

};