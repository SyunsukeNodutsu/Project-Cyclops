#include "GraphicsDeviceChild.h"

GraphicsDevice* GraphicsDeviceChild::m_graphicsDevice = nullptr;

//-----------------------------------------------------------------------------
// グラフィックスデバイスの設定
//-----------------------------------------------------------------------------
void GraphicsDeviceChild::SetDevice(GraphicsDevice* device)
{
	if (device == nullptr) return;
	m_graphicsDevice = device;
}
