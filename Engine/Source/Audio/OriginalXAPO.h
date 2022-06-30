//-----------------------------------------------------------------------------
// File: OriginalXAPO.h
//
// 自作XAPO
// TODO: 検証"CLSID"不要では... ※__declspec(novtable)
//-----------------------------------------------------------------------------
#pragma once

//検証用自作XAPOクラス
class  __declspec(uuid("{A3FD60BA-9070-48A0-ACE1-3A357DF8035D}"))OriginalXAPO : public CXAPOBase
{
public:

	OriginalXAPO();
	~OriginalXAPO() = default;

	//@brief XAPOを特定の入力/出力構成にロック
	//@note LockForProcessのオーバーライド
	STDMETHOD(LockForProcess) (UINT32 InputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters, UINT32 OutputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters);

	//@brief 指定された入出力バッファに対してXAPOのDSPコードを実行
	//@note Processのオーバーライド
	STDMETHOD_(void, Process) (UINT32 InputProcessParameterCount, const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters, UINT32 OutputProcessParameterCount, XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters, BOOL IsEnabled);

private:

	static XAPO_REGISTRATION_PROPERTIES m_xapoRegProp; //プロパティ

	WAVEFORMATEX m_inputFmt; //入力フォーマットの記憶場所
	WAVEFORMATEX m_outputFmt; //出力フォーマットの記億場所

};
