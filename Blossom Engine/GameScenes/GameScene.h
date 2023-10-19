#pragma once

#include "IScene.h"

#include "../Blossom Engine/Math/Vector4.h"
#include "../Blossom Engine/Sys/Sys.h"
#include "../Blossom Engine/Engine/Engine.h"
#include "../ImGui/ImGuiManager.h"
#include "../Blossom Engine/Sprite/Sprite.h"
#include "../Blossom Engine/Sphere/Sphere.h"
#include "../Blossom Engine/Model/Model.h"

class GameManager;
class GameScene : public IScene
{
public:
	// 初期化
	void Initialize()override;

	// 更新処理
	void Update()override;

	// 描画
	void Draw()override;

	void Finalize()override;
private:
	// カメラのポインタ
	DebugCamera* camera_ = nullptr;

	//// マップデータ
	//Map* map_ = nullptr;

	// デバッグ用の変数
	int loadStageNum_ = 0;	// 読み込むステージの番号
};