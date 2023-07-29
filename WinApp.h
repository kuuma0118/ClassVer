#pragma once

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include <Windows.h>
#include <cstdint>
#include <d3d12.h>

#pragma comment(lib,"d3d12.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class WinApp {
public:
	static ID3D12Debug1* GetDebugController() { return debugController_; }

	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	static inline HWND GetHwnd() { return hwnd_; }
	HINSTANCE GetHInstance()const { return wc_.hInstance; }

	static bool ProcessMessage();
	static void Finalize();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	static void CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight);

private:
	static ID3D12Debug1* debugController_;
	static inline WNDCLASS wc_{};
	static inline RECT wrc_ = { 0,0,kClientWidth,kClientHeight };
	static HWND hwnd_;
	static UINT windowStyle_;
};