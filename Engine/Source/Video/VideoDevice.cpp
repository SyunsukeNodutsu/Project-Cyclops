#include "VideoDevice.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
VideoDevice::VideoDevice()
{
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

VideoDevice::~VideoDevice()
{
	MFShutdown();
}
