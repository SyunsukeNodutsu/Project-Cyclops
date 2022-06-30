//-----------------------------------------------------------------------------
// File: OriginalXAPO.h
//
// ����XAPO
// TODO: ����"CLSID"�s�v�ł�... ��__declspec(novtable)
//-----------------------------------------------------------------------------
#pragma once

//���ؗp����XAPO�N���X
class  __declspec(uuid("{A3FD60BA-9070-48A0-ACE1-3A357DF8035D}"))OriginalXAPO : public CXAPOBase
{
public:

	OriginalXAPO();
	~OriginalXAPO() = default;

	//@brief XAPO�����̓���/�o�͍\���Ƀ��b�N
	//@note LockForProcess�̃I�[�o�[���C�h
	STDMETHOD(LockForProcess) (UINT32 InputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters, UINT32 OutputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters);

	//@brief �w�肳�ꂽ���o�̓o�b�t�@�ɑ΂���XAPO��DSP�R�[�h�����s
	//@note Process�̃I�[�o�[���C�h
	STDMETHOD_(void, Process) (UINT32 InputProcessParameterCount, const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters, UINT32 OutputProcessParameterCount, XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters, BOOL IsEnabled);

private:

	static XAPO_REGISTRATION_PROPERTIES m_xapoRegProp; //�v���p�e�B

	WAVEFORMATEX m_inputFmt; //���̓t�H�[�}�b�g�̋L���ꏊ
	WAVEFORMATEX m_outputFmt; //�o�̓t�H�[�}�b�g�̋L���ꏊ

};
