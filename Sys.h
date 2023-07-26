#pragma once

#include "Common.h"
#include "Vector4.h"

class CreateEngine;

class DrawTriangle
{
public:
	void Initialize(DirectXCommon* direct);
	void Draw(const TriangleDate& v1, const TriangleDate& v2, const TriangleDate& v3);
	void Finalize();

private:
	void SettingVertex();

private:
	CreateEngine* Engine;
	DirectXCommon* direct_;
	TriangleDate* vertexData_;
	ID3D12Resource* vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
};