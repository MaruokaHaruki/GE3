/*********************************************************************
 * \file   main.cpp
 * \brief  メインファイル
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#include <Windows.h>
 ///--------------------------------------------------------------
 ///						 標準ライブラリ
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
///--------------------------------------------------------------
///						 コムポインタ
#include <wrl.h>
#include <memory> // std::unique_ptr
///--------------------------------------------------------------
///						 ファイル読み込み用
#include <fstream>
#include <sstream>
///--------------------------------------------------------------
///						 自作クラス
#include "WinApp.h"
#include "DirectXCore.h"
#include "Input.h"
#include "spriteSetup.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Object3dSetup.h"
#include "Object3d.h"
#include "ModelSetup.h"
#include "Model.h"
#include "ModelManager.h"
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
#include "Calc4x4.h"			// 4x4行列演算
#include "AffineCalc.h"			// 3次元アフィン演算
#include "RendPipeLine.h"		// レンダリングパイプライン
#include "WstringUtility.h"		// Wstring変換
#include "Logger.h"				// ログ出力

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
	TextureManager::GetInstance()->Initialize(dxCore.get());
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resources/monsterBall.png");

	//========================================
	// スプライトクラス
	//ユニークポインタ
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	//スプライトの初期化
	sprite->Initialize(spriteSetup.get(), "resources/uvChecker.png");
	//サイズ
	sprite->SetSize({ 256.0f,256.0f });

	// 複数枚描画用
	std::vector<std::unique_ptr<Sprite>> sprites;
	for(uint32_t i = 0; i < 5; ++i) {
		for(uint32_t i = 0; i < 5; ++i) {
			std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
			if(i % 2 == 0) {
				sprite->Initialize(spriteSetup.get(), "resources/monsterBall.png");
				//サイズ
				sprite->SetSize({ 256.0f,256.0f });
			} else {
				sprite->Initialize(spriteSetup.get(), "resources/uvChecker.png");
				//サイズ
				sprite->SetSize({ 256.0f,256.0f });
			}
			sprites.push_back(std::move(sprite));
		}
		// ユニークポインタでスプライトを作成
		//NOTE:autoを使用せず明示的を心がけろ
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize(spriteSetup.get(), "resources/uvChecker.png");

		// std::move で vector に追加
		//NOTE:unique_ptr はコピーができないので、std::move を使ってオーナーシップを移動させる必要がある
		sprites.push_back(std::move(sprite));
	}


	//TODO:06_03にて実装
	///--------------------------------------------------------------
	///						 3D系クラス
	//========================================
	// モデルマネージャの初期化
	ModelManager::GetInstance()->Initialize(dxCore.get());
	//モデルの読み込み
	ModelManager::GetInstance()->LoadMedel("axis.obj");

	//========================================
	// Model
	//std::unique_ptr<Model> model = std::make_unique<Model>();
	//model->Initialize();

	//========================================
	// 3Dオブジェクト共通部
	std::unique_ptr<Object3dSetup> object3dSetup = std::make_unique<Object3dSetup>();
	//3Dオブジェクト共通部の初期化
	object3dSetup->Initialize(dxCore.get());

	//========================================
	// 3Dオブジェクトクラス
	std::unique_ptr<Object3d> object3d = std::make_unique<Object3d>();
	//3Dオブジェクトの初期化
	object3d->Initialize(object3dSetup.get());
	object3d->SetModel("axis.obj");



	///--------------------------------------------------------------
	///						 メインループ用変数
	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//カメラの作成
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	Transform uvTransform{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	//========================================
	// 2Dオブジェクト用
	//TransformSprite
	Transform transformSprite{ {256.0f,256.0f,256.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//マテリアル
	Vector4 materialSprite = sprite->GetColor();
	//複数枚回転
	float spritesRotate = 0.01f;

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
			//========================================
			//ImGuiのフレーム開始
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			//========================================
			// 3Dオブジェクト設定
			ImGui::Begin("3D Object");
			// カメラのトランスフォーム設定
			ImGui::Text("Camera Transform");
			ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x, 0.01f);
			ImGui::DragFloat3("cameraRotate", &cameraTransform.rotate.x, 0.01f);
			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();

			// オブジェクトのトランスフォーム設定
			ImGui::Text("3D Object Transform");
			ImGui::DragFloat3("3D Rotate", &transform.rotate.x, 0.01f);
			ImGui::DragFloat3("3D Translate", &transform.translate.x, 0.01f);
			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();

			ImGui::DragFloat2("UVTransform", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVTScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVTransform", &uvTransformSprite.translate.z);

			ImGui::End();

			//========================================
			// 2Dオブジェクト設定
			ImGui::Begin("2D Object");
			// カラーピッカーを表示
			ImGui::Text("2D Material Settings");
			ImGui::ColorPicker4("Color", reinterpret_cast<float*>( &materialSprite.x ), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);
			//スプライトに適用
			sprite->SetColor(materialSprite);
			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();
			//サイズ設定
			ImGui::Text("2D Object Size");
			ImGui::DragFloat2("2D Scale", &transformSprite.scale.x, 0.01f);
			//回転設定
			ImGui::Text("2D Object rotate");
			ImGui::DragFloat("2D Rotate", &transformSprite.rotate.x, 0.01f);
			// 座標設定
			ImGui::Text("2D Object Translate");
			ImGui::DragFloat3("2D Translate", &transformSprite.translate.x, 1.0f);
			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();
			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();
			ImGui::End();

			///--------------------------------------------------------------
			///						更新処理

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
			//複数枚処理
			//NOTE:中身の分だけfor文を回す
			float offset = 100.0f;//ずらす距離
			int index = 0;
			for(const auto& sprite : sprites) {
				// スプライトを更新 (operator-> でアクセス)
				sprite->Update();
				sprite->SetSize(Vector2{ 128.0f,128.0f });
				sprite->SetRotation(sprite->GetRotation() + spritesRotate);
				sprite->SetPosition(Vector2{ index * offset, 1.0f });
				index++;
			}

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


			///--------------------------------------------------------------
			///						 描画(コマンドを積む)
			//ImGuiの内部コマンド生成
			ImGui::Render();
			///---------------------------------------
			/// ループ前処理
			dxCore->PreDraw();
			//3Dオブジェクト共通描画設定
			object3dSetup->CommonDrawSetup();

			//========================================
			// 3D描画
			object3d->Draw();

			//========================================
			// 2Dオブジェクト共通描画設定
			spriteSetup->CommonDrawSetup();

			//Spriteクラス
			sprite->Draw();
			//複数枚描画
			for(const auto& sprite : sprites) {
				// スプライトを描画
				sprite->Draw();
			}

			//========================================
			// ImGui描画
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCore->GetCommandList().Get());

			///---------------------------------------
			///ループ後処理
			dxCore->PostDraw();
		}
	}


	///=============================================================================
	///						開放処理
	//========================================
	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();  // ImGuiのDirectX12サポート終了
	ImGui_ImplWin32_Shutdown();  // ImGuiのWin32サポート終了
	ImGui::DestroyContext();  // ImGuiコンテキストの破棄

	//========================================
	// テクスチャマネージャの終了処理
	TextureManager::GetInstance()->Finalize();	//終了処理

	//========================================
	// モデルマネージャの終了処理
	ModelManager::GetInstance()->Finalize();	//終了処理

	//========================================
	// ダイレクトX
	dxCore->ReleaseDirectX();  // DirectXの解放処理

	//========================================
	// ウィンドウの終了
	win->CloseWindow();

	return 0;
}
