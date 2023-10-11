#include "GameScene.h"

void GameScene::Initialize(ModelEngine* engine, DirectXCommon* directXCommon) {
	engine_ = engine;
	directXCommon_ = directXCommon;

	triangleVertex_[0].v1 = { -0.5f,-0.5f,0.5f,1.0f };
	triangleVertex_[0].v2 = { 0.0f,0.0f,0.0f,1.0f };
	triangleVertex_[0].v3 = { 0.5f,-0.5f,-0.5f,1.0f };
	triangleMaterial_[0] = { 1.0f,1.0f,1.0f,1.0f };

	triangleVertex_[1].v1 = { -0.5f,-0.5f,0.0f,1.0f };
	triangleVertex_[1].v2 = { 0.0f,0.5f,0.0f,1.0f };
	triangleVertex_[1].v3 = { 0.5f,-0.5f,0.0f,1.0f };
	triangleMaterial_[1] = { 1.0f,1.0f,1.0f,1.0f };

	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	spriteDataLeftTop_[0] = { 0.0f,0.0f,0.0f,1.0f };
	spriteDataRightDown_[0] = { 320.0f,180.0f,0.0f,1.0f };
	spriteTransform_[0] = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	spriteDataLeftTop_[1] = { 960.0f,540.0f,0.0f,1.0f };
	spriteDataRightDown_[1] = { 1280.0f,720.0f,0.0f,1.0f };
	spriteTransform_[1] = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	spriteMaterial_[0] = { 1.0f,1.0f,1.0f,1.0f };

	sphereTransform_ = { {1.0f,1.0f,1.0f},{0.0f,1.6f,0.0f},{0.0f,0.0f,0.0f} };
	sphereMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

	modelTransform_ = { {1.0f,1.0f,1.0f},{0.0f,1.6f,0.0f},{0.0f,0.0f,0.0f} };
	modelMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

	spriteUvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f} };

	/*sphere_ = new Sphere();
	sphere_->Initialize(directXCommon_, engine_, directionalLight_);*/

	model_ = new Model();
	model_->Initialize(directXCommon_, engine_, "Resources", "axis.obj", 0, directionalLight_);

	/*uvResourceNumber_ = 0;
	engine_->LoadTexture("Resource/uvChecker.png",uvResourceNumber_);*/

	monsterBallResourceNumber_ = 1;
	engine_->LoadTexture("Resources/monsterBall.png", monsterBallResourceNumber_);

	directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},1.0f };

	for (int i = 0; i < 2; i++) {
		triangle_[i] = new Triangle();
		triangle_[i]->Initialize(directXCommon_, engine_, triangleVertex_[i].v1, triangleVertex_[i].v2, triangleVertex_[i].v3, directionalLight_);

		sprite_[i] = new Sprite();
		sprite_[i]->Initialize(directXCommon_, engine_, spriteDataLeftTop_[0], spriteDataRightDown_[0], directionalLight_);
	}
}

void GameScene::Update() {
	transform_.rotate.y += 0.01f;
	directionalLight_.direction = Normalize(directionalLight_.direction);

	ImGui::Begin("Window");
	ImGui::DragFloat3("CameraTranslate", &cameraTransform_.translate.x, 0.01f);
	ImGui::DragFloat3("ModelMaterial", &modelMaterial_.x, 0.01f);
	ImGui::DragFloat3("ModelScale", &modelTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("ModelRotate", &modelTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("ModelTranslate", &modelTransform_.translate.x, 0.01f);
	ImGui::End();
}

void GameScene::Draw3D() {
	for (int i = 0; i < 2; i++) {
		triangle_[i]->Draw(transform_, cameraTransform_, triangleMaterial_[i]);
	}

	model_->Draw(modelMaterial_, modelTransform_, 0, cameraTransform_);
	//sphere_->Draw(sphereMaterial_, sphereTransform_, 1, cameraTransform_);
}

void GameScene::Draw2D() {
	sprite_[0]->Draw(spriteTransform_[0], spriteUvTransform_, spriteMaterial_[0], 0);
}

void GameScene::Finalize() {
	delete model_;
	//delete sphere_;

	for (int i = 0; i < 2; i++) {
		delete triangle_[i];
		delete sprite_[i];
	}
}