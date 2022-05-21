//-----------------------------------------------------------------------------
// File: Buffer.h
//
// ID3D11Bufferをラップ
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDeviceChild.h"

//バッファクラス
class Buffer : public GraphicsDeviceChild
{
public:

	Buffer();
	~Buffer() = default;



	ID3D11Buffer* Get() const { return m_cpBuffer.Get(); }
	ID3D11Buffer* const* GetAddress() const { return m_cpBuffer.GetAddressOf(); }

	UINT GetSize() const { return m_size; }

private:

	ComPtr<ID3D11Buffer>	m_cpBuffer;	//バッファ本体
	UINT					m_size;		//バッファのサイズ(byte)
	D3D11_USAGE				m_usage;	//バッファの使用法

private:

	Buffer(const Buffer& src) = delete;
	void operator=(const Buffer& src) = delete;

};

//作業データ付き 定数バッファクラス ※TODO: Buffer継承でいい
template<class DataType>
class ConstantBuffer : public GraphicsDeviceChild
{
public:

	//TODO: 
	enum class SHADER_STAGE
	{
		VS,
		PS,
	};

	void SetToDevice(int slot, SHADER_STAGE stage);

private:

};
