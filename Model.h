#pragma once

#include "Common.h"
#include "Vector4.h"
#include "Engine.h"
#include <string>

class Model {
public:
	void Initialize(DirectXCommon* directXCommon, ModelEngine* engine, const std::string& directoryPath, const std::string& fileName, uint32_t index);

	void Draw(const Vector4& material, const Transform& transform, uint32_t texIndex, const Transform& cameraTransform, const DirectionalLight& light);

	void Finalize();

	ModelData modelData_;

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& fileName);
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName);

private:
	DirectXCommon* directXCommon_;
	Material* material_;
	ModelEngine* engine_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	ID3D12Resource* vertexResource;
	VertexData* vertexData_;

	DirectionalLight* directionalLight_;
	ID3D12Resource* directionalLightResource_;
	ID3D12Resource* wvpResource_;
	TransformationMatrix* wvpData_;
	ID3D12Resource* materialResource_;

private:
	void CreateVertexData();
	void SetColor();
	void TransformMatrix();
	void CreateDirectionalLight();
};