#include "Sys.h"
#include <assert.h>
#include "Engine.h"

void DrawTriangle::Initialize(DirectXCommon* direct) {
	direct_ = direct;
	SettingVertex();

}

void DrawTriangle::Draw(const Vector4& a, const Vector4& b, const Vector4& c) {
	vertexData_[0] = a;
	vertexData_[1] = b;
	vertexData_[2] = c;

	direct_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	direct_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	direct_->GetCommandList()->DrawInstanced(3, 1, 0, 0);

}

void DrawTriangle::Finalize() {
	vertexResource_->Release();
}

void DrawTriangle::SettingVertex() {
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC vertexResourceDesc{};

	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(Vector4) * 3;

	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr;

	hr = direct_->GetDevice()->CreateCommittedResource(&uplodeHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource_));
	assert(SUCCEEDED(hr));

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;

	vertexBufferView_.StrideInBytes = sizeof(Vector4);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
}
