#include "SceneManager.h"
#include "ImguiSetup.h"
// public:
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "ClearScene.h"
// private:
#include "DebugScene.h"

///=============================================================================
///						初期化
void SceneManager::Initialize(SpriteSetup *spriteSetup, Object3dSetup *object3dSetup, ParticleSetup *particleSetup) {
	//========================================
	// 2D共通部
	spriteSetup_ = spriteSetup;
	// 3D共通部
	object3dSetup_ = object3dSetup;
	// パーティクル共通部
	particleSetup_ = particleSetup;
	// 初期シーンを設定（例としてDebugSceneを設定）
	nowScene_ = std::make_unique<DebugScene>();
	nowScene_->Initialize(spriteSetup_, object3dSetup_, particleSetup_);

	// シーンの初期設定
	currentSceneNo_ = 0;
	prevSceneNo_ = -1;
}

///=============================================================================
///						終了処理
void SceneManager::Finalize() {
	if(nowScene_) {
		nowScene_->Finalize();
	}
}

///=============================================================================
///						更新
void SceneManager::Update() {
	// TODO:シーン切り替え機構

	//========================================
	// シーンのチェック
	prevSceneNo_ = currentSceneNo_;
	currentSceneNo_ = nowScene_->GetSceneNo();

	//========================================
	// 次のシーンがある場合
	if(prevSceneNo_ != currentSceneNo_) {
		//---------------------------------------
		// 旧シーンの終了処理
		if(nowScene_ != nullptr) {
			nowScene_->Finalize();
		}
		//---------------------------------------
		// 新シーンの初期化
		if(currentSceneNo_ == DEBUG) {
			nowScene_ = std::make_unique<DebugScene>();
		} else if(currentSceneNo_ == GAMEPLAY) {
			nowScene_ = std::make_unique<GamePlayScene>();
		} else if(currentSceneNo_ == TITLE) {
			nowScene_ = std::make_unique<TitleScene>();
		} else if(currentSceneNo_ == CLEAR) {
			nowScene_ = std::make_unique<ClearScene>();
		}
		// シーンの初期化
		nowScene_->Initialize(spriteSetup_, object3dSetup_, particleSetup_);
	}

	//========================================
	// シーンの更新
	if(nowScene_) {
		nowScene_->Update();
	}
}

///=============================================================================
///						2D描画
void SceneManager::Object2DDraw() {
	if(nowScene_) {
		nowScene_->Object2DDraw();
	}
}

///=============================================================================
///						3D描画
void SceneManager::Object3DDraw() {
	if(nowScene_) {
		nowScene_->Object3DDraw();
	}
}

///=============================================================================
///						パーティクル描画
void SceneManager::ParticleDraw() {
	if(nowScene_) {
		nowScene_->ParticleDraw();
	}
}

///=============================================================================
///						ImGui描画
void SceneManager::ImGuiDraw() {
	//========================================

	if(nowScene_) {
		nowScene_->ImGuiDraw();
	}
	//========================================
	// シーンを切り替えるボタン
	//publicScene
	ImGui::Begin("SceneChange");
	ImGui::Text("publicScene");
	if(ImGui::Button("TitleScene")) {
		nowScene_->SetSceneNo(TITLE);
	}
	if(ImGui::Button("GamePlayScene")) {
		nowScene_->SetSceneNo(GAMEPLAY);
	}
	if(ImGui::Button("ClearScene")) {
		nowScene_->SetSceneNo(CLEAR);
	}
	//========================================
	// privateScene
	ImGui::Separator();
	ImGui::Text("privateScene");
	if(ImGui::Button("DebugScene")) {
		nowScene_->SetSceneNo(DEBUG);
	}

	ImGui::End();
}
