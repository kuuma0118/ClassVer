#include "Sys.h"
#include <assert.h>
#include "Engine.h"

void DrawTriangle::Initialize(DirectXCommon* direct, const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material) {
	direct_ = direct;
	SettingVertex(a, b, c);
	SetColor(material);
}

void DrawTriangle::SetColor(const Vector4& material) {
	materialResource_ = CreateBufferResource(direct_->GetDevice(),
		sizeof(Vector4));

	materialResource_->Map(0, nullptr,
		reinterpret_cast<void**>(&materialData_));
	*materialData_ = material;
}

void DrawTriangle::Draw() {
	direct_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	direct_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	direct_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void DrawTriangle::Finalize() {
	materialResource_->Release();
	vertexResource_->Release();
}

void DrawTriangle::SettingVertex(const Vector4& a, const Vector4& b, const Vector4& c) {
	vertexResource_ = CreateBufferResource(direct_->GetDevice(),
		sizeof(Vector4) * 3);
	vertexBufferView_.BufferLocation =
		vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;
	vertexBufferView_.StrideInBytes = sizeof(Vector4);
	vertexResource_->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexData_));

	vertexData_[0] = a;
	vertexData_[1] = b;
	vertexData_[2] = c;
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