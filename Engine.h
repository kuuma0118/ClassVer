#pragma once

#include "Common.h"
#include <dxcapi.h>
#include "Vector4.h"
#include "Sys.h"
#include "Transform.h"

#pragma comment(lib,"dxcompiler.lib")

class ModelEngine {
public:
	void variableInitialize();
	void Initialize(WinApp* win, int32_t width, int32_t height);
	void BeginFrame();
	void EndFrame();
	void Finalize();
	void Update();
	void Draw();

private:
	static WinApp* win_;
	static DirectXCommon* direct_;

	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;
	ID3D10Blob* signatureBlob_;
	ID3D10Blob* errorBlob_;
	ID3D12RootSignature* rootSignature_;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	D3D12_BLEND_DESC blendDesc_{};
	IDxcBlob* vertexShaderBlob_;
	IDxcBlob* pixelShaderBlob_;
	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	ID3D12PipelineState* graphicsPipelineState_;
	ID3D12Resource* vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	D3D12_VIEWPORT viewPort_{};
	D3D12_RECT scissorRect_{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[1];

	

	Transform transform_;
	Transform cameraTransform_;

	Matrix4x4 worldMatrix_;

	DrawTriangle* triangle[3];
	TriangleDate vertexData_[3];
	TriangleDate TriangleVertex[3];

	Vector4 material[3];

	IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

	void InitializeDxcCompiler();
	void CreateRootSignature();
	void CreateInputlayOut();
	void SettingBlendState();
	void SettingRasterizerState();
	void InitializePSO();
	//void SettingVertex();
	void SettingViewPort();
	void SettingScissor();

	//ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
};