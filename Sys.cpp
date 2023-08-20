#include "Sys.h"
#include <assert.h>
#include "Engine.h"

void Triangle::Initialize(DirectXCommon* directXCommon, ModelEngine* engine) {
	directXCommon_ = directXCommon;
	engine_ = engine;

	SettingVertex();
	SetColor();
	TransformMatrix();
}

void Triangle::SetColor() {
	materialResource_ = DirectXCommon::CreateBufferResource(
		directXCommon_->GetDevice(), sizeof(VertexData));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Triangle::TransformMatrix() {
	wvpResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = MakeIdentity4x4();
}

void Triangle::Draw(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material, const Matrix4x4& wvpData) {
	vertexData_[0].position = a;
	vertexData_[0].texcoord = { 0.0f,1.0f };

	vertexData_[1].position = b;
	vertexData_[1].texcoord = { 0.5f,0.0f };

	vertexData_[2].position = c;
	vertexData_[2].texcoord = { 1.0f,1.0f };

	*materialData_ = material;
	*wvpData_ = wvpData;

	directXCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	directXCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	directXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, engine_->textureSrvHandleGPU_[0]);

	directXCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Triangle::Finalize() {
	vertexResource_->Release();
	materialResource_->Release();
	wvpResource_->Release();
}

void Triangle::SettingVertex() {
	vertexResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice(),
		sizeof(VertexData) * 3);
	vertexBufferView_.BufferLocation =
		vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexData_));
}