#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>

#include "WinApp.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

class DirectXCommon {
public:
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	static void Log(const std::string& message);

	static void DirectXInitialize();

	static void DXGIDeviceInitialize();

	static void CommandInitialize();

	static void CreateSwapChain();

	static void CreateRenderTarget();

	static void CreateFence();

	static void Release();

	static void ResourceLeakCheck();

private:
	static WinApp* winApp_;

	// DXGIファクトリーの生成
	static IDXGIFactory7* dxgiFactory;
	static HRESULT result;

	// 使用するアダプタ用
	static IDXGIAdapter4* useAdapter;

	// D3D12Deviceの生成
	static ID3D12Device* device;

	// コマンドキュー生成
	static ID3D12CommandQueue* commandQueue;
	static inline D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	// コマンドアロケータの生成
	static ID3D12CommandAllocator* commandAllocator;

	// コマンドリストを生成する
	static ID3D12GraphicsCommandList* commandList;

	// スワップチェーン
	static IDXGISwapChain4* swapChain;
	static inline DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	// swapChainからResourceを引っ張ってくる
	static ID3D12Resource* swapChainResources[2];

	// ディスクリプタヒープの生成
	static ID3D12DescriptorHeap* rtvDescriptorHeap;
	static inline D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorDesc{};

	// Fence
	static ID3D12Fence* fence;
	static uint64_t fenceValue;
	static HANDLE fenceEvent;

	// debug
	static ID3D12Debug1* debugController;
	static IDXGIDebug1* debug;
};