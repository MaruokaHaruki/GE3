/*********************************************************************
 * \file   TitleScene.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "TitleScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "CameraManager.h"
#include "MAudioG.h"

 ///=============================================================================
 ///						初期化
void TitleScene::Initialize(SpriteSetup *spriteSetup, Object3dSetup *object3dSetup, ParticleSetup *particleSetup) {
	//適当に引数を使用
	//引数を使用しない場合は警告を出さないようにする
	spriteSetup;
	object3dSetup;
	particleSetup;
	//========================================
	// カメラ設定
	CameraManager::GetInstance()->GetCamera("DefaultCamera")->SetTransform({ {1.0f,1.0f,1.0f},{0.2f,0.0f,0.0f},{0.0f,4.0f,-16.0f} });

	//========================================
	// 音の読み込み
	MAudioG::GetInstance()->LoadWav("se_charge.wav");
	MAudioG::GetInstance()->LoadWav("se_damage.wav");
	MAudioG::GetInstance()->LoadWav("se_exlpo.wav");
	MAudioG::GetInstance()->LoadWav("se_samon.wav");
	MAudioG::GetInstance()->LoadWav("se_select.wav");
	MAudioG::GetInstance()->LoadWav("se_shot.wav");
	MAudioG::GetInstance()->LoadWav("Refine.wav");
	MAudioG::GetInstance()->LoadWav("Beast-Mode.wav");

	//========================================
	// モデルの読み込み
	ModelManager::GetInstance()->LoadMedel("title.obj");
	TextureManager::GetInstance()->LoadTexture("press.png");

	//========================================
	// タイトルオブジェクト
	objTitle_ = std::make_unique<Object3d>();
	objTitle_->Initialize(object3dSetup);
	objTitle_->SetModel("title.obj");

	//========================================
	// スプライトクラス(Game)
	//ユニークポインタ
	pressSprite_ = std::make_unique<Sprite>();
	//スプライトの初期化
	pressSprite_->Initialize(spriteSetup, "press.png");
	pressSprite_->SetPosition(Vector2{ 400.0f, 400.0f });

}

///=============================================================================
///						終了処理
void TitleScene::Finalize() {
	//曲の終了
	MAudioG::GetInstance()->StopWav("Refine.wav");
}

///=============================================================================
///						更新
void TitleScene::Update() {
	//========================================
	// 曲を再生
	if(MAudioG::GetInstance()->IsWavPlaying("Refine.wav") == false){
		MAudioG::GetInstance()->PlayWav("Refine.wav", true);
	}
	//タイトルを左右に回転させて揺らす
	angle += 0.04f;
	transform.rotate.y = sin(angle) * 0.4f;

	//=======================================
	//画面中央下に配置
	pressSprite_->SetPosition(Vector2{ 400.0f, 400.0f });
	pressSprite_->Update();


	//========================================
	// Object3D
	objTitle_->SetTransform(transform);
	objTitle_->Update();


	//========================================
	// シーン遷移
	if(Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		sceneNo = SCENE::GAMEPLAY;
	}
	//コントローラ
	if(Input::GetInstance()->TriggerButton(Input::BUTTON_A)) {
		sceneNo = SCENE::GAMEPLAY;
	}
}

///=============================================================================
///						2D描画
void TitleScene::Object2DDraw() {
}

///=============================================================================
///						3D描画
void TitleScene::Object3DDraw() {
	//========================================
	// タイトル
	objTitle_->Draw();

	//========================================
	// スプライト
	pressSprite_->Draw();
}

///=============================================================================
///						パーティクル描画
void TitleScene::ParticleDraw() {
}

///=============================================================================
///						ImGui描画
void TitleScene::ImGuiDraw() {
#ifdef _DEBUG
	//TitleSceneのImGui描画
	ImGui::Begin("TitleScene");
	ImGui::Text("Hello, TitleScene!");
	ImGui::End();
#endif // DEBUG
}
