/*********************************************************************
 * \file   main.cpp
 * \brief  メインファイル
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
 ///--------------------------------------------------------------
 ///						 標準ライブラリ
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
//---------------------------------------
// コムポインタ
#include <wrl.h>
#include <memory> // std::unique_ptr
//---------------------------------------
// ファイル読み込み用
#include <fstream>
#include <sstream>
///--------------------------------------------------------------
///						 自作クラス
//---------------------------------------
// base
#include "WinApp.h"
#include "ImguiSetup.h"
#include "DirectXCore.h"
#include "SrvSetup.h"
//---------------------------------------
// io
#include "Input.h"
//---------------------------------------
// 2D
#include "spriteSetup.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ParticleSetup.h"
#include "Particle.h"
#include "ParticleEmitter.h"
//---------------------------------------
// 3D
#include "Object3dSetup.h"
#include "Object3d.h"
#include "ModelSetup.h"
#include "Model.h"
#include "ModelManager.h"
//---------------------------------------
// camera
#include "Camera.h"
///--------------------------------------------------------------
///						 自作構造体
//========================================
// 基底構造体
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Matrix3x3.h"
//========================================
// 描画データ構造体
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "MaterialData.h"
#include "ModelData.h"
///--------------------------------------------------------------
///						 自作数学
#include "MathFunc4x4.h"			// 4x4行列演算
#include "AffineTransformations.h"	// 3次元アフィン演算
#include "RenderingMatrices.h"		// レンダリングパイプライン
#include "WstringUtility.h"			// Wstring変換
#include "Logger.h"					// ログ出力
//ランダム用
#include <random>
#include <stdlib.h>

///=============================================================================
///						Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	///--------------------------------------------------------------
	///						 ウィンドウ生成
	std::unique_ptr<WinApp> win = std::make_unique<WinApp>();
	win->CreateGameWindow(L"GE3");

	///--------------------------------------------------------------
	///						 ダイレクトX生成
	//インスタンスの取得
	std::unique_ptr<DirectXCore> dxCore = std::make_unique<DirectXCore>();
	//ダイレクトXの初期化
	dxCore->InitializeDirectX(win.get());

	///--------------------------------------------------------------
	///						 ImGuiのセットアップ
	std::unique_ptr<ImguiSetup> imguiSetup = std::make_unique<ImguiSetup>();
	//ImGuiの初期化
	imguiSetup->Initialize(win.get(), dxCore.get(), Style::CYBER);
	///--------------------------------------------------------------
	///						 SrvSetupクラス
	std::unique_ptr<SrvSetup> srvSetup = std::make_unique<SrvSetup>();
	//SrvSetupの初期化
	srvSetup->Initialize(dxCore.get());

	///--------------------------------------------------------------
	///						 入力クラス
	//ユニークポインタ
	std::unique_ptr<Input> input = std::make_unique<Input>();
	//入力の初期化
	input->Initialize(win->GetWindowClass().hInstance, win->GetWindowHandle());

	///--------------------------------------------------------------
	///						 2D系クラス
	//========================================
	// スプライト共通部
	std::unique_ptr<SpriteSetup> spriteSetup = std::make_unique<SpriteSetup>();
	//スプライト共通部の初期化
	spriteSetup->Initialize(dxCore.get());

	//========================================
	// テクスチャマネージャ
	TextureManager::GetInstance()->Initialize(dxCore.get(), "resources/texture/", srvSetup.get());
	TextureManager::GetInstance()->LoadTexture("uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("monsterBall.png");

	//========================================
	// スプライトクラス
	//ユニークポインタ
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	//スプライトの初期化
	sprite->Initialize(spriteSetup.get(), "uvChecker.png");
	//サイズ
	sprite->SetSize({ 256.0f,256.0f });

	///--------------------------------------------------------------
	///						 3D系クラス
	//========================================
	// モデルマネージャの初期化
	ModelManager::GetInstance()->Initialize(dxCore.get());
	//モデルの読み込み
	ModelManager::GetInstance()->LoadMedel("axisPlus.obj");
	ModelManager::GetInstance()->LoadMedel("Particle.obj");

	//========================================
	// 3Dオブジェクト共通部
	std::unique_ptr<Object3dSetup> object3dSetup = std::make_unique<Object3dSetup>();
	//3Dオブジェクト共通部の初期化
	object3dSetup->Initialize(dxCore.get());

	//========================================
	// カメラ
	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	//カメラの設定
	object3dSetup->SetDefaultCamera(camera.get());

	//========================================
	// 3Dオブジェクトクラス
	std::unique_ptr<Object3d> object3d = std::make_unique<Object3d>();
	//3Dオブジェクトの初期化
	object3d->Initialize(object3dSetup.get());
	object3d->SetModel("axisPlus.obj");

	///--------------------------------------------------------------
	///						 パーティクル系
	// ========================================
	// パーティクルセットアップ
	std::unique_ptr<ParticleSetup> particleSetup = std::make_unique<ParticleSetup>();
	//パーティクルセットアップの初期化
	particleSetup->Initialize(dxCore.get(), srvSetup.get());
	//カメラの設定
	particleSetup->SetDefaultCamera(camera.get());

	//========================================
	// パーティクルクラス
	std::unique_ptr<Particle> particle = std::make_unique<Particle>();
	//パーティクルの初期化
	particle->Initialize(particleSetup.get());
	//パーティクルグループの作成
	particle->CreateParticleGroup("Particle", "monsterBall.png");

	//========================================
	// パーティクルエミッター
	std::unique_ptr<ParticleEmitter> particleEmitter =
		std::make_unique<ParticleEmitter>(particle.get(), "Particle", Transform{ {0.2f,0.2f,0.2f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} }, 100, 0.1f, true);


	///--------------------------------------------------------------
	///						 メインループ用変数
	//========================================
	// 3Dオブジェクト用
	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//========================================
	// 2Dオブジェクト用
	//TransformSprite
	Transform transformSprite{ {256.0f,256.0f,256.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//マテリアル
	Vector4 materialSprite = sprite->GetColor();

	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};




	///=============================================================================
	///						 メインループ
	MSG msg{};
	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//========================================
			// インプットの更新
			input->Update();

			///--------------------------------------------------------------
			///						 開発用UIの処理
			////========================================
			////ImGuiのフレーム開始
			//ImGui_ImplDX12_NewFrame();
			//ImGui_ImplWin32_NewFrame();
			//ImGui::NewFrame();

			////========================================
			//// 3Dオブジェクト設定
			//ImGui::Begin("3D Object");
			//// カメラのトランスフォーム設定
			//ImGui::Text("Camera Transform");
			//ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x, 0.01f);
			//ImGui::DragFloat3("cameraRotate", &cameraTransform.rotate.x, 0.01f);
			//// 空白とセパレータ
			//ImGui::Dummy(ImVec2(0.0f, 10.0f));
			//ImGui::Separator();

			//// オブジェクトのトランスフォーム設定
			//ImGui::Text("3D Object Transform");
			//ImGui::DragFloat3("3D Rotate", &transform.rotate.x, 0.01f);
			//ImGui::DragFloat3("3D Translate", &transform.translate.x, 0.01f);
			//// 空白とセパレータ
			//ImGui::Dummy(ImVec2(0.0f, 10.0f));
			//ImGui::Separator();

			//ImGui::DragFloat2("UVTransform", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			//ImGui::DragFloat2("UVTScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			//ImGui::SliderAngle("UVTransform", &uvTransformSprite.translate.z);

			//ImGui::End();

			////========================================
			//// 2Dオブジェクト設定
			//ImGui::Begin("2D Object");
			//// カラーピッカーを表示
			//ImGui::Text("2D Material Settings");
			//ImGui::ColorPicker4("Color", reinterpret_cast<float*>( &materialSprite.x ), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);
			////スプライトに適用
			//sprite->SetColor(materialSprite);
			//// 空白とセパレータ
			//ImGui::Dummy(ImVec2(0.0f, 10.0f));
			//ImGui::Separator();
			////サイズ設定
			//ImGui::Text("2D Object Size");
			//ImGui::DragFloat2("2D Scale", &transformSprite.scale.x, 0.01f);
			////回転設定
			//ImGui::Text("2D Object rotate");
			//ImGui::DragFloat("2D Rotate", &transformSprite.rotate.x, 0.01f);
			//// 座標設定
			//ImGui::Text("2D Object Translate");
			//ImGui::DragFloat3("2D Translate", &transformSprite.translate.x, 1.0f);
			//// 空白とセパレータ
			//ImGui::Dummy(ImVec2(0.0f, 10.0f));
			//ImGui::Separator();
			//// 空白とセパレータ
			//ImGui::Dummy(ImVec2(0.0f, 10.0f));
			//ImGui::Separator();
			//ImGui::End();

			////========================================
			//// カメラの設定
			//ImGui::Begin("camera");
			//// カメラのトランスフォーム設定
			//ImGui::Text("Camera Transform");
			//ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x, 0.01f);
			//ImGui::DragFloat3("cameraRotate", &cameraTransform.rotate.x, 0.01f);
			//camera->SetTransform(cameraTransform);
			//camera->SetRotate(cameraTransform.rotate);
			//// 空白とセパレータ
			//ImGui::Dummy(ImVec2(0.0f, 10.0f));
			//ImGui::Separator();

			//ImGui::End();

			///--------------------------------------------------------------
			///						更新処理
			//========================================
			// カメラの更新
			//camera->SetTransform(Transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-100.0f} });
			camera->Update();

			//========================================
			// 2D更新
			//大きさのセット
			sprite->SetSize(Vector2{ transformSprite.scale.x,transformSprite.scale.y });
			//回転のセット
			sprite->SetRotation(transformSprite.rotate.x);
			//座標のセット
			sprite->SetPosition(Vector2{ transformSprite.translate.x, transformSprite.translate.y });
			//単体
			sprite->Update();

			//========================================
			// 3D更新 
			//大きさのセット
			object3d->SetScale(Vector3{ transform.scale.x,transform.scale.y,transform.scale.z });
			//回転のセット
			object3d->SetRotation(Vector3{ transform.rotate.x,transform.rotate.y,transform.rotate.z });
			//座標のセット
			object3d->SetPosition(Vector3{ transform.translate.x,transform.translate.y,transform.translate.z });
			//更新
			object3d->Update();

			//========================================
			// パーティクル系
			//パーティクルの更新
			particle->Update();
			//パーティクルエミッターの更新
			particleEmitter->Update();

			//========================================
			// ImGuiの更新
			imguiSetup->Begin();
			//↓この間に書け!!!
			//DEMOウィンドウの表示
			ImGui::ShowDemoWindow();

			ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_Once);
			ImGui::Begin("2D Object");
			// カラーピッカーを表示
			ImGui::Text("2D Material Settings");
			ImGui::ColorPicker4("Color", reinterpret_cast<float*>( &materialSprite.x ), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);
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
			imguiSetup->End();
			//========================================

			///--------------------------------------------------------------
			///						 描画(コマンドを積む)
			//ImGuiの内部コマンド生成
			//ImGui::Render();
			///---------------------------------------
			/// ループ前処理
			dxCore->PreDraw();
			srvSetup->PreDraw();

			//========================================
			//パーティクル


			//========================================
			//3Dオブジェクト共通描画設定
			object3dSetup->CommonDrawSetup();
			// 3D描画
			object3d->Draw();

			//========================================
			// 2Dオブジェクト共通描画設定
			spriteSetup->CommonDrawSetup();

			//Spriteクラス
			sprite->Draw();

			//========================================
			//パーティクル共通描画設定
			particleSetup->CommonDrawSetup();
			//パーティクル描画
			particle->Draw();


			//========================================
			// ImGui描画
			//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCore->GetCommandList().Get());
			imguiSetup->Draw();

			///---------------------------------------
			///ループ後処理
			dxCore->PostDraw();
		}
	}


	///=============================================================================
	///						開放処理
	//========================================
	// ImGuiの終了処理
	//ImGui_ImplDX12_Shutdown();  // ImGuiのDirectX12サポート終了
	//ImGui_ImplWin32_Shutdown();  // ImGuiのWin32サポート終了
	//ImGui::DestroyContext();  // ImGuiコンテキストの破棄
	imguiSetup->Finalize();

	//========================================
	// テクスチャマネージャの終了処理
	TextureManager::GetInstance()->Finalize();	//終了処理

	//========================================
	// モデルマネージャの終了処理
	ModelManager::GetInstance()->Finalize();	//終了処理

	//========================================
	// パーティクルマネージャの終了処理

	//========================================
	// ダイレクトX
	dxCore->ReleaseDirectX();  // DirectXの解放処理

	//========================================
	// ウィンドウの終了
	win->CloseWindow();

	return 0;
}
