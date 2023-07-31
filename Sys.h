#pragma once

#include "Common.h"
#include "Vector2.h"
#include "Vector4.h"
#include "MatrixCalculate.h"

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
};

class ModelEngine;

class DrawTriangle
{
public:
	void Initialize(DirectXCommon* direct, ModelEngine* engine);
	void Draw(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material, const Matrix4x4& wvpData);
	void Finalize();

private:
	void SettingVertex();
	void SetColor();
	void TransformMatrix();

private:
	ID3D12Resource* materialResource_;

	ModelEngine* Engine;
	DirectXCommon* direct_;
	Vector4* vertexData_;
	ID3D12Resource* vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInByte);
	Vector4* materialData_;

	ID3D12Resource* wvpResource_;
	Matrix4x4* wvpData_;
};