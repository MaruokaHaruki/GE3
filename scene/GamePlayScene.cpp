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

}

///=============================================================================
///						終了処理
void GamePlayScene::Finalize() {

}

///=============================================================================
///						更新
void GamePlayScene::Update() {

}

///=============================================================================
///						
void GamePlayScene::Object2DDraw() {

}

///=============================================================================
///						
void GamePlayScene::Object3DDraw() {

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
