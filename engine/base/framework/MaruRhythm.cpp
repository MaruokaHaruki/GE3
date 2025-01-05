/*********************************************************************
 * \file   MaruRhythm.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#include "MaruRhythm.h"

///=============================================================================
///						初期化
void MaruRhythm::Initialize() {
	///--------------------------------------------------------------
	///						 フレームワーク初期化
	MRFramework::Initialize();

	///--------------------------------------------------------------
	///						 ゲームプレイシーン初期化
	//gamePlayScene_ = std::make_unique<GamePlayScene>();
	//gamePlayScene_->Initialize(spriteSetup_.get(), object3dSetup_.get(), particleSetup_.get());
	//ユニークポインタ
	//std::unique_ptr<BaseScene> gamePlayScene_ = std::make_unique<GamePlayScene>();
	//sceneManager_->SetNextScene(gamePlayScene_.get);
}

///=============================================================================
///						終了処理
void MaruRhythm::Finalize() {
	//========================================
	// ゲームプレイ
	//gamePlayScene_->Finalize();
	CameraManager::GetInstance()->Finalize();///
	//========================================
	// フレームワークの終了処理
	MRFramework::Finalize();
}

///=============================================================================
///						更新
void MaruRhythm::Update() {
	//========================================
	// 更新処理
	MRFramework::Update();

	///--------------------------------------------------------------
	///						更新処理
	//========================================
	// カメラの更新
	CameraManager::GetInstance()->UpdateAll();

	//========================================
	// ゲームシーンの更新
	//gamePlayScene_->Update();

	//========================================
	// ImGuiの更新
	MRFramework::ImGuiPreDraw();
	//↓この間に書け!!!
	//gamePlayScene_->ImGuiDraw();
	//↑
	MRFramework::ImGuiPostDraw();
}

///=============================================================================
///						描画
void MaruRhythm::Draw() {
	//========================================
	// ループ前処理
	MRFramework::FrameworkPreDraw();

	//========================================
	//3Dオブジェクト共通描画設定
	MRFramework::Object3DCommonDraw();
	//ゲームプレイシーンの3D描画
	//gamePlayScene_->Object3DDraw();

	//========================================
	// 2Dオブジェクト共通描画設定
	MRFramework::Object2DCommonDraw();
	//ゲームプレイシーンの2D描画
	//gamePlayScene_->Object2DDraw();

	//========================================
	//パーティクル共通描画設定
	MRFramework::ParticleCommonDraw();
	//ゲームプレイシーンのパーティクル描画
	//gamePlayScene_->ParticleDraw();

	//========================================
	// ループ後処理
	MRFramework::FrameworkPostDraw();
}
