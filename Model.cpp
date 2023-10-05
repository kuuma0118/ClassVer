#include "Model.h"
#include <fstream>
#include <sstream>

void Model::Initialize(DirectXCommon* directXCommon, ModelEngine* engine, const std::string& directoryPath, const std::string& fileName, uint32_t index, const DirectionalLight& light) {
	directXCommon_ = directXCommon;
	engine_ = engine;

	modelData_ = LoadObjFile(directoryPath, fileName);
	engine_->LoadTexture(modelData_.material.textureFilePath, index);

	CreateVertexData();
	SetColor();
	TransformMatrix();
	CreateDirectionalLight(light);
}

void Model::Draw(const Vector4& material, const Transform& transform, uint32_t texIndex, const Transform& cameraTransform) {
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(directXCommon_->GetWin()->kClientWidth) / float(directXCommon_->GetWin()->kClientHeight), 0.1f, 100.0f);

	Matrix4x4 wvpMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	Transform uvTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));

	*material_ = { material,false };
	material_->uvTransform = uvTransformMatrix;

	*wvpData_ = { wvpMatrix,worldMatrix };

	directXCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	directXCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	directXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, engine_->textureSrvHandleGPU_[texIndex]);
	directXCommon_->GetCommandList()->DrawInstanced(UINT(modelData_.verticles.size()), 1, 0, 0);
}

void Model::Finalize() {

}

ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& fileName) {
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;

	std::ifstream file(directoryPath + "/" + fileName);

	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;

			position.z *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;

			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;

			normal.z *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];

			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				std::istringstream v(vertexDefinition);
				uint32_t elementIndeices[3];

				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');

					elementIndeices[element] = std::stoi(index);
				}

				Vector4 position = positions[elementIndeices[0] - 1];
				Vector2 texcoord = texcoords[elementIndeices[1] - 1];
				Vector3 normal = normals[elementIndeices[2] - 1];

				VertexData vertex = { position,texcoord,normal };
				modelData.verticles.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}

			modelData.verticles.push_back(triangle[2]);
			modelData.verticles.push_back(triangle[1]);
			modelData.verticles.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			std::string materialFileName;
			s >> materialFileName;

			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFileName);
		}
	}

	return modelData;
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName) {
	MaterialData materialData;

	std::string line;
	std::ifstream file(directoryPath + "/" + fileName);

	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);

		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFileName;
			s >> textureFileName;

			materialData.textureFilePath = directoryPath + "/" + textureFileName;
		}
	}

	return materialData;
}

void Model::CreateVertexData() {
	vertexResource = directXCommon_->CreateBufferResource(directXCommon_->GetDevice().Get(), sizeof(VertexData) * modelData_.verticles.size());

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * (UINT)modelData_.verticles.size();
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	std::memcpy(vertexData_, modelData_.verticles.data(), sizeof(VertexData) * modelData_.verticles.size());
}

void Model::SetColor() {
	materialResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice().Get(), sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));

	material_->uvTransform = MakeIdentity4x4();
}

void Model::TransformMatrix() {
	wvpResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice().Get(), sizeof(TransformationMatrix));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));

	wvpData_->WVP = MakeIdentity4x4();
}

void Model::CreateDirectionalLight(const DirectionalLight& light) {
	directionalLightResource_ = DirectXCommon::CreateBufferResource(directXCommon_->GetDevice().Get(), sizeof(DirectionalLight));
	directionalLightResource_->Map(0, NULL, reinterpret_cast<void**>(&directionalLight_));

	*directionalLight_ = light;
}