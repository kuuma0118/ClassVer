#include "GameScene.h"

void GameScene::Initialize(ModelEngine* engine, DirectXCommon* directXCommon){
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

	spriteMaterial_[0] = { 1.0f,1.0f,1.0f,1.0f };
	sphereTransform_ = { {1.0f,1.0f,1.0f},{0.0f,1.6f,0.0f},{0.0f,0.0f,0.0f} };
	sphereMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	sphere_ = new Sphere();
	sphere_->Initialize(directXCommon_, engine_);
	uvResourceNumber_ = 0;

	engine_->LoadTexture("Resources/uvChecker.png", uvResourceNumber_);

	monsterBallResourceNumber_ = 1;
	engine_->LoadTexture("Resources/monsterBall.png", monsterBallResourceNumber_);

	for (int i = 0; i < 2; i++) {
		triangle_[i] = new Triangle();
		triangle_[i]->Initialize(directXCommon_, engine_);

		sprite_[i] = new Sprite();
		sprite_[i]->Initialize(directXCommon_, engine_);
	}
}

void GameScene::Update() {
	transform_.rotate.y += 0.01f;
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	Matrix4x4 sphereAffine = MakeAffineMatrix(sphereTransform_.scale, sphereTransform_.rotate, sphereTransform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(directXCommon_->GetWin()->kClientWidth) / float(directXCommon_->GetWin()->kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix_, Multiply(viewMatrix, projectionMatrix));

	sphereMatrix_ = Multiply(sphereAffine, Multiply(viewMatrix, projectionMatrix));
	worldMatrix_ = worldViewProjectionMatrix;

	sphereTransform_.rotate.y += 0.01f;

	ImGui::Begin("Window");
	ImGui::DragFloat3("CameraTranslate", &cameraTransform_.translate.x, 0.01f);
	ImGui::DragFloat3("SphereScale", &sphereTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("SphereRotate", &sphereTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("SphereTranslate", &sphereTransform_.translate.x, 0.01f);
	ImGui::End();
}

void GameScene::Draw3D() {
	

	sphere_->Draw(sphereMaterial_, sphereMatrix_, monsterBallResourceNumber_);
}

void GameScene::Draw2D() {
	for (int i = 0; i < 1; i++) {
		sprite_[i]->Draw(spriteLeftTop_[i], spriteRightDown_[i], spriteTransform_[i], spriteMaterial_[0], uvResourceNumber_);
	}
}

void GameScene::Finalize() {
	sphere_->Finalize();

	for (int i = 0; i < 2; i++) {
		triangle_[i]->Finalize();
		sprite_[i]->Finalize();
	}
}