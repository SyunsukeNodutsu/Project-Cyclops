//-----------------------------------------------------------------------------
// File: VideoDevice.h
//
// エンジンのビデオ機能
//-----------------------------------------------------------------------------
#pragma once

//ビデオデバイスクラス
class VideoDevice : public ISubsystem
{
public:

	VideoDevice();
	~VideoDevice()= default;

	bool Initialize();
	void Finalize();

	void Play();

	HRESULT ResizeVideo(WORD width, WORD height);

private:

	//std::wstring m_filepath = L"../test_360p.mp4";
	std::wstring m_filepath = L"../test.mp4";

	IMFSourceResolver* pSourceResolver = NULL;
	IUnknown* uSource = NULL;
	IMFTopology* pTopology = NULL;
	IMFPresentationDescriptor* pSourcePD = NULL;
	IMFStreamDescriptor* pSourceSD = NULL;
	IMFMediaSource* pSource = NULL;
	IMFMediaSession* pSession = NULL;
	MF_OBJECT_TYPE ObjectType = MF_OBJECT_TYPE::MF_OBJECT_INVALID;
	IMFMediaTypeHandler* pHandler = NULL;
	IMFActivate* pAudioActivate = NULL, * pVideoActivate = NULL;
	PROPVARIANT varStart;
	DWORD sourceStreamCount = 0;
	IMFStreamSink* pOutputSink = NULL;
	IMFMediaType* pOutputNodeMediaType = NULL;
	IMFTopologyNode* pAudioSourceNode = NULL, * pVideoSourceNode = NULL;
	IMFTopologyNode* pAudioSinkNode = NULL, * pVideoSinkNode = NULL;
	IMFVideoDisplayControl* m_pVideoDisplay;

private:

	//
	HRESULT OnTopologyStatus(IMFMediaEvent* pEvent);

	//@brief Add a source node to a topology.
	//@param pTopology Topology.
	//@param pSource Media source.
	//@param pPD Presentation descriptor.
	//@param pSD Stream descriptor.
	//@param ppNode Receives the node pointer.
	HRESULT AddSourceNode(IMFTopology* pTopology, IMFMediaSource* pSource, IMFPresentationDescriptor* pPD, IMFStreamDescriptor* pSD, IMFTopologyNode** ppNode);

	//@brief Add an output node to a topology.
	//@param pTopology Topology.
	//@param pActivate Media sink activation object.
	//@param dwId Identifier of the stream sink.
	//@param ppNode Receives the node pointer.
	HRESULT AddOutputNode(IMFTopology* pTopology, IMFActivate* pActivate, DWORD dwId, IMFTopologyNode** ppNode);

};
