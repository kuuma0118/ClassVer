#include "DIrectX.h"

// コンソール画面隠蔽
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

int main(int argc, char* argv[])
{
	// 基本ウィンドウクラス登録
	CSWindowClass::RegistBasicWindowClass();
	// 基本ウィンドウ作成
	CSWindowClass Window;
	Window.CreateWnd(_T("タイトル"));
	// メッセージループ
	CSWindowClass::MsgLoop();

	return 0;
}



