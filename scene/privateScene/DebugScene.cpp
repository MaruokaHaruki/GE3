/*********************************************************************
 * \file   DebugScene.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "DebugScene.h"

///=============================================================================
///						初期化
void DebugScene::Initialize(SpriteSetup *spriteSetup, Object3dSetup *object3dSetup, ParticleSetup *particleSetup) {
	///--------------------------------------------------------------
	///						 音声クラス

	///--------------------------------------------------------------
	///						 2D系クラス
	////========================================
	//// テクスチャマネージャ

	//========================================
	// スプライトクラス(Game)


	///--------------------------------------------------------------
	///						 3D系クラス
	//モデルの読み込み
	ModelManager::GetInstance()->LoadMedel("axisPlus.obj");
	ModelManager::GetInstance()->LoadMedel("ball.obj");
	//========================================
	// 3Dオブジェクトクラス
	object3d_ = std::make_unique<Object3d>();
	//3Dオブジェクトの初期化
	object3d_->Initialize(object3dSetup);
	object3d_->SetModel("ball.obj");

	//========================================
	// ライト情報の取得
	lightColor = object3d_->GetDirectionalLight().color;
	lightDirection = object3d_->GetDirectionalLight().direction;
	lightIntensity = object3d_->GetDirectionalLight().intensity;

	///--------------------------------------------------------------
	///						 パーティクル系
	//========================================
	// パーティクルクラス
	
	//========================================
	// パーティクルエミッター
}

///=============================================================================
///						終了処理
void DebugScene::Finalize() {
}

///=============================================================================
///						更新
void DebugScene::Update() {
	///--------------------------------------------------------------
	///						更新処理
	//========================================
	// カメラの更新
	//CameraManager::GetInstance()->UpdateAll();

	//========================================
	// 2D更新


	//========================================
	// 3D更新 
	//大きさのセット
	object3d_->SetScale(Vector3{ transform.scale.x,transform.scale.y,transform.scale.z });
	//回転のセット
	object3d_->SetRotation(Vector3{ transform.rotate.x,transform.rotate.y,transform.rotate.z });
	//座標のセット
	object3d_->SetPosition(Vector3{ transform.translate.x,transform.translate.y,transform.translate.z });
	//更新
	object3d_->Update();

	//========================================
	// パーティクル系


	//========================================
	// 音声の再生

}

///=============================================================================
///						2D描画
void DebugScene::Object2DDraw() {

}

///=============================================================================
///						3D描画
void DebugScene::Object3DDraw() {
	//========================================
	// 3D描画
	object3d_->Draw();
}

///=============================================================================
///						パーティクル描画
void DebugScene::ParticleDraw() {

}

///=============================================================================
///						ImGui描画
void DebugScene::ImGuiDraw() {
	//DebugSceneのImGui描画
	ImGui::Begin("DebugScene");
	ImGui::Text("Hello, DebugScene!");
	ImGui::End();


	//========================================
	// 3DオブジェクトのImGui描画
	//ライトの設定
	ImGui::Begin("Light");
	//ライトの色
	ImGui::ColorEdit4("LightColor", &lightColor.x);
	//ライトの方向
	ImGui::SliderFloat3("LightDirection", &lightDirection.x, -1.0f, 1.0f);
	//ライトの強度
	ImGui::SliderFloat("LightIntensity", &lightIntensity, 0.0f, 10.0f);
	//ライトの設定
	object3d_->SetDirectionalLight(lightColor, lightDirection, lightIntensity);
	ImGui::End();

}
