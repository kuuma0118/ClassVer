#pragma once

#include <../Blossom Engine/Math/Vector4.h>
#include <../Blossom Engine/Sys/Sys.h>
#include <../Blossom Engine/Engine/Engine.h>
#include <../ImGui/ImGuiManager.h>
#include <../Blossom Engine/Sprite/Sprite.h>
#include <../Blossom Engine/Sphere/Sphere.h>
#include <../Blossom Engine/Model/Model.h>

struct TriangleVertex {
	Vector4 v1;
	Vector4 v2;
	Vector4 v3;
};

class GameScene {
public:
	void Initialize(ModelEngine* engine, DirectXCommon* directXCommon);
	void Update();
	void Draw2D();
	void Draw3D();
	void Finalize();

private:
	ModelEngine* engine_;
	DirectXCommon* directXCommon_;

	Sprite* sprite_[2];

	Vector4 spriteDataLeftTop_[2];
	Vector4 spriteDataRightDown_[2];

	Transform spriteTransform_[2];
	Vector4 spriteMaterial_[2];

	Sphere* sphere_;
	Transform sphereTransform_;
	Vector4 sphereMaterial_;
	Matrix4x4 sphereMatrix_;

	Transform spriteUvTransform_;

	TriangleVertex triangleVertex_[2];
	Vector4 triangleMaterial_[2];

	Triangle* triangle_[2];
	Transform transform_;

	Transform modelTransform_;
	Vector4 modelMaterial_;

	Matrix4x4 worldMatrix_;
	Transform cameraTransform_;

	DirectionalLight directionalLight_;

	uint32_t uvResourceNumber_;
	int monsterBallResourceNumber_;

	Model* model_;
};