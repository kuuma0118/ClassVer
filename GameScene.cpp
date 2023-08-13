#include "GameScene.h"

void GameScene::Initialize(ModelEngine* engine, DirectXCommon* directXCommon) {
	engine_ = engine;
	directXCommon_ = directXCommon;

	data1_[0] = { -0.8f,-0.5f,0.5f,1.0f };
	data2_[0] = { 0.0f,0.0f,0.0f,1.0f };
	data3_[0] = { 0.5f,-0.5f,-0.5f,1.0f };
	material_[0] = { 1.0f,1.0f,1.0f,1.0f };

	data1_[1] = { -0.5f,-0.7f,0.0f,1.0f };
	data2_[1] = { 0.0f,0.7f,0.0f,1.0f };
	data3_[1] = { 0.5f,-0.5f,0.0f,1.0f };
	material_[1] = { 1.0f,1.0f,1.0f,1.0f };

	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };

	spriteLeftTop_[0] = { 0.0f,0.0f,0.0f,1.0f };
	spriteRightDown_[0] = { 320.0f,180.0f,0.0f,1.0f };
	spriteTransform_[0] = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	spriteLeftTop_[1] = { 960.0f,540.0f,0.0f,1.0f };
	spriteRightDown_[1] = { 1280.0f,720.0f,0.0f,1.0f };
	spriteTransform_[1] = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	engine_->LoadTexture("Resources/uvChecker.png");

	for (int i = 0; i < 2; i++) {
		triangle_[i] = new Triangle();
		triangle_[i]->Initialize(directXCommon_, engine_);

		sprite_[i] = new Sprite();
		sprite_[i]->Initialize(directXCommon_);
	}
}

void GameScene::Update() {
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(directXCommon_->GetWin()->kClientWidth) / float(directXCommon_->GetWin()->kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix_, Multiply(viewMatrix, projectionMatrix));

	worldMatrix_ = worldViewProjectionMatrix;

	ImGui::Begin("Window");
	ImGui::DragFloat3("Triangle Scale", &transform_.scale.x, 0.01f);
	ImGui::DragFloat3("Triangle Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Triangle Translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Sprite Scale", &spriteTransform_[0].scale.x, 0.01f);
	ImGui::DragFloat3("Sprite Rotate", &spriteTransform_[0].rotate.x, 0.01f);
	ImGui::DragFloat3("Sprite Translate", &spriteTransform_[0].translate.x, 1.0f);
	ImGui::End();
}

void GameScene::Draw3D() {
	for (int i = 0; i < 2; i++) {
		triangle_[i]->Draw(data1_[i], data2_[i], data3_[i], material_[i], worldMatrix_);
	}
}

void GameScene::Draw2D() {
	for (int i = 0; i < 1; i++) {
		sprite_[i]->Draw(spriteLeftTop_[i], spriteRightDown_[i], spriteTransform_[i]);
	}
}

void GameScene::Finalize() {
	for (int i = 0; i < 2; i++) {
		triangle_[i]->Finalize();
		sprite_[i]->Finalize();
	}
}