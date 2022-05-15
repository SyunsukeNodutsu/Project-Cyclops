#include "GraphicsDeviceChild.h"

GraphicsDevice* GraphicsDeviceChild::g_graphicsDevice = nullptr;

//-----------------------------------------------------------------------------
// グラフィックスデバイスの設定
//-----------------------------------------------------------------------------
void GraphicsDeviceChild::SetDevice(GraphicsDevice* device)
{
	if (device == nullptr) return;
	g_graphicsDevice = device;
}
