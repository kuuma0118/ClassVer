#include "WinApp.h"
#include "Common.h"

const char kWindowTitle[] = "CG2_DirectXClass";

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//初期化
	WinApp::CreateWindowView();
	DirectXCommon::DirectXInitialize();

	//ウィンドウのxが押されるまでループ
	while (WinApp::ProccessMessage() == 0) {

	}

	DirectXCommon::Release();

	return 0;
}