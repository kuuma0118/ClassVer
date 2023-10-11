#pragma once

#include <../Blossom Engine/Common/Common.h>
#include <../Blossom Engine/Math/Vector4.h>
#include <../Blossom Engine/Engine/Engine.h>
#include <string>

class Model {
public:
	void Initialize(DirectXCommon* directXCommon, ModelEngine* engine, const std::string& directoryPath, const std::string& fileName, uint32_t index, const DirectionalLight& light);

	void Draw(const Vector4& material, const Transform& transform, uint32_t texIndex, const Transform& cameraTransform);

	void Finalize();

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

private:
	DirectXCommon* directXCommon_;
	ModelEngine* engine_;

	ModelData modelData_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	Material* material_;

	DirectionalLight* directionalLight_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

private:
	void CreateVertexData();
	void SetColor();
	void TransformMatrix();
	void CreateDirectionalLight(const DirectionalLight& light);
};