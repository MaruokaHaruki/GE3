/*********************************************************************
 * \file   ClearScene.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "ClearScene.h"
#include "Input.h"

///=============================================================================
///						初期化
void ClearScene::Initialize(SpriteSetup *spriteSetup, Object3dSetup *object3dSetup, ParticleSetup *particleSetup) {
	//適当に引数を使用
	//引数を使用しない場合は警告を出さないようにする
	spriteSetup;
	object3dSetup;
	particleSetup;

	TextureManager::GetInstance()->LoadTexture("gameClear.png");
	TextureManager::GetInstance()->LoadTexture("press.png");

	//========================================
	// スプライトクラス(Game)
	//ユニークポインタ
	sprite_ = std::make_unique<Sprite>();
	//スプライトの初期化
	sprite_->Initialize(spriteSetup, "gameClear.png");
	//サイズ
	sprite_->SetSize({ 1280.0f,720.0f });

	//========================================
	// スプライトクラス(Game)
	//ユニークポインタ
	pressSprite_ = std::make_unique<Sprite>();
	//スプライトの初期化
	pressSprite_->Initialize(spriteSetup, "press.png");
	//


}

///=============================================================================
///						終了処理
void ClearScene::Finalize() {
}

///=============================================================================
///						更新
void ClearScene::Update() {
	//========================================
	// 2D更新
	//座標のセット
	sprite_->SetPosition(Vector2{ transformSprite.translate.x, transformSprite.translate.y });
	//単体
	sprite_->Update();

	//=======================================
	//画面中央下に配置
	pressSprite_->SetPosition(Vector2{ 400.0f, 400.0f });
	pressSprite_->Update();

	//========================================
	// シーン遷移
	if(Input::GetInstance()->PushKey(DIK_SPACE)) {
		BaseScene::sceneNo = TITLE;
	}
	//コントローラ
	if(Input::GetInstance()->TriggerButton(Input::BUTTON_A)) {
		BaseScene::sceneNo = TITLE;
	}
}

///=============================================================================
///						2D描画
void ClearScene::Object2DDraw() {
	//スプライトの描画
	sprite_->Draw();

	pressSprite_->Draw();
}

///=============================================================================
///						3D描画
void ClearScene::Object3DDraw() {
}

///=============================================================================
///						パーティクル描画
void ClearScene::ParticleDraw() {
}

///=============================================================================
///						ImGui描画
void ClearScene::ImGuiDraw() {
#ifdef DEBUG
	//ClearSceneのImGui描画
	ImGui::Begin("ClearScene");
	ImGui::Text("Hello, ClearScene!");
	ImGui::End();
#endif // DEBUG
}
