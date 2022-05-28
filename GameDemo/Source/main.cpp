#include "GameDemo.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	//不使用な引数をコンパイラに伝えてWarningを抑制
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if _DEBUG
	//メモリリークの検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//Windows10 CreatorsUpdateで入ったモニタ単位でDPIが違う環境への対応
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	//COMの初期化
	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
	{
		MessageBoxA(nullptr, "COM initialization failed.", "Failed", MB_OK);
		return -1;
	}

	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

	//エンジン起動
	ApplicationBase* gameDemo = new GameDemo();
	if (gameDemo)
	{
		CyclopsRoot* app = new CyclopsRoot(gameDemo);
		if (app)
		{
			app->Run();
			SafeDelete(app);
		}
		SafeDelete(gameDemo);
	}

	MFShutdown();

	CoUninitialize();

	return 0;
}
