//-----------------------------------------------------------------------------
// File: Behaviour.h
//
// 登場オブジェクト基底
//-----------------------------------------------------------------------------
#pragma once

//登場オブジェクト基底
class Behaviour
{
public:

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}

	virtual void OnDestroy() {}

protected:

	Transform	m_transform;
	std::string m_name;			//名前
	UINT32		m_tag;			//識別TAG
	bool		m_enable;		//有効かどうか

};
