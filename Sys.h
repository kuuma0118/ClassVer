#pragma once

#include "Common.h"
#include "Vector4.h"

class ModelEngine;

class DrawTriangle
{
public:
	void Initialize(DirectXCommon* direct, const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material);
	void Draw();
	void Finalize();

private:
	void SettingVertex(const Vector4& a, const Vector4& b, const Vector4& c);
	void SetColor(const Vector4& material);

private:
	ID3D12Resource* materialResource_;

	ModelEngine* Engine;
	DirectXCommon* direct_;
	Vector4* vertexData_;
	ID3D12Resource* vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInByte);
	Vector4* materialData_;
};