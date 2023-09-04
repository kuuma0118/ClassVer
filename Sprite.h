#pragma once

#include "Common.h"
#include "Vector4.h"
#include "Engine.h"

class Sprite {
public:
	void Initialize(DirectXCommon* directXCommon, ModelEngine* engine);
	void Draw(const Vector4& a, const Vector4& b, const Transform transform, const Vector4& material, uint32_t texIndex, const DirectionalLight& light);
	void Finalize();

private:
	ModelEngine* engine_;
	DirectXCommon* directXCommon_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ID3D12Resource* vertexResource_;
	VertexData* vertexData_;

	DirectionalLight* directionalLight_;
	ID3D12Resource* directionalLightResource_;
	ID3D12Resource* wvpResource_;
	TransformationMatrix* wvpData_;

	ID3D12Resource* indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* indexData_;

	ID3D12Resource* materialResource_;
	Material* materialData_;

private:
	void CreateVertexData();
	void CreateTransform();
	void SetColor();
	void CreateDirectionalLight();
};