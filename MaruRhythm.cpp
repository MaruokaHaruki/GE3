/*********************************************************************
 * \file   MaruRhythm.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#include "MaruRhythm.h"
//========================================
// Framework
#include "WinApp.h"
#include "DirectXCore.h"
#include "ImguiSetup.h"
#include "SrvSetup.h"
#include "Input.h"
#include "MAudioG.h"
#include "SpriteSetup.h"
#include "TextureManager.h"
#include "ParticleSetup.h"
#include "Object3dSetup.h"
#include "ModelManager.h"

///=============================================================================
///						初期化
void MaruRhythm::Initialize() {
	///--------------------------------------------------------------
	///						 フレームワーク初期化
	MRFramework::Initialize();

	///--------------------------------------------------------------
	///						 音声クラス
	// ポインタの取得
	audio_ = MAudioG::GetInstance();
	// 音声の読み込み
	audio_->LoadWav("Duke_Ellington.wav");

	///--------------------------------------------------------------
	///						 2D系クラス
	////========================================
	//// テクスチャマネージャ
	TextureManager::GetInstance()->LoadTexture("uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("monsterBall.png");
	//========================================
	// スプライトクラス(Game)
	//ユニークポインタ
	sprite_ = std::make_unique<Sprite>();
	//スプライトの初期化
	sprite_->Initialize(spriteSetup_.get(), "uvChecker.png");
	//サイズ
	sprite_->SetSize({ 256.0f,256.0f });

	///--------------------------------------------------------------
	///						 3D系クラス
	//モデルの読み込み
	ModelManager::GetInstance()->LoadMedel("axisPlus.obj");
	ModelManager::GetInstance()->LoadMedel("Particle.obj");
	//========================================
	// 3Dオブジェクトクラス
	object3d_ = std::make_unique<Object3d>();
	//3Dオブジェクトの初期化
	object3d_->Initialize(object3dSetup_.get());
	object3d_->SetModel("axisPlus.obj");

	///--------------------------------------------------------------
	///						 パーティクル系
	//========================================
	// パーティクルクラス
	particle_ = std::make_unique<Particle>();
	//パーティクルの初期化
	particle_->Initialize(particleSetup_.get());
	//パーティクルグループの作成
	particle_->CreateParticleGroup("Particle", "monsterBall.png");

	//========================================
	// パーティクルエミッター
	particleEmitter_ =
		std::make_unique<ParticleEmitter>(particle_.get(),
			"Particle",
			Transform{ {0.2f,0.2f,0.2f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} },
			100,
			0.1f,
			true);
}

///=============================================================================
///						終了処理
void MaruRhythm::Finalize() {
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
	// 2D更新
	//大きさのセット
	sprite_->SetSize(Vector2{ transformSprite.scale.x,transformSprite.scale.y });
	//回転のセット
	sprite_->SetRotation(transformSprite.rotate.x);
	//座標のセット
	sprite_->SetPosition(Vector2{ transformSprite.translate.x, transformSprite.translate.y });
	//単体
	sprite_->Update();

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
	//パーティクルの更新
	particle_->Update();
	//パーティクルエミッターの更新
	particleEmitter_->Update();

	//========================================
	// 音声の再生
	if(audio_->IsWavPlaying("Duke_Ellington.wav") == false) {
		audio_->PlayWav("Duke_Ellington.wav", true, 1.0f, 1.0f);
	}

	//========================================
	// ImGuiの更新
	MRFramework::ImGuiPreDraw();
	//↓この間に書け!!!

	//DEMOウィンドウの表示
	ImGui::ShowDemoWindow();
	ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_Once);
	//2Dオブジェクトのウィンドウ
	ImGui::Begin("2D Object");
	// カラーピッカーを表示
	ImGui::Text("2D Material Settings");
	ImGui::ColorPicker4("Color", reinterpret_cast<float *>( &materialSprite.x ), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);
	//空白と罫線
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Separator();
	//SpriteのSRT設定
	ImGui::Text("2D Object");
	ImGui::DragFloat2("Scale", &transformSprite.scale.x, 0.01f);
	ImGui::DragFloat("Rotate", &transformSprite.rotate.x, 0.01f);
	ImGui::DragFloat3("Translate", &transformSprite.translate.x, 1.0f);
	ImGui::End();

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
	// 3D描画
	object3d_->Draw();

	//========================================
	// 2Dオブジェクト共通描画設定
	MRFramework::Object2DCommonDraw();
	//Spriteクラス
	sprite_->Draw();

	//========================================
	//パーティクル共通描画設定
	MRFramework::ParticleCommonDraw();
	//パーティクル描画
	particle_->Draw();

	//========================================
	// ループ後処理
	MRFramework::FrameworkPostDraw();
}
