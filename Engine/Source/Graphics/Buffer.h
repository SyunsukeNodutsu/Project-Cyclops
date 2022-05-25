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

	bool Create(UINT bindFlags, UINT bufferSize, D3D11_USAGE bufferUsage, const D3D11_SUBRESOURCE_DATA* initData);
	bool CreateStructured(UINT elementSize, UINT count, bool isUAV, const D3D11_SUBRESOURCE_DATA* initData);
	void WriteData(const void* srcData, UINT size);
	void CopyFrom(const Buffer& srcBuffer);

	static ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer);

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

	ConstantBuffer() = default;
	~ConstantBuffer() = default;

	bool Create(const DataType * initData = nullptr)
	{
		if (initData) m_work = *initData;

		D3D11_SUBRESOURCE_DATA srd{};
		srd.pSysMem = &m_work;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;
		return m_buffer.Create(D3D11_BIND_CONSTANT_BUFFER, sizeof(DataType), D3D11_USAGE_DYNAMIC, &srd);
	}

	void Write()
	{
		if (m_isDirty)
		{
			m_buffer.WriteData(&m_work, m_buffer.GetSize());
			m_isDirty = false;
		}
	}

	void SetToDevice(int slot, SHADER_STAGE stage) const
	{
		if (m_graphicsDevice == nullptr) return;
		if (m_graphicsDevice->m_cpContext == nullptr) return;

		if (stage == SHADER_STAGE::VS) m_graphicsDevice->m_cpContext.Get()->VSSetConstantBuffers(slot, 1, m_buffer.GetAddress());
		if (stage == SHADER_STAGE::PS) m_graphicsDevice->m_cpContext.Get()->PSSetConstantBuffers(slot, 1, m_buffer.GetAddress());
		if (stage == SHADER_STAGE::CS) m_graphicsDevice->m_cpContext.Get()->CSSetConstantBuffers(slot, 1, m_buffer.GetAddress());
	}

	ID3D11Buffer* const* GetAddress() const { return m_buffer.GetAddress(); }

	DataType& Work() { m_isDirty = true; return m_work; }
	const DataType& Get() const { return m_work; }

private:

	Buffer		m_buffer;			//定数バッファ
	DataType	m_work;				//作業用定数バッファ ※内容
	bool		m_isDirty = true;	//データ更新フラグ

	ConstantBuffer(const ConstantBuffer& src) = delete;
	void operator=(const ConstantBuffer& src) = delete;

};
