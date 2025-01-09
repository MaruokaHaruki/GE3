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
}

///=============================================================================
///						終了処理
void ClearScene::Finalize() {
}

///=============================================================================
///						更新
void ClearScene::Update() {
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
