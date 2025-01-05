/*********************************************************************
 * \file   TitleScene.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "TitleScene.h"

 ///=============================================================================
 ///						初期化
void TitleScene::Initialize(SpriteSetup *spriteSetup, Object3dSetup *object3dSetup, ParticleSetup *particleSetup) {
	//適当に引数を使用
	//引数を使用しない場合は警告を出さないようにする
	spriteSetup;
	object3dSetup;
	particleSetup;
}

///=============================================================================
///						終了処理
void TitleScene::Finalize() {
}

///=============================================================================
///						更新
void TitleScene::Update() {
}

///=============================================================================
///						2D描画
void TitleScene::Object2DDraw() {
}

///=============================================================================
///						3D描画
void TitleScene::Object3DDraw() {
}

///=============================================================================
///						パーティクル描画
void TitleScene::ParticleDraw() {
}

///=============================================================================
///						ImGui描画
void TitleScene::ImGuiDraw() {
	//TitleSceneのImGui描画
	ImGui::Begin("TitleScene");
	ImGui::Text("Hello, TitleScene!");
	ImGui::End();
}
