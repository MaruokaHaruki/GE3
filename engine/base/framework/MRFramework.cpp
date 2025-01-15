/*********************************************************************
 * \file   MRFramework.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#include "MRFramework.h"
#include "WinApp.h"

///=============================================================================
///						実行
void MRFramework::Run() {
	//========================================
	// 初期化
	Initialize();
	//========================================
	// メインループ
	MSG msg{};
	// メッセージがなくなるまでループ
	while(msg.message != WM_QUIT) {
		// メッセージがあれば処理
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// メッセージ処理
			TranslateMessage(&msg);
			// メッセージ処理
			DispatchMessage(&msg);
		} else {
			//---------------------------------------
			// 更新
			Update();
			//---------------------------------------
			// 終了リクエストがあれば終了
			if(IsEndRequest()) {
				break;
			}
			//---------------------------------------
			// 描画
			Draw();
		}
	}
	//========================================
	// 終了処理
	Finalize();
}

///=============================================================================
///						初期化
void MRFramework::Initialize() {
	///--------------------------------------------------------------
	///						 ウィンドウ生成
	win_ = std::make_unique<WinApp>();
	//ウィンドウの生成
	win_->CreateGameWindow(L"Ver1.0_SpaceDodgeBall");

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
	//入力の初期化
	Input::GetInstance()->Initialize(win_->GetWindowClass().hInstance, win_->GetWindowHandle());

	///--------------------------------------------------------------
	///						 スプライトクラス
	//========================================
	// スプライト共通部
	spriteSetup_ = std::make_unique<SpriteSetup>();
	//スプライト共通部の初期化
	spriteSetup_->Initialize(dxCore_.get());
	//========================================
	// テクスチャマネージャ
	TextureManager::GetInstance()->Initialize(dxCore_.get(), "resources/texture/", srvSetup_.get());

	///--------------------------------------------------------------
	///						 Object3D共通部
	//========================================
	// モデルマネージャの初期化
	ModelManager::GetInstance()->Initialize(dxCore_.get());
	//========================================
	// 3Dオブジェクト共通部
	object3dSetup_ = std::make_unique<Object3dSetup>();
	//3Dオブジェクト共通部の初期化
	object3dSetup_->Initialize(dxCore_.get());

	///--------------------------------------------------------------
	///						 パーティクル共通部
	//========================================
	// パーティクルセットアップ
	particleSetup_ = std::make_unique<ParticleSetup>();
	//パーティクルセットアップの初期化
	particleSetup_->Initialize(dxCore_.get(), srvSetup_.get());

	///--------------------------------------------------------------
	///						 オーディオの初期化
	MAudioG::GetInstance()->Initialize("resources/sound/");

	///--------------------------------------------------------------
	/// デフォルトカメラの初期化
	//========================================
	// カメラ
	CameraManager::GetInstance()->AddCamera("DefaultCamera");///
	CameraManager::GetInstance()->SetCurrentCamera("DefaultCamera");///
	// Object3Dのカメラ設定
	object3dSetup_->SetDefaultCamera(CameraManager::GetInstance()->GetCurrentCamera());///
	// カメラの設定
	particleSetup_->SetDefaultCamera(CameraManager::GetInstance()->GetCurrentCamera());
	
	///--------------------------------------------------------------
	///						 シーンマネージャ
	sceneManager_ = std::make_unique<SceneManager>();
	//シーンマネージャの初期化
	sceneManager_->Initialize(spriteSetup_.get(), object3dSetup_.get(), particleSetup_.get());
}

///=============================================================================
///						更新
void MRFramework::Update() {
	//========================================
	// インプットの更新
	Input::GetInstance()->Update();
	//========================================
	// シーンマネージャの更新
	sceneManager_->Update();
}

///=============================================================================
///						終了処理
void MRFramework::Finalize() {
	//========================================
	// ImGuiの終了処理
	imguiSetup_->Finalize();
	//========================================
	// audioの終了処理
	MAudioG::GetInstance()->Finalize();
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
///						フレームワーク共通前処理
void MRFramework::FrameworkPreDraw() {
	//========================================
	// ループ前処理
	dxCore_->PreDraw();
	srvSetup_->PreDraw();
}

///=============================================================================
///						フレームワーク共通後処理
void MRFramework::FrameworkPostDraw() {
	//========================================
	// ImGui描画
	imguiSetup_->Draw();
	//========================================
	//ループ後処理
	dxCore_->PostDraw();
}

///=============================================================================
///						ImGuiの更新前処理
void MRFramework::ImGuiPreDraw() {
	// imguiの初期化
	imguiSetup_->Begin();
#ifdef _DEBUG
	// imguiの描画
	sceneManager_->ImGuiDraw();
	// InPutのImGui描画
	Input::GetInstance()->ImGuiDraw();
#endif // DEBUG
}

///=============================================================================
///						ImGuiの更新後処理
void MRFramework::ImGuiPostDraw() {
	// imguiの終了処理
	imguiSetup_->End();
}

///=============================================================================
///						Object2D共通描画設定
void MRFramework::Object2DCommonDraw() {
	spriteSetup_->CommonDrawSetup();
	// 2D描画
	sceneManager_->Object2DDraw();
}

///=============================================================================
///						particle共通描画設定
void MRFramework::ParticleCommonDraw() {
	particleSetup_->CommonDrawSetup();
	// パーティクル描画
	sceneManager_->ParticleDraw();
}

///=============================================================================
///						Object3D共通描画設定
void MRFramework::Object3DCommonDraw() {
	object3dSetup_->CommonDrawSetup();
	// 3D描画
	sceneManager_->Object3DDraw();
}
