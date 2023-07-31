#include "Engine.h"
#include <cassert>

IDxcBlob* ModelEngine::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
	Log(ConvertString(std::format(L"Begin CompileShader, path:{},profile:{}\n", filePath, profile)));
	IDxcBlobEncoding* shaderSource = nullptr;
	direct_->SetHr(dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource));

	assert(SUCCEEDED(direct_->GetHr()));

	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};

	IDxcResult* shaderResult = nullptr;
	direct_->SetHr(dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)
	));

	assert(SUCCEEDED(direct_->GetHr()));

	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		assert(false);
	}

	IDxcBlob* shaderBlob = nullptr;
	direct_->SetHr(shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr));
	assert(SUCCEEDED(direct_->GetHr()));

	Log(ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));

	shaderSource->Release();
	shaderResult->Release();

	return shaderBlob;
}

void ModelEngine::InitializeDxcCompiler() {
	HRESULT hr;
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));
	//現時点でincludeはしないが、includeに対応するための設定を行っていく
	includeHandler_ = nullptr;
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void ModelEngine::CreateRootSignature() {
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	D3D12_DESCRIPTOR_RANGE descriptoraRange[1] = {};
	descriptoraRange[0].BaseShaderRegister = 0;
	descriptoraRange[0].NumDescriptors = 1;
	descriptoraRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptoraRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptoraRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptoraRange);
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	signatureBlob_ = nullptr;
	errorBlob_ = nullptr;

	HRESULT hr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);

	if (FAILED(direct_->GetHr())) {
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	rootSignature_ = nullptr;
	hr = direct_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void ModelEngine::CreateInputlayOut() {
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void ModelEngine::SettingBlendState() {
	blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
}

void ModelEngine::SettingRasterizerState() {
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	vertexShaderBlob_ = CompileShader(L"./shader/Object3d.VS.hlsl",
		L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlob_ != nullptr);


	pixelShaderBlob_ = CompileShader(L"./shader/Object3d.PS.hlsl",
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob_ != nullptr);
}

void ModelEngine::InitializePSO() {
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;

	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize() };

	graphicsPipelineStateDesc.BlendState = blendDesc_;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineState_ = nullptr;
	HRESULT hr = direct_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}

void ModelEngine::SettingViewPort() {
	viewPort_.Width = WinApp::kClientWidth;
	viewPort_.Height = WinApp::kClientHeight;
	viewPort_.TopLeftX = 0;
	viewPort_.TopLeftY = 0;
	viewPort_.MinDepth = 0.0f;
	viewPort_.MaxDepth = 1.0f;
}

void ModelEngine::SettingScissor() {
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight;
}

void ModelEngine::variableInitialize() {
	vertexData_[0].v1 = { -0.1f,0.1f,0.0f,1.0f };
	vertexData_[0].v2 = { 0.0f,0.3f,0.0f,1.0f };
	vertexData_[0].v3 = { 0.1f,0.1f,0.0f,1.0f };
	vertexData_[1].v1 = { -0.1f,-0.3f,0.0f,1.0f };
	vertexData_[1].v2 = { 0.0f,-0.1f,0.0f,1.0f };
	vertexData_[1].v3 = { 0.1f,-0.3f,0.0f,1.0f };
	vertexData_[2].v1 = { -0.1f,-0.7f,0.0f,1.0f };
	vertexData_[2].v2 = { 0.0f,-0.5f,0.0f,1.0f };
	vertexData_[2].v3 = { 0.1f,-0.7f,0.0f,1.0f };

	material[0] = { 1.0f,0.1f,1.0f,1.0f };
	material[1] = { 2.0f,1.3f,1.4f,1.2f };
	material[2] = { 0.3f,1.0f,0.4f,1.0f };

	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };
	vertexTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	for (int i = 0; i < 3; i++) {
		triangle[i] = new DrawTriangle();
		triangle[i]->Initialize(direct_,this);
	}
	LoadTexture("resources/uvChecker.png");
}

void ModelEngine::Initialize(WinApp* win, int32_t width, int32_t height) {
	direct_->Initialize(win, win->kClientWidth, win->kClientHeight);

	InitializeDxcCompiler();


	CreateRootSignature();
	CreateInputlayOut();


	SettingBlendState();

	SettingRasterizerState();

	InitializePSO();

	SettingViewPort();

	SettingScissor();

	direct_->ImGuiInitialize();
}


void ModelEngine::BeginFrame() {
	direct_->PreDraw();
	direct_->GetCommandList()->RSSetViewports(1, &viewPort_);
	direct_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);

	direct_->GetCommandList()->SetGraphicsRootSignature(rootSignature_);
	direct_->GetCommandList()->SetPipelineState(graphicsPipelineState_);

	ImGui::ShowDemoWindow();
}

void ModelEngine::EndFrame() {
	ImGui::Render();
	direct_->PostDraw();
}

void ModelEngine::Finalize() {
	textureResource->Release();
	for (int i = 0; i < 3; i++) {
		triangle[i]->Finalize();
	}

	graphicsPipelineState_->Release();
	signatureBlob_->Release();

	if (errorBlob_) {
		errorBlob_->Release();
	}

	rootSignature_->Release();
	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();
	direct_->Finalize();
}

void ModelEngine::Update() {
	worldmatrix_ = MakeAffineMatrix(vertexTransform_.scale, vertexTransform_.rotate, vertexTransform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(win_->kClientWidth) / float(win_->kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldmatrix_, Multiply(viewMatrix, projectionMatrix));

	vertexTransform_.rotate.y += 0.03f;
	worldmatrix_ = worldViewProjectionMatrix;

	ImGui::Begin("Window");
	ImGui::ColorEdit4("Color", &material[0].x);
	ImGui::ColorEdit4("Color", &material[1].x);
	ImGui::ColorEdit4("Color", &material[2].x);
	ImGui::DragFloat3("CameraTranslate", &cameraTransform_.translate.x, 0.01f);
	ImGui::End();
}

void ModelEngine::Draw() {
	for (int i = 0; i < 3; i++) {
		triangle[i]->Draw(vertexData_[i].v1, vertexData_[i].v2, vertexData_[i].v3, material[i], worldmatrix_);
	}
}
ID3D12Resource* ModelEngine::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

void ModelEngine::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	for (size_t miplevel = 0; miplevel < metadata.mipLevels; ++miplevel) {
		const DirectX::Image* img = mipImages.GetImage(miplevel, 0, 0);
		HRESULT hr = texture->WriteToSubresource(UINT(miplevel), nullptr, img->pixels, UINT(img->rowPitch), UINT(img->slicePitch));

		assert(SUCCEEDED(hr));
	}
}

DirectX::ScratchImage ModelEngine::SendTexture(const std::string& filePath)
{
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));
	return mipImages;
}

void ModelEngine::LoadTexture(const std::string& filePath) {
	DirectX::ScratchImage mipImage = SendTexture(filePath);
	const DirectX::TexMetadata& metaData = mipImage.GetMetadata();

	textureResource = CreateTextureResource(direct_->GetDevice(), metaData);
	UploadTextureData(textureResource, mipImage);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);

	textureSrvHandleGPU_ = direct_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	textureSrvHandleCPU_ = direct_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	textureSrvHandleGPU_.ptr += direct_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleCPU_.ptr += direct_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	direct_->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU_);
}

WinApp* ModelEngine::win_;
DirectXCommon* ModelEngine::direct_;