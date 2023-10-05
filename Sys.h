#pragma once

#include "Common.h"
#include "MatrixCalculate.h"

class ModelEngine;

class Triangle
{
public:
	void Initialize(DirectXCommon* directXCommon, ModelEngine* engine, const Vector4& a, const Vector4& b, const Vector4& c, const DirectionalLight& light);

	void Draw(const Transform& transform, const Transform& cameraTransform, const Vector4& material);

	void Finalize();

private:
	ModelEngine* engine_;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	DirectXCommon* directXCommon_;

	VertexData* vertexData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData_;

	DirectionalLight* directionalLight_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

private:
	void SettingVertex(const Vector4& a, const Vector4& b, const Vector4& c);
	void SetColor();
	void TransformMatrix();
	void CreateDirectionalLight(const DirectionalLight& light);
};