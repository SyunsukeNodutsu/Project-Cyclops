#include "OriginalXAPO.h"

XAPO_REGISTRATION_PROPERTIES OriginalXAPO::m_xapoRegProp = {
	__uuidof(OriginalXAPO),
	L"MyTestXAPO2",
	L"(c) Yohinori Tsuchida All Rights reserved.",
	1,0,
	XAPOBASE_DEFAULT_FLAG,
	1,1,1,1
};

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
OriginalXAPO::OriginalXAPO()
	: CXAPOBase(&m_xapoRegProp)
{
}

//-----------------------------------------------------------------------------
// LockForProcess
//-----------------------------------------------------------------------------
HRESULT OriginalXAPO::LockForProcess(UINT32 InputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters, UINT32 OutputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters)
{
	m_inputFmt = *pInputLockedParameters[0].pFormat;
	m_outputFmt = *pOutputLockedParameters[0].pFormat;

	return CXAPOBase::LockForProcess(InputLockedParameterCount, pInputLockedParameters, OutputLockedParameterCount, pOutputLockedParameters);
}

//-----------------------------------------------------------------------------
// Process
//-----------------------------------------------------------------------------
void OriginalXAPO::Process(UINT32 InputProcessParameterCount, const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters, UINT32 OutputProcessParameterCount, XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters, BOOL IsEnabled)
{
	const XAPO_PROCESS_BUFFER_PARAMETERS& inputParam = pInputProcessParameters[0];
	XAPO_PROCESS_BUFFER_PARAMETERS& outputParam = pOutputProcessParameters[0];
	memcpy(outputParam.pBuffer, inputParam.pBuffer, m_outputFmt.nBlockAlign * inputParam.ValidFrameCount);
	outputParam.ValidFrameCount = inputParam.ValidFrameCount;
	outputParam.BufferFlags = inputParam.BufferFlags;
}
