#include "Sys.h"
#include <assert.h>
#include <../Blossom Engine/Engine/Engine.h>

void Triangle::Initialize(DirectXCommon* directXCommon, ModelEngine* engine, const Vector4& a, const Vector4& b, const Vector4& c, const DirectionalLight& light) {
	directXCommon_ = directXCommon;
	engine_ = engine;

	SettingVertex(a, b, c);
	SetColor();
	TransformMatrix();
	CreateDirectionalLight(light);
}

void Triangle::SetColor() {
	materialResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice().Get(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Triangle::TransformMatrix() {
	wvpResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice().Get(), sizeof(TransformationMatrix));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	wvpData_->WVP = MakeIdentity4x4();
}

void Triangle::Draw(const Transform& transform, const Transform& cameraTransform, const Vector4& material) {
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(directXCommon_->GetWin()->kClientWidth) / float(directXCommon_->GetWin()->kClientHeight), 0.1f, 100.0f);

	Matrix4x4 wvpmatrix_ = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	*wvpData_ = { wvpmatrix_,worldMatrix };

	Transform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	Matrix4x4 uvtransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMatrix = Multiply(uvtransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvtransformMatrix = Multiply(uvtransformMatrix, MakeTranslateMatrix(uvTransform.translate));

	*materialData_ = { material,false };
	materialData_->uvTransform = uvtransformMatrix;

	directXCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	directXCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	directXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, engine_->textureSrvHandleGPU_[0]);

	directXCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Triangle::Finalize() {

}

void Triangle::CreateDirectionalLight(const DirectionalLight& light)
{
	directionalLightResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice().Get(), sizeof(DirectionalLight));
	directionalLightResource_->Map(0, NULL, reinterpret_cast<void**>(&directionalLight_));

	*directionalLight_ = light;
}

void Triangle::SettingVertex(const Vector4& a, const Vector4& b, const Vector4& c) {
	vertexResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice().Get(), sizeof(VertexData) * 3);
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0].position = a;
	vertexData_[0].texcoord = { 0.0f,1.0f };

	vertexData_[1].position = b;
	vertexData_[1].texcoord = { 0.5f,0.0f };

	vertexData_[2].position = c;
	vertexData_[2].texcoord = { 1.0f,1.0f };
}