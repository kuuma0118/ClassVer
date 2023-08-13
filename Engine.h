#pragma once

#include "Common.h"
#include <dxcapi.h>
#include "Vector4.h"
#include "Sys.h"
#include "ImGuiManager.h"

#include "Externals/DirectXTex/d3dx12.h"

#pragma comment(lib,"dxcompiler.lib")

class ModelEngine {
public:
	void VariableInitialize();

	void Initialize(WinApp* winApp, int32_t width, int32_t height);

	void BeginFrame();

	void EndFrame();

	void Finalize();

	void Update();

	void Draw();

	void LoadTexture(const std::string& filePath);

	DirectXCommon* GetDirectXCommon() { return directXCommon_; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetTextureSrvHandleCPU() { return textureSrvHandleCPU_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }

private:
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

	static WinApp* winApp_;
	static DirectXCommon* directXCommon_;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

	ImGuiManager* imguiManager_;

	ID3D12Resource* intermediateResource_;

	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;

	IDxcIncludeHandler* includeHandler_;

	ID3DBlob* signatureBlob_;
	ID3DBlob* errorBlob_;

	ID3D12RootSignature* rootSignature_;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	D3D12_BLEND_DESC blendDesc_{};

	IDxcBlob* vertexShaderBlob_;
	IDxcBlob* pixelShaderBlob_;

	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	ID3D12PipelineState* graphicsPipelineState_;

	D3D12_VIEWPORT viewPort_{};
	D3D12_RECT scissorRect_{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[2];

	Vector4 vertexData_;

	ID3D12Resource* textureResource_;

	IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

	void InitializeDxcCompiler();
	void InitializePSO();

	void CreateRootSignature();
	void CreateInputlayOut();

	void SettingBlendState();
	void SettingRasterizerState();

	void SettingViewPort();
	void SettingScissor();

	void SettingDepth();

	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	DirectX::ScratchImage SendTexture(const std::string& filePath);
};