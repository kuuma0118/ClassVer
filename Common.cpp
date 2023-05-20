#include "Common.h"
#include <cassert>
#include <dxgidebug.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Winmm.lib")

void DirectXCommon::Initialize(WinApp* win, int32_t backBufferWidth, int32_t backBufferHeight) {
	winApp_ = win;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;
	winApp_->CreateGameWindow(L"CG2", 1280, 720);

	InitializeDXGIDevice();

	InitializeCommand();

	CreateSwapChain();

	CreateFinalRenderTargets();

	CreateFence();
}

void DirectXCommon::InitializeDXGIDevice() {
	dxgiFactory_ = nullptr;
	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr_));

	useAdapter_ = nullptr;

	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr_));

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_ = nullptr;
	}

	assert(useAdapter_ != nullptr);
	device_ = nullptr;

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelString[] = { "12.2","12.1","12.0" };

	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		hr_ = D3D12CreateDevice(useAdapter_, featureLevels[i], IID_PPV_ARGS(&device_));
		if (SUCCEEDED(hr_)) {
			Log(std::format("FeatureLevel : {}\n", featureLevelString[i]));
			break;
		}
	}

	assert(device_ != nullptr);
	Log("Complete Create D3D12 Device!!\n");

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		infoQueue->PushStorageFilter(&filter);
		infoQueue->Release();
	}
#endif // _DEBUG
}

void DirectXCommon::InitializeCommand() {
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	hr_ = device_->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr_));

	commandAllocator_ = nullptr;
	hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr_));

	commandList_ = nullptr;
	hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_, nullptr,
		IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::CreateSwapChain() {
	swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = WinApp::kClientWidth;
	swapChainDesc.Height = WinApp::kClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_, winApp_->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain_));
	assert(SUCCEEDED(hr));

	rtvDescriptorHeap_ = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptionHeapDesc{};
	rtvDescriptionHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptionHeapDesc.NumDescriptors = 2;
	hr = device_->CreateDescriptorHeap(&rtvDescriptionHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap_));
	assert(SUCCEEDED(hr));

	backBuffers_[0] = { nullptr };
	backBuffers_[1] = { nullptr };
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&backBuffers_[0]));

	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&backBuffers_[1]));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateFinalRenderTargets() {
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(backBuffers_[0], &rtvDesc, rtvHandles_[0]);

	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	device_->CreateRenderTargetView(backBuffers_[1], &rtvDesc, rtvHandles_[1]);
}

void DirectXCommon::CreateFence() {
	fence_ = nullptr;
	fenceVal_ = 0;
	HRESULT	hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXCommon::PreDraw() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier_.Transition.pResource = backBuffers_[backBufferIndex];
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier_);

	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);

	float clearcolor[] = { 0.1f,0.25f,0.5f,1.0f };
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearcolor, 0, nullptr);
}

void DirectXCommon::PostDraw() {
	hr_;
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	commandList_->ResourceBarrier(1, &barrier_);
	hr_ = commandList_->Close();
	assert(SUCCEEDED(hr_));

	ID3D12CommandList* commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	swapChain_->Present(1, 0);

	fenceVal_++;
	commandQueue_->Signal(fence_, fenceVal_);

	if (fence_->GetCompletedValue() < fenceVal_) {
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));

	hr_ = commandList_->Reset(commandAllocator_, nullptr);
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::ClearRenderTarget() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);

	float clearcolor[] = { 0.1f,0.25f,0.5f,1.0f };
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearcolor, 0, nullptr);
}

void DirectXCommon::Finalize() {
	CloseHandle(fenceEvent_);
	fence_->Release();

	rtvDescriptorHeap_->Release();
	backBuffers_[0]->Release();
	backBuffers_[1]->Release();
	swapChain_->Release();
	commandList_->Release();
	commandAllocator_->Release();
	commandQueue_->Release();
	device_->Release();
	useAdapter_->Release();
	dxgiFactory_->Release();

#ifdef _DEBUG
	winApp_->GetDebugController()->Release();
#endif // _DEBUG

	CloseWindow(winApp_->GetHwnd());

	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
}

WinApp* DirectXCommon::winApp_;
IDXGIAdapter4* DirectXCommon::useAdapter_;
IDXGIFactory7* DirectXCommon::dxgiFactory_;
ID3D12Device* DirectXCommon::device_;
ID3D12CommandQueue* DirectXCommon::commandQueue_;
ID3D12CommandAllocator* DirectXCommon::commandAllocator_;
ID3D12GraphicsCommandList* DirectXCommon::commandList_;
IDXGISwapChain4* DirectXCommon::swapChain_;
ID3D12DescriptorHeap* DirectXCommon::rtvDescriptorHeap_;
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::rtvHandles_[2];
ID3D12Resource* DirectXCommon::backBuffers_[2];
UINT64 DirectXCommon::fenceVal_;
int32_t DirectXCommon::backBufferWidth_;
int32_t DirectXCommon::backBufferHeight_;
ID3D12Fence* DirectXCommon::fence_;
HANDLE DirectXCommon::fenceEvent_;
HRESULT DirectXCommon::hr_;