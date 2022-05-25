#include "Buffer.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Buffer::Buffer()
	: m_cpBuffer(nullptr)
	, m_size(0)
	, m_usage(D3D11_USAGE_DEFAULT)
{
}

//-----------------------------------------------------------------------------
// 作成
//-----------------------------------------------------------------------------
bool Buffer::Create(UINT bindFlags, UINT bufferSize, D3D11_USAGE bufferUsage, const D3D11_SUBRESOURCE_DATA* initData)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpContext == nullptr) return false;

	//アライメント チェック
	if (bindFlags == D3D11_BIND_CONSTANT_BUFFER)
	{
		if (bufferSize % 16 != 0)
		{
			Debug::LogError("定数バッファ アライメント."); return false;
		}
	}

	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = bindFlags;		//デバイスにバインドする際の種類
	desc.ByteWidth = bufferSize;	//作成するバッファのバイトサイズ
	desc.MiscFlags = 0;				//その他のフラグ
	desc.StructureByteStride = 0;	//構造化バッファの場合、その構造体のサイズ
	desc.Usage = bufferUsage;		//作成するバッファの使用法

	//CPUアクセスレベル C++(CPU)側から書き換え可能
	switch (desc.Usage)
	{
		//動的ビデオメモリバッファ
	case D3D11_USAGE_DYNAMIC:
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;

		//静的ビデオメモリバッファ
	case D3D11_USAGE_DEFAULT:
		desc.CPUAccessFlags = 0;
		break;

		//ステージングバッファ
	case D3D11_USAGE_STAGING:
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		break;
	}

	if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateBuffer(&desc, initData, &m_cpBuffer)))
	{
		Debug::LogError("バッファ作成失敗."); return false;
	}

	m_usage = bufferUsage;
	m_size = bufferSize;

	return true;
}

//-----------------------------------------------------------------------------
// 構造化バッファを作成
//-----------------------------------------------------------------------------
bool Buffer::CreateStructured(UINT elementSize, UINT count, bool isUAV, const D3D11_SUBRESOURCE_DATA* initData)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpContext == nullptr) return false;

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth				= elementSize * count;
	desc.MiscFlags				= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride	= elementSize;
	desc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;

	if (isUAV)
	{
		//順不同アクセスビューの場合
		desc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		m_usage					= desc.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		m_usage = desc.Usage	= D3D11_USAGE_DYNAMIC;
	}

	if (FAILED(m_graphicsDevice->m_cpDevice->CreateBuffer(&desc, initData, &m_cpBuffer)))
	{
		Debug::LogError("CreateBuffer失敗."); return false;
	}

	m_size = elementSize * count;

	return true;
}

//-----------------------------------------------------------------------------
// 書き込み
//-----------------------------------------------------------------------------
void Buffer::WriteData(const void* srcData, UINT size)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;
	if (m_cpBuffer.Get() == nullptr) return;

	//動的バッファの場合
	if (m_usage == D3D11_USAGE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE pData{};
		if (SUCCEEDED(m_graphicsDevice->m_cpContext.Get()->Map(m_cpBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			memcpy_s(pData.pData, m_size, srcData, size);
			m_graphicsDevice->m_cpContext.Get()->Unmap(m_cpBuffer.Get(), 0);
		}
	}
	//静的バッファの場合
	else if (m_usage == D3D11_USAGE_DEFAULT)
	{
		m_graphicsDevice->m_cpContext.Get()->UpdateSubresource(m_cpBuffer.Get(), 0, 0, srcData, 0, 0);
	}
	//ステージングバッファの場合(読み書き両方)
	else if (m_usage == D3D11_USAGE_STAGING)
	{
		D3D11_MAPPED_SUBRESOURCE pData{};
		if (SUCCEEDED(m_graphicsDevice->m_cpContext.Get()->Map(m_cpBuffer.Get(), 0, D3D11_MAP_READ_WRITE, 0, &pData)))
		{
			memcpy_s(pData.pData, m_size, srcData, size);
			m_graphicsDevice->m_cpContext.Get()->Unmap(m_cpBuffer.Get(), 0);
		}
	}
}

//-----------------------------------------------------------------------------
// コピー
//-----------------------------------------------------------------------------
void Buffer::CopyFrom(const Buffer& srcBuffer)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	if (srcBuffer.Get() == nullptr) return;

	m_graphicsDevice->m_cpContext.Get()->CopyResource(m_cpBuffer.Get(), srcBuffer.Get());
}

//-----------------------------------------------------------------------------
// UAVのバッファの内容を CPU から読み込み可能なバッファへコピーして返す
//-----------------------------------------------------------------------------
ID3D11Buffer* Buffer::CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer)
{
	if (m_graphicsDevice == nullptr) return nullptr;
	if (m_graphicsDevice->m_cpDevice == nullptr) return nullptr;

	if (pBuffer == nullptr) return nullptr;

	ID3D11Buffer* debugbuf = nullptr;

	D3D11_BUFFER_DESC desc{};
	pBuffer->GetDesc(&desc);

	//下記変更点
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(m_graphicsDevice->m_cpDevice->CreateBuffer(&desc, nullptr, &debugbuf)))
		m_graphicsDevice->m_cpContext->CopyResource(debugbuf, pBuffer);
	else
		Debug::LogError("CreateBuffer失敗.");

	return debugbuf;
}
