//-----------------------------------------------------------------------------
// File: GraphicsDeviceChild.h
//
// エンジンのグラフィックス機能へのアクセス権限を付与
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDevice.h"

//グラフィックスデバイスの子クラスに継承
class GraphicsDeviceChild
{
public:

    //@brief グラフィックスデバイスの設定
    //@param device 設定するデバイスソース
    //@note アプリケーション実行時に一回呼び出す
    static void SetDevice(GraphicsDevice* device);

protected:

    static GraphicsDevice* m_graphicsDevice;

};
