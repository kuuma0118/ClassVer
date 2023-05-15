//---------------------------------------//
	//	ウィンドウクラス
	//
	//	作成者：Daiki Terai
	//	作成日：2010/11/29
	//	修正履歴：
	//---------------------------------------//

#ifndef _WINDOWCLASS_H_
#define _WINDOWCLASS_H_

// ウィンドウズバージョン指定
#define _WIN32_WINNT 0x0500

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"winmm.lib") 

static const TCHAR* WC_BASIC = _T("Basic");		// 基本ウィンドウクラス名

// ウィンドウクラス
class CSWindowClass
{
private:
	static HINSTANCE		m_hInstance;	// インスタンスハンドル
	HWND					m_hWnd;			// ウィンドウハンドル
	static	CSWindowClass* m_pThis;		// 登録用

public:
	// コンストラクタ
	CSWindowClass();
	// デストラクタ
	virtual ~CSWindowClass();

	// シンプルウィンドウクラスの登録
	static bool RegistBasicWindowClass();

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
	virtual bool CreateWnd(
		const TCHAR* Title, int Width = CW_USEDEFAULT, int Height = CW_USEDEFAULT, int X = CW_USEDEFAULT, int Y = CW_USEDEFAULT,
		HWND hParentWnd = NULL, HMENU hMenu = NULL, const TCHAR* ClassName = WC_BASIC, DWORD Style = WS_OVERLAPPEDWINDOW | WS_VISIBLE, DWORD ExStyle = 0);

private:
	// ウィンドウプロシージャの管理者
	static LRESULT CALLBACK WndProcManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	// ウィンドウプロシージャ
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:

	// メッセージループ
	static void MsgLoop(int ArrayLength = 0, CSWindowClass* pWindowArray = NULL);

	// メッセージ処理していないときにする処理
	virtual void Idle();
	// ウィンドウ作成メッセージ
	virtual BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
	// ウィンドウ破棄メッセージ
	virtual void OnDestroy(HWND hWnd);
	// ウィンドウ描画メッセージ
	virtual void OnPaint(HWND hWnd);
	// キー入力関係メッセージ
	virtual void OnChar(HWND hWnd, TCHAR ch, int cRepeat);
	// マウス関連メッセージ
	virtual void OnLButtonDown(HWND hWnd, BOOL isDoubleClick, int x, int y, UINT keyFlags);
	virtual void OnRButtonDown(HWND hWnd, BOOL isDoubleClick, int x, int y, UINT keyFlags);
	virtual void OnMButtonDown(HWND hWnd, BOOL isDoubleClick, int x, int y, UINT keyFlags);
	virtual void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
	virtual void OnRButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
	virtual void OnMButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
	virtual void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnMouseWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys);


};


#endif // _WINDOWCLASS_H_