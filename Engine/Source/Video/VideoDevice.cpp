#include "VideoDevice.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
VideoDevice::VideoDevice()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool VideoDevice::Initialize()
{
	HRESULT hr = S_FALSE;

	hr = MFStartup(MF_VERSION/*, MFSTARTUP_NOSOCKET*/);
	if (FAILED(hr)) { Debug::Log("Media Foundation initialisation failed."); return false; }

	hr = MFCreateSourceResolver(&pSourceResolver);
	if (FAILED(hr)) { Debug::Log("Failed to create source resolved."); return false; }
	
	//ファイルパスからメディアソース作成
	hr = pSourceResolver->CreateObjectFromURL(
		m_filepath.c_str(),
		MF_RESOLUTION_MEDIASOURCE,	//ソースオブジェクトを作成します
		nullptr,					//オプションのプロパティストアです
		&ObjectType,				//作成されたオブジェクトタイプを受信する
		&uSource);					//メディアソースへのポインタを受信する
	if (FAILED(hr)) { Debug::Log("Failed to create media object from URL."); return false; }

	hr = MFCreateMediaSession(NULL, &pSession);
	if (FAILED(hr)) { Debug::Log("Failed to create media session."); return false; }

	hr = MFCreateTopology(&pTopology);
	if (FAILED(hr)) { Debug::Log("Failed to create topology object."); return false; }

	hr = uSource->QueryInterface(IID_PPV_ARGS(&pSource));
	if (FAILED(hr)) { Debug::Log("Failed to get media source."); return false; }

	//Add source node to topology.
	hr = pSource->CreatePresentationDescriptor(&pSourcePD);
	if (FAILED(hr)) { Debug::Log("Failed to create presentation descriptor from source."); return false; }

	//Get the number of streams in the media source.
	hr = pSourcePD->GetStreamDescriptorCount(&sourceStreamCount);
	if (FAILED(hr)) { Debug::Log("Failed to get source stream count."); return false; }

	Debug::Log("Source stream count: " + ToString(sourceStreamCount));

	//利用可能なソースストリームを反復処理し レンダラーを作成する
	for (DWORD i = 0; i < sourceStreamCount; i++)
	{
		BOOL fSelected = FALSE;
		GUID guidMajorType;

		hr = pSourcePD->GetStreamDescriptorByIndex(i, &fSelected, &pSourceSD);
		if (FAILED(hr)) { Debug::Log("Failed to get stream descriptor from presentation descriptor."); return false; }

		hr = pSourceSD->GetMediaTypeHandler(&pHandler);
		if (FAILED(hr)) { Debug::Log("Failed to create media type handler from presentation descriptor."); return false; }

		hr = pHandler->GetMajorType(&guidMajorType);
		if (FAILED(hr)) { Debug::Log("Failed to get media type handler from source stream."); return false; }

		if (guidMajorType == MFMediaType_Audio && fSelected)
		{
			Debug::Log("Creating audio renderer for stream index: " + ToString(i));

			hr = MFCreateAudioRendererActivate(&pAudioActivate);
			if (FAILED(hr)) { Debug::Log("Failed to create audio renderer activate object."); return false; }

			hr = AddSourceNode(pTopology, pSource, pSourcePD, pSourceSD, &pAudioSourceNode);
			if (FAILED(hr)) { Debug::Log("Failed to add audio source node"); return false; }

			hr = AddOutputNode(pTopology, pAudioActivate, 0, &pAudioSinkNode);
			if (FAILED(hr)) { Debug::Log("Failed to add audio source node"); return false; }

			hr = pAudioSourceNode->ConnectOutput(0, pAudioSinkNode, 0);
			if (FAILED(hr)) { Debug::Log("Failed to connect audio source and sink nodes."); return false; }
		}
		else if (guidMajorType == MFMediaType_Video && fSelected)
		{
			Debug::Log("Creating video renderer for stream index: " + ToString(i));

			hr = MFCreateVideoRendererActivate(m_hwnd, &pVideoActivate);
			if (FAILED(hr)) { Debug::Log("Failed to create video renderer activate object."); return false; }

			hr = AddSourceNode(pTopology, pSource, pSourcePD, pSourceSD, &pVideoSourceNode);
			if (FAILED(hr)) { Debug::Log("Failed to add video source node"); return false; }

			hr = AddOutputNode(pTopology, pVideoActivate, 0, &pVideoSinkNode);
			if (FAILED(hr)) { Debug::Log("Failed to add video output node."); return false; }

			hr = pVideoSourceNode->ConnectOutput(0, pVideoSinkNode, 0);
			if (FAILED(hr)) { Debug::Log("Failed to connect video source and sink nodes."); return false; }
		}
		else
		{
			hr = pSourcePD->DeselectStream(i);
			if (FAILED(hr)) { Debug::Log("Error deselecting source stream."); return false; }
		}
	}

	hr = pSession->SetTopology(0, pTopology);
	if (FAILED(hr)) { Debug::Log("Failed to set the topology on the session."); return false; }

	PropVariantInit(&varStart);

	return true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void VideoDevice::Finalize()
{
	SafeRelease(pSourceResolver);
	SafeRelease(uSource);
	SafeRelease(pTopology);
	SafeRelease(pSourcePD);
	SafeRelease(pSourceSD);
	SafeRelease(pSource);
	SafeRelease(pSession);
	SafeRelease(pHandler);
	SafeRelease(pAudioActivate);
	SafeRelease(pVideoActivate);
	SafeRelease(pAudioSourceNode);
	SafeRelease(pVideoSourceNode);
	SafeRelease(pAudioSinkNode);
	SafeRelease(pVideoSinkNode);
	SafeRelease(pOutputSink);
	SafeRelease(pOutputNodeMediaType);

	MFShutdown();
}

//-----------------------------------------------------------------------------
// 再生
//-----------------------------------------------------------------------------
void VideoDevice::Play()
{
	if (FAILED(pSession->Start(&GUID_NULL, &varStart)))
		Debug::Log("Failed to start session.");
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
HRESULT VideoDevice::ResizeVideo(WORD width, WORD height)
{
	if (m_pVideoDisplay)
	{
		// Set the destination rectangle.
		// Leave the default source rectangle (0,0,1,1).

		RECT rcDest = { 0, 0, width, height };

		return m_pVideoDisplay->SetVideoPosition(NULL, &rcDest);
	}
	else
	{
		return S_OK;
	}
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
HRESULT VideoDevice::OnTopologyStatus(IMFMediaEvent* pEvent)
{
	UINT32 status;

	HRESULT hr = pEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &status);
	if (SUCCEEDED(hr) && (status == MF_TOPOSTATUS_READY))
	{
		//SafeRelease(&m_pVideoDisplay);

		// Get the IMFVideoDisplayControl interface from EVR. This call is
		// expected to fail if the media file does not have a video stream.

		(void)MFGetService(pSession, MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&m_pVideoDisplay));

		{
			PROPVARIANT varStart;
			PropVariantInit(&varStart);

			HRESULT hr = pSession->Start(&GUID_NULL, &varStart);
			PropVariantClear(&varStart);
		}
	}
	return hr;
}

//-----------------------------------------------------------------------------
// Add a source node to a topology.
//-----------------------------------------------------------------------------
HRESULT VideoDevice::AddSourceNode(IMFTopology* pTopology, IMFMediaSource* pSource, IMFPresentationDescriptor* pPD, IMFStreamDescriptor* pSD, IMFTopologyNode** ppNode)
{
	IMFTopologyNode* pNode = NULL;

	HRESULT hr = S_FALSE;
	hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode);
	if (FAILED(hr)) { Debug::Log("Failed to create topology node."); SafeRelease(&pNode); return false; }

	hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pSource);
	if (FAILED(hr)) { Debug::Log("Failed to set source on topology node."); SafeRelease(&pNode); return false; }

	hr = pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD);
	if (FAILED(hr)) { Debug::Log("Failed to set presentation descriptor on topology node."); SafeRelease(&pNode); return false; }

	hr = pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);
	if (FAILED(hr)) { Debug::Log("Failed to set stream descriptor on topology node."); SafeRelease(&pNode); return false; }

	hr = pTopology->AddNode(pNode);
	if (FAILED(hr)) { Debug::Log("Failed to add node to topology."); SafeRelease(&pNode); return false; }

	//Return the pointer to the caller.
	*ppNode = pNode;
	(*ppNode)->AddRef();

	return S_OK;
}

//-----------------------------------------------------------------------------
// Add an output node to a topology.
//-----------------------------------------------------------------------------
HRESULT VideoDevice::AddOutputNode(IMFTopology* pTopology, IMFActivate* pActivate, DWORD dwId, IMFTopologyNode** ppNode)
{
	IMFTopologyNode* pNode = NULL;

	HRESULT hr = S_FALSE;

	hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode);
	if (FAILED(hr)) { Debug::Log("Failed to create topology node."); SafeRelease(&pNode); return false; }

	hr = pNode->SetObject(pActivate);
	if (FAILED(hr)) { Debug::Log("Failed to set sink on topology node."); SafeRelease(&pNode); return false; }

	hr = pNode->SetUINT32(MF_TOPONODE_STREAMID, dwId);
	if (FAILED(hr)) { Debug::Log("Failed to set stream ID on topology node."); SafeRelease(&pNode); return false; }

	hr = pNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
	if (FAILED(hr)) { Debug::Log("Failed to set no shutdown on topology node."); SafeRelease(&pNode); return false; }

	hr = pTopology->AddNode(pNode);
	if (FAILED(hr)) { Debug::Log("Failed to add node to the topology."); SafeRelease(&pNode); return false; }

	//Return the pointer to the caller.
	*ppNode = pNode;
	(*ppNode)->AddRef();

	return S_OK;
}
