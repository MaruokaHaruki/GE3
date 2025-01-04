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
	///						 ウィンドウ生成
	win_ = std::make_unique<WinApp>();
	//ウィンドウの生成
	win_->CreateGameWindow(L"GE3");

	///--------------------------------------------------------------
	///						 ダイレクトX生成
	dxCore_ = std::make_unique<DirectXCore>();
	//ダイレクトXの初期化
	dxCore_->InitializeDirectX(win_.get());

	///--------------------------------------------------------------
	///						 ImGuiのセットアップ
	imguiSetup_ = std::make_unique<ImguiSetup>();
	//ImGuiの初期化
	imguiSetup_->Initialize(win_.get(), dxCore_.get(), Style::CYBER);

	///--------------------------------------------------------------
	///						 SrvSetupクラス
	srvSetup_ = std::make_unique<SrvSetup>();
	//SrvSetupの初期化
	srvSetup_->Initialize(dxCore_.get());

	///--------------------------------------------------------------
	///						 入力クラス
	//ユニークポインタ
	input_ = std::make_unique<Input>();
	//入力の初期化
	input_->Initialize(win_->GetWindowClass().hInstance, win_->GetWindowHandle());

	///--------------------------------------------------------------
	///						 音声クラス
	// 初期化
	MAudioG::GetInstance()->Initialize("resources/sound/");
	// ポインタの取得
	audio_ = MAudioG::GetInstance();
	// サウンドセット
	//SoundSet testSound;
	// 音声の読み込み
	audio_->LoadWav("Duke_Ellington.wav");

	///--------------------------------------------------------------
	///						 2D系クラス
	//========================================
	// スプライト共通部
	spriteSetup_ = std::make_unique<SpriteSetup>();
	//スプライト共通部の初期化
	spriteSetup_->Initialize(dxCore_.get());
	//========================================
	// テクスチャマネージャ
	TextureManager::GetInstance()->Initialize(dxCore_.get(), "resources/texture/", srvSetup_.get());
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
	//========================================
	// モデルマネージャの初期化
	ModelManager::GetInstance()->Initialize(dxCore_.get());
	//モデルの読み込み
	ModelManager::GetInstance()->LoadMedel("axisPlus.obj");
	ModelManager::GetInstance()->LoadMedel("Particle.obj");

	//========================================
	// 3Dオブジェクト共通部
	object3dSetup_ = std::make_unique<Object3dSetup>();
	//3Dオブジェクト共通部の初期化
	object3dSetup_->Initialize(dxCore_.get());

	//========================================
	// カメラ
	camera_ = std::make_unique<Camera>();
	//カメラの設定
	object3dSetup_->SetDefaultCamera(camera_.get());

	//========================================
	// 3Dオブジェクトクラス
	object3d_ = std::make_unique<Object3d>();
	//3Dオブジェクトの初期化
	object3d_->Initialize(object3dSetup_.get());
	object3d_->SetModel("axisPlus.obj");

	///--------------------------------------------------------------
	///						 パーティクル系
	// ========================================
	// パーティクルセットアップ
	particleSetup_ = std::make_unique<ParticleSetup>();
	//パーティクルセットアップの初期化
	particleSetup_->Initialize(dxCore_.get(), srvSetup_.get());
	//カメラの設定
	particleSetup_->SetDefaultCamera(camera_.get());

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
	//========================================
	// ImGuiの終了処理
	imguiSetup_->Finalize();
	//========================================
	// audioの終了処理
	audio_->Finalize();
	//========================================
	// テクスチャマネージャの終了処理
	TextureManager::GetInstance()->Finalize();
	//========================================
	// モデルマネージャの終了処理
	ModelManager::GetInstance()->Finalize();
	//========================================
	// ダイレクトX
	dxCore_->ReleaseDirectX();
	//========================================
	// ウィンドウの終了
	win_->CloseWindow();
}

///=============================================================================
///						更新
void MaruRhythm::Update() {
	//========================================
	// インプットの更新
	input_->Update();

	///--------------------------------------------------------------
	///						更新処理
	//========================================
	// カメラの更新
	//camera->SetTransform(Transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-100.0f} });
	camera_->Update();

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
	/*if(audio->IsWavPlaying(testSound.voiceHandle) == false) {
	testSound.voiceHandle = audio->PlayWav(testSound.dataHandle, 1.0f, 1.0f, 1.0f);

	}*/
	if(audio_->IsWavPlaying("Duke_Ellington.wav") == false) {
		audio_->PlayWav("Duke_Ellington.wav", true, 1.0f, 1.0f);
	}

	//========================================
	// ImGuiの更新
	imguiSetup_->Begin();
	//↓この間に書け!!!
	//DEMOウィンドウの表示
	ImGui::ShowDemoWindow();

	ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_Once);
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
	imguiSetup_->End();
}

///=============================================================================
///						描画
void MaruRhythm::Draw() {
	///--------------------------------------------------------------
	///						 描画(コマンドを積む)
	///---------------------------------------
	/// ループ前処理
	dxCore_->PreDraw();
	srvSetup_->PreDraw();

	//========================================
	//3Dオブジェクト共通描画設定
	object3dSetup_->CommonDrawSetup();
	// 3D描画
	object3d_->Draw();

	//========================================
	// 2Dオブジェクト共通描画設定
	spriteSetup_->CommonDrawSetup();

	//Spriteクラス
	sprite_->Draw();

	//========================================
	//パーティクル共通描画設定
	particleSetup_->CommonDrawSetup();
	//パーティクル描画
	particle_->Draw();

	//========================================
	// ImGui描画
	imguiSetup_->Draw();

	///---------------------------------------
	///ループ後処理
	dxCore_->PostDraw();

}
