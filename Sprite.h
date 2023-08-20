#pragma once

#include "Common.h"
#include "Vector4.h"
#include "Engine.h"

class Sprite {
public:
	void Initialize(DirectXCommon* directXCommon, ModelEngine* engine);
	void Draw(const Vector4& a, const Vector4& b, const Transform transform, const Vector4& material, uint32_t texIndex);
	void Finalize();

private:
	ModelEngine* engine_;
	DirectXCommon* directXCommon_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ID3D12Resource* vertexResource_;
	VertexData* vertexData_;
	ID3D12Resource* transformMatrixResource_;
	Matrix4x4* transformMatrixData_;

	ID3D12Resource* materialResource_;
	Vector4* materialData_;

private:
	void CreateVertexData();
	void CreateTransform();
	void SetColor();
};