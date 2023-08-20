#include "Sprite.h"

void Sprite::Initialize(DirectXCommon* directXCommon, ModelEngine* engine) {
	directXCommon_ = directXCommon;
	engine_ = engine;

	CreateVertexData();
	SetColor();
	CreateTransform();
	CreateDirectionalLight();
}

void Sprite::SetColor() {
	materialResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Sprite::Draw(const Vector4& a, const Vector4& b, const Transform transform, const Vector4& material, uint32_t texIndex, const DirectionalLight& light) {
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, (float)directXCommon_->GetWin()->kClientWidth, (float)directXCommon_->GetWin()->kClientHeight, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	vertexData_[0].position = { a.x,b.y,0.0f,1.0f };
	vertexData_[1].position = { a.x,a.y,0.0f,1.0f };
	vertexData_[2].position = { b.x,b.y,0.0f,1.0f };
	vertexData_[3].position = { a.x,a.y,0.0f,1.0f };
	vertexData_[4].position = { b.x,a.y,0.0f,1.0f };
	vertexData_[5].position = { b.x,b.y,0.0f,1.0f };

	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].texcoord = { 0.0f,0.0f };
	vertexData_[4].texcoord = { 1.0f,0.0f };
	vertexData_[5].texcoord = { 1.0f,1.0f };

	for (int i = 0; i < 6; i++) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}

	*materialData_ = { material,false };

	*wvpData_ = { worldViewProjectionMatrix,worldMatrix };
	*directionalLight_ = light;

	directXCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	directXCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, engine_->textureSrvHandleGPU_[texIndex]);
	directXCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
}

void Sprite::CreateDirectionalLight() {
	directionalLightResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice(), sizeof(DirectionalLight));
	directionalLightResource_->Map(0, NULL, reinterpret_cast<void**>(&directionalLight_));
}

void Sprite::Finalize() {
	vertexResource_->Release();
	materialResource_->Release();
	wvpResource_->Release();
	directionalLightResource_->Release();
}

void Sprite::CreateVertexData() {
	vertexResource_ = directXCommon_->CreateBufferResource(directXCommon_->GetDevice(), sizeof(VertexData) * 6);


	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;

	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
}

void Sprite::CreateTransform() {
	wvpResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice(), sizeof(TransformationMatrix));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	wvpData_->WVP = MakeIdentity4x4();
}