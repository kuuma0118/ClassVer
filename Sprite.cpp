#include "Sprite.h"

void Sprite::Initialize(DirectXCommon* directXCommon) {
	directXCommon_ = directXCommon;
	CreateVertexData();
	CreateTransform();
}

void Sprite::Draw(Vector4 a, Vector4 b, Transform transform) {
	vertexData_[0].position = { a.x,b.y,0.0f,1.0f };
	vertexData_[1].position = { a.x,a.y,0.0f,1.0f };
	vertexData_[2].position = { b.x,b.y,0.0f,1.0f };
	vertexData_[3].position = { a.x,a.y,0.0f,1.0f };
	vertexData_[4].position = { b.x,a.y,0.0f,1.0f };
	vertexData_[5].position = { b.x,b.y,0.0f,1.0f };

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, (float)directXCommon_->GetWin()->kClientWidth, (float)directXCommon_->GetWin()->kClientHeight, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	*transformationMatrixData_ = worldViewProjectionMatrix;

	directXCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	directXCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
}

void Sprite::Finalize() {
	vertexResource_->Release();
	transformationMatrixResource_->Release();
}

void Sprite::CreateVertexData() {
	vertexResource_ = directXCommon_->CreateBufferResource(directXCommon_->GetDevice(), sizeof(VertexData) * 6);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].texcoord = { 0.0f,0.0f };
	vertexData_[4].texcoord = { 1.0f,0.0f };
	vertexData_[5].texcoord = { 1.0f,1.0f };
}

void Sprite::CreateTransform() {
	transformationMatrixResource_ = directXCommon_->CreateBufferResource(directXCommon_->GetDevice(), sizeof(Matrix4x4));
	transformationMatrixData_ = nullptr;

	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	*transformationMatrixData_ = MakeIdentity4x4();
}