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
