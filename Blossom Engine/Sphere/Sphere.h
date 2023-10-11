#pragma once

#include <../Blossom Engine/Common/Common.h>
#include <../Blossom Engine/Math/MatrixCalculate.h>
#include <../Blossom Engine/Engine/Engine.h>

class Sphere {
public:
	void Initialize(DirectXCommon* directXCommon, ModelEngine* engine, const DirectionalLight& light);

	void Draw(const Vector4& material, const Transform& transform, uint32_t texIndex, const Transform& cameratransform);

	void Finalize();

private:
	ModelEngine* engine_;
	DirectXCommon* directXCommon_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_;

	const float pi = 3.1415f;

	uint32_t kSubDivision;
	uint32_t vertexCount;

	DirectionalLight* directionalLight_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

private:
	void CreateVertexData();
	void SetColor();
	void TransformMatrix();
	void CreateDirectionalLight(const DirectionalLight& light);
};