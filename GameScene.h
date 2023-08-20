#pragma once

#include "Vector4.h"
#include "Sys.h"
#include "Engine.h"
#include "ImGuiManager.h"
#include "Sprite.h"
#include "Sphere.h"

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

	Vector4 spriteLeftTop_[2];
	Vector4 spriteRightDown_[2];

	Transform spriteTransform_[2];

	Vector4 spriteMaterial_[2];
	Sphere* sphere_;
	Transform sphereTransform_;
	Vector4 sphereMaterial_;
	Matrix4x4 sphereMatrix_;

	Vector4 data1_[2];
	Vector4 data2_[2];
	Vector4 data3_[2];
	Vector4 material_[2];

	Triangle* triangle_[2];
	Transform transform_;

	Matrix4x4 worldMatrix_;
	Transform cameraTransform_;
	DirectionalLight directionalLight_;
	uint32_t uvResourceNumber_;
	int monsterBallResourceNumber_;
};