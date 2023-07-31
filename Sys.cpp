#include "Sys.h"
#include <assert.h>
#include "Engine.h"

void DrawTriangle::Initialize(DirectXCommon* direct,ModelEngine* engine) {
	direct_ = direct;
	Engine = engine;
	SettingVertex();
	SetColor();
	TransformMatrix();
}

void DrawTriangle::SetColor() {
	materialResource_ = CreateBufferResource(direct_->GetDevice(),
		sizeof(Vector4));

	materialResource_->Map(0, nullptr,
		reinterpret_cast<void**>(&materialData_));
}

void DrawTriangle::TransformMatrix() {
	wvpResource_ = CreateBufferResource(direct_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = MakeIdentity4x4();
}

void DrawTriangle::Draw(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material, const Matrix4x4& wvpData) {
	vertexData_[0] = a;
	vertexData_[1] = b;
	vertexData_[2] = c;

	*materialData_ = material;
	*wvpData_ = wvpData;

	direct_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	direct_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	direct_->GetCommandList()->SetGraphicsRootDescriptorTable(2, Engine->GetSrvHandleGPU());

	direct_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void DrawTriangle::Finalize() {
	wvpResource_->Release();
	materialResource_->Release();
	vertexResource_->Release();
}

void DrawTriangle::SettingVertex() {
	vertexResource_ = CreateBufferResource(direct_->GetDevice(),
		sizeof(Vector4) * 3);
	vertexBufferView_.BufferLocation =
		vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;
	vertexBufferView_.StrideInBytes = sizeof(Vector4);
	vertexResource_->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexData_));
}

ID3D12Resource* DrawTriangle::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resourceDesc{};

	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;

	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr;

	ID3D12Resource* resource = nullptr;

	hr = device->CreateCommittedResource(&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr
		, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}