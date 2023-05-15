// WindowClass.cpp 
#include "DirectX.h"
// インスタンスハンドル
HINSTANCE CSWindowClass::m_hInstance = NULL;
// 登録用
CSWindowClass* CSWindowClass::m_pThis = NULL;

// コンストラクタ
CSWindowClass::CSWindowClass()
	:m_hWnd(NULL)
{
}

// デストラクタ
CSWindowClass::~CSWindowClass() {
}

// シンプルウィンドウクラスの登録
bool CSWindowClass::RegistBasicWindowClass() {

	m_hInstance = GetModuleHandle(NULL);

	/********************************************************/
	/*		ウィンドウクラスの定義と作成					*/
	/********************************************************/

	// シンプルウィンドウクラス設定
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProcManager, 0,0,m_hInstance,
		(HICON)LoadImage(NULL,MAKEINTRESOURCE(IDI_APPLICATION),IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_SHARED),
		(HCURSOR)LoadImage(NULL,MAKEINTRESOURCE(IDC_ARROW),IMAGE_CURSOR,0,0,LR_DEFAULTSIZE | LR_SHARED),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, WC_BASIC , NULL };

	// シンプルウィンドウクラス作成
	if (!RegisterClassEx(&wcex))
		return false;

	return true;
}

//--------------------------------------------------------------------//
//	ウィンドウ作成
//
//	引数：		Title		タイトル
//				Width		幅
//				Height		高さ
//				X			X座標
//				Y			Y座標
//				hParentWnd	親ウィンドウハンドル
//				hMenu		メニューハンドルもしくはリソースID
//				ClassName	ウィンドウクラス名
//				Style		ウィンドウスタイル
//				ExStyle		ウィンドウ拡張スタイル
//
//	戻り値：	true:	作成成功
//				false:	作成失敗
//--------------------------------------------------------------------//
bool CSWindowClass::CreateWnd(
	const TCHAR* Title, int Width, int Height, int X, int Y,
	HWND hParentWnd, HMENU hMenu, const TCHAR* ClassName, DWORD Style, DWORD ExStyle)
{
	// 重要：あとで正しいウィンドウプロシージャを呼び出すために覚えておきます
	m_pThis = this;

	/********************************************************/
	/*		ウィンドウの作成								*/
	/********************************************************/

	m_hWnd = CreateWindowEx(
		ExStyle,							//拡張ウィンドウスタイル
		ClassName,							//ウィンドウクラス名
		Title,								//タイトルバーにこの名前が表示されます
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,	//ウィンドウスタイル
		X,									//Ｘ座標
		Y,									//Ｙ座標
		Width,								//幅
		Height,								//高さ
		hParentWnd,							//親ウィンドウのハンドル、親を作るときはNULL
		hMenu,								//メニューハンドルorリソースID
		m_hInstance,						//インスタンスハンドル
		NULL);

	// ウィンドウ作成失敗
	if (m_hWnd == NULL)
		return false;

	return true;
}

//-----------------------------------------------------//
//  ウィンドウプロシージャの管理者
//	基本ウィンドウクラスではこの関数を登録して
//	この関数を経由して各ウィンドウの本当のプロシージャを呼び出します
//
//	引数：	hWnd	ウィンドウハンドル
//			msg		メッセージ
//			wParam	パラメータ
//			lParam	パラメータ
//
//	戻り値：処理結果
//-----------------------------------------------------//
LRESULT CALLBACK CSWindowClass::WndProcManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	CSWindowClass* pWindow = (CSWindowClass*)(LONG_PTR)GetWindowLong(hWnd, GWLP_USERDATA);

	//thisポインタが取得できた場合
	if (pWindow != NULL)
	{
		// 本当のウィンドウプロシージャを呼び出しメッセージ処理する
		LRESULT lResult = pWindow->WndProc(hWnd, msg, wParam, lParam);
		return lResult;
	}
	//thisポインタが取得できなかった場合
	else
	{
		// ウィンドウの作成時の場合
		if (msg == WM_CREATE || msg == WM_INITDIALOG) {
			// プロパティリストにオブジェクトポインタ(thisポインタ)を設定する
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)(LONG_PTR)(m_pThis));
			//PRINT("Create %s\n",typeid(*m_Instance).name());// どのウィンドウタイプなのか表示
			// 初期化メッセージの再送信
			SendMessage(hWnd, msg, wParam, lParam);

			m_pThis = NULL;	// 登録したので一時インスタンスポインタ格納の解除
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


//-----------------------------------------------------//
//	ウィンドウプロシージャ
//	デフォルトの処理はDefWindowProc関数で行う
//
//	引数：	hWnd	ウィンドウハンドル
//			msg		メッセージ
//			wParam	パラメータ
//			lParam	パラメータ
//
//	戻り値：処理結果
//-----------------------------------------------------//
LRESULT CALLBACK CSWindowClass::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		// キーボード
		HANDLE_MSG(hWnd, WM_CHAR, OnChar);
		// マウス
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_RBUTTONDOWN, OnRButtonDown);
		HANDLE_MSG(hWnd, WM_MBUTTONDOWN, OnMButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONDBLCLK, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_RBUTTONDBLCLK, OnRButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
		HANDLE_MSG(hWnd, WM_RBUTTONUP, OnRButtonUp);
		HANDLE_MSG(hWnd, WM_MBUTTONUP, OnMButtonUp);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
		HANDLE_MSG(hWnd, WM_MOUSEWHEEL, OnMouseWheel);

		// MCI処理終了通知メッセージ
	case MM_MCINOTIFY: {
		MCI_PLAY_PARMS mciPlay;

		// 通知メッセージ失敗
		if (wParam != MCI_NOTIFY_SUCCESSFUL)
			return 0;

		// 初期位置に戻す
		mciSendCommand((MCIDEVICEID)lParam, MCI_SEEK, MCI_SEEK_TO_START, 0);

		mciPlay.dwCallback = (DWORD_PTR)hWnd;
		// ループ再生
		mciSendCommand((MCIDEVICEID)lParam, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)&mciPlay);

		return 0;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// メッセージループ
void CSWindowClass::MsgLoop(int ArrayLength, CSWindowClass* pWindowArray) {

	/********************************************************/
	/*		メッセージループ								*/
	/********************************************************/

	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// メッセージ処理をしてないときする処理
			for (int i = 0; i < ArrayLength; ++i) {
				pWindowArray[i].Idle();
			}
		}
	}
}

// メッセージ処理していないときにする処理
void CSWindowClass::Idle() {
	// 継承時に処理を書く
}

// ウィンドウ作成メッセージ
BOOL CSWindowClass::OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct) {
	return TRUE;
}

// ウィンドウ破棄メッセージ
void CSWindowClass::OnDestroy(HWND hWnd) {
	PostQuitMessage(0);
}

// ウィンドウ描画メッセージ
void CSWindowClass::OnPaint(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
}

// キー入力メッセージ
void CSWindowClass::OnChar(HWND hWnd, TCHAR ch, int cRepeat) {}

// マウス左ボタン押下メッセージ
void CSWindowClass::OnLButtonDown(HWND hWnd, BOOL isDoubleClick, int x, int y, UINT keyFlags) {}
// マウス右ボタン押下メッセージ
void CSWindowClass::OnRButtonDown(HWND hWnd, BOOL isDoubleClick, int x, int y, UINT keyFlags) {}
// マウス真ん中ボタン押下メッセージ
void CSWindowClass::OnMButtonDown(HWND hWnd, BOOL isDoubleClick, int x, int y, UINT keyFlags) {}
// マウス左ボタン押上メッセージ
void CSWindowClass::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags) {}
// マウス右ボタン押上げメッセージ
void CSWindowClass::OnRButtonUp(HWND hWnd, int x, int y, UINT keyFlags) {}
// マウス真ん中ボタン押上メッセージ
void CSWindowClass::OnMButtonUp(HWND hWnd, int x, int y, UINT keyFlags) {}
// マウス移動メッセージ
void CSWindowClass::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags) {}
// マウスホイールメッセージ
void CSWindowClass::OnMouseWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys) {}
