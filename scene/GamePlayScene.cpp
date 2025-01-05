/*********************************************************************
 * \file   GamePlayScene.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "GamePlayScene.h"

///=============================================================================
///						初期化
void GamePlayScene::Initialize(SpriteSetup* spriteSetup, Object3dSetup* object3dSetup, ParticleSetup* particleSetup) {
	//適当に引数を使用
	//引数を使用しない場合は警告を出さないようにする
	spriteSetup;
	object3dSetup;
	particleSetup;

	//========================================
	// プレイヤー
	object3d_ = std::make_unique<Object3d>();
	//3Dオブジェクトの初期化
	object3d_->Initialize(object3dSetup);
	object3d_->SetModel("axisPlus.obj");
	//プレイヤーの初期化
	player_ = std::make_unique<Player>();
	player_->Initialize(object3d_.get());
}

///=============================================================================
///						終了処理
void GamePlayScene::Finalize() {

}

///=============================================================================
///						更新
void GamePlayScene::Update() {
	//========================================
	// プレイヤー
	player_->Update();

}

///=============================================================================
///						
void GamePlayScene::Object2DDraw() {

}

///=============================================================================
///						
void GamePlayScene::Object3DDraw() {
	//========================================
	// プレイヤー
	player_->Draw();
}

///=============================================================================
///						
void GamePlayScene::ParticleDraw() {

}

///=============================================================================
///						ImGui描画
void GamePlayScene::ImGuiDraw() {
	ImGui::Begin("DebugScene");
	ImGui::Text("Hello, GamePlayScene!");
	ImGui::End();
}
