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
///						 Win関係
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
///						関数プロトタイプ
///--------------------------------------------------------------
///						 ファイル読み込み関数
///NOTE:ディレクトリパスとファイルネームの設定を忘れずに
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);

	while(std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if(identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

///--------------------------------------------------------------
///						 OBJファイル読み込み関数
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	//========================================
	// 1.中で必要となる変数の宣言
	ModelData modelData;            //構築するModelData
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals;   //法線
	std::vector<Vector2> texcoords; //テクスチャ座標
	std::string line;               //ファイルから読んだ1行を格納するもの

	//========================================
	// 2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	//========================================
	// 3.実際にファイルを読み、ModelDataを構築していく
	while(std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; //先頭の識別子を読む

		//---------------------------------------
		// identifierに応じた処理
		if(identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f; //NOTE:同次座標を送っているためw=1
			position.x *= -1.0f; //反転
			positions.push_back(position);

		} else if(identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y; // y軸を反転
			texcoords.push_back(texcoord);

		} else if(identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f; //反転
			normals.push_back(normal);

		} else if(identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for(int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for(int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の値を取得して、頂点を構築する
				//NOTE:1始まりなので添字として利用するときは-1を忘れに
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position, texcoord, normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		} else if(identifier == "mtllib") {
			//MaterialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同１改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	//========================================
	// 4.ModelDataを返す
	return modelData;
}

///--------------------------------------------------------------
///						 リソースリークチェッカー
struct D3DResourceLeakCheker {
	~D3DResourceLeakCheker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			//開放を忘れてエラーが出た場合、205行目をコメントアウト
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			//debug->Release();
		}
	}
};



///=============================================================================
///						Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	///--------------------------------------------------------------
	///						 ウィンドウ生成
	std::unique_ptr<WinApp> win = std::make_unique<WinApp>();
	win->CreateGameWindow(L"GE3");

	///--------------------------------------------------------------
	///						 リークチェック
	D3DResourceLeakCheker leakCheck;

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
	// 3Dオブジェクト共通部
	std::unique_ptr<Object3dSetup> object3dSetup = std::make_unique<Object3dSetup>();
	//3Dオブジェクト共通部の初期化
	object3dSetup->Initialize(dxCore.get());

	//========================================
	// 3Dオブジェクトクラス
	std::unique_ptr<Object3d> object3d = std::make_unique<Object3d>();
	//3Dオブジェクトの初期化
	object3d->Initialize(object3dSetup.get()/*, "resources/uvChecker.png"*/);





	///--------------------------------------------------------------
	///						 ModelResourceを生成
	//========================================
	// モデルデータの読み込み
	ModelData modelData = LoadObjFile("resources", "axis.obj");
	//========================================
	// 頂点リソースを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource = dxCore->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	//========================================
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();				//リソースの先頭アドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());	//使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);									//1頂点あたりのサイズ
	//========================================
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>( &vertexData ));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	///--------------------------------------------------------------
	///						 並行光源用のリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource = dxCore->CreateBufferResource(sizeof(DirectionalLight));
	//並行光源リソースデータ
	DirectionalLight* directionalLightData = nullptr;
	//並行光源書き込み用データ
	DirectionalLight directionalLight{};
	//書き込むためのアドレス取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>( &directionalLightData ));

	///--------------------------------------------------------------
	///						 リソースデータへの書き込み(初期設定)
	directionalLight.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight.direction = { 0.0f,-1.0f,0.0f };
	directionalLight.intensity = 1.0f;
	*directionalLightData = directionalLight;


	///--------------------------------------------------------------
	///						 マテリアル用のリソース(3D)
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource = dxCore->CreateBufferResource(sizeof(Material));
	//マテリアルデータ
	Material* materialData = nullptr;
	//マテリアルデータ書き込み用変数
	Material material = { {1.0f, 1.0f, 1.0f, 1.0f},true };
	//書き込むためのアドレス取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>( &materialData ));
	//今回は赤を書き込む
	*materialData = material;
	materialData->uvTransform = Identity4x4();

	///--------------------------------------------------------------
	///						 WVP用のリソース Matrix4x4 1つ分のサイズを用意
	//wvp用のリソースを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource = dxCore->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixData = nullptr;
	//書き込み用変数
	TransformationMatrix transformationMatrix;
	//書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>( &transformationMatrixData ));
	//書き込み
	transformationMatrix.WVP = Identity4x4();
	//単位行列を書き込む
	*transformationMatrixData = transformationMatrix;




	///--------------------------------------------------------------
	///						 textureResourceの読み込み
	//========================================
	// 一枚目
	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxCore->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//テクスチャリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource = dxCore->CreateTextureResource(metadata);
	//中間リソース
	//NOTE:中間リソースはGPU側に設置したリソースの経由地点になるらしい。
	Microsoft::WRL::ComPtr <ID3D12Resource> interMediateResource = dxCore->UploadTextureData(textureResource, mipImages);

	//========================================
	// 二枚目
	////Textureを読んで転送する
	////DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	//DirectX::ScratchImage mipImages2 = dxCore->LoadTexture(modelData.material.textureFilePath);
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	////テクスチャリソース
	//Microsoft::WRL::ComPtr <ID3D12Resource> textureResource2 = dxCore->CreateTextureResource(metadata2);
	////中間リソース
	//Microsoft::WRL::ComPtr <ID3D12Resource> interMediateResource2 = dxCore->UploadTextureData(textureResource2, mipImages2);


	///--------------------------------------------------------------
	///						 SRVの設定
	//========================================
	//	一枚目
	//metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHadleCPU = dxCore->GetSRVCPUDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHadleGPU = dxCore->GetSRVGPUDescriptorHandle(1);
	//SRVの生成
	dxCore->GetDevice().Get()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHadleCPU);

	//========================================
	// 二枚目
	//metaDataを元にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	//srvDesc2.Format = metadata.format;
	//srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	//srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);
	////SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHadleCPU2 = dxCore->GetSRVCPUDescriptorHandle(2);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHadleGPU2 = dxCore->GetSRVGPUDescriptorHandle(2);
	////SRVの生成
	//dxCore->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHadleCPU2);

	///--------------------------------------------------------------
	///						 ViewportとScissor
	//========================================
	// ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(win->GetWindowWidth());
	viewport.Height = static_cast<float>(win->GetWindowHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;
	//========================================
	// シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようになる
	scissorRect.left = 0;
	scissorRect.right = win->GetWindowWidth();
	scissorRect.top = 0;
	scissorRect.bottom = win->GetWindowHeight();


	///--------------------------------------------------------------
	///						 コマンドリスト
	//コマンドリストの設定
	dxCore->CloseCommandList();
	//コマンドキック
	dxCore->ExecuteCommandList();
	//フェンス生成
	dxCore->FenceGeneration();
	//Imguiの初期化
	dxCore->ImGuiInitialize();


	///--------------------------------------------------------------
	///						 メインループ用変数
	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//カメラの作成
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	//========================================
	// 2Dオブジェクト用
	//TransformSprite
	Transform transformSprite{ {256.0f,256.0f,256.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//マテリアル
	Vector4 materialSprite = sprite->GetColor();
	//複数枚回転
	float spritesRotate = 0.01f;

	//もう一枚のテクスチャの描画
	bool usaMonsterBall = true;

	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	Transform uvTransform{
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
			// カラーピッカーを表示
			ImGui::Text("3D Material Settings");
			ImGui::ColorPicker4("Color", reinterpret_cast<float*>( &material.color.x ), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);
			*materialData = material;
			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();

			ImGui::Text("DirectionalLighting");
			ImGui::DragFloat3("Light direction", &directionalLight.direction.x, 0.01f);
			directionalLight.direction = Normalize(directionalLight.direction);
			*directionalLightData = directionalLight;


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
            ImGui::ColorPicker4("Color", reinterpret_cast<float*>(&materialSprite.x), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);
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
			// テクスチャの切り替え設定
			ImGui::Text("Texture Settings");
			ImGui::Checkbox("Use Monster Ball", &usaMonsterBall);
			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();
			ImGui::End();

			///--------------------------------------------------------------
			///						更新処理
			//========================================
			// カメラ処理
			transform.rotate.y += 0.001f;
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			transformationMatrix.World = worldMatrix;
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);

			//========================================
			// 3Dオブジェクト処理
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(win->GetWindowWidth()) / float(win->GetWindowHeight()), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = Multiply4x4(worldMatrix, Multiply4x4(viewMatrix, projectionMatrix));
			transformationMatrix.WVP = worldViewProjectionMatrix;
			*transformationMatrixData = transformationMatrix;
			materialData->uvTransform = Identity4x4();

			//========================================
			// 2Dオブジェクト処理
			//大きさのセット
			sprite->SetSize(Vector2{ transformSprite.scale.x,transformSprite.scale.y });
			//回転のセット
			sprite->SetRotation(transformSprite.rotate.x);
			//座標のセット
			sprite->SetPosition(Vector2{ transformSprite.translate.x, transformSprite.translate.y });


			//========================================
			// 2D更新
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
			dxCore->GetCommandList()->RSSetViewports(1, &viewport);
			dxCore->GetCommandList()->RSSetScissorRects(1, &scissorRect);

			//dxCore->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
			//// インデックスバッファをバインド
			////dxCore->GetCommandList()->IASetIndexBuffer(&indexBufferView);
			//dxCore->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			//dxCore->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
			////テクスチャの切り替え
			//dxCore->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHadleGPU);
			////dxCore->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetSrvHandleGPU(1));
			//// DirectionalLight用のCBV設定 (b1)
			//dxCore->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
			//// 描画コマンド(頂点データ)
			////dxCore->GetCommandList()->DrawInstanced(kNumVertices, 1, 0, 0);
			//// 描画コマンド(インデックスデータ)
			////dxCore->GetCommandList()->DrawIndexedInstanced(kNumIndices, 1, 0, 0, 0);
			//// 描画コマンド(モデルデータ)
			//dxCore->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

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
	//srvDescriptorHeap->Release();  // シェーダーリソースビュー用ディスクリプタヒープの解放
	ImGui_ImplDX12_Shutdown();  // ImGuiのDirectX12サポート終了
	ImGui_ImplWin32_Shutdown();  // ImGuiのWin32サポート終了
	ImGui::DestroyContext();  // ImGuiコンテキストの破棄

	//========================================
	// テクスチャマネージャ
	TextureManager::GetInstance()->Finalize();	//終了処理

	//========================================
	// ダイレクトX
	dxCore->ReleaseDirectX();  // DirectXの解放処理
	//delete dxCore;

	//========================================
	// ウィンドウの終了
	win->CloseWindow();

	return 0;
}
