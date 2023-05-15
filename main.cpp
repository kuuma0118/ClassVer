#include "WinApp.h"

const char kWindowTitle[] = "CG2_DirectX";

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    //初期化
    WinApp::CreateWindowView();

    //ウィンドウのxが押されるまでループ
    while (WinApp::ProccessMessage() == 0) {
    }


    return 0;
}