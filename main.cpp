///===================================================================///
///								main.cpp
///===================================================================///

///----------------------------------------------------///
///					インクルードスペース
///----------------------------------------------------///
#include <Windows.h>
///====================Win関係====================///
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
///====================コムポインタ====================///
#include <wrl.h>
#include <memory> // std::unique_ptr
///====================ファイル読み込み用====================///
#include <fstream>
#include <sstream>
///====================自作クラス====================///
#include "base/WinApp.h"
#include "base/DirectXManager.h"
#include "input/Input.h"
#include "base/SpriteManager.h"
#include "base/Sprite.h"
#include "TextureManager.h"
///====================自作構造体====================///
///----------------基底構造体
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Matrix3x3.h"
///----------------描画データ構造体
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "MaterialData.h"
#include "ModelData.h"
///====================自作数学関数====================///
///----------------3x3行列演算
#include "Calc3x3.h"
///----------------4x4行列演算
#include "Calc4x4.h"
///----------------3次元アフィン演算
#include"AffineCalc.h"
///----------------レンダリングパイプライン
#include"RendPipeLine.h"
///----------------Wstring変換
#include "base/utils/WstringUtility.h"
///----------------ログ出力
#include "base/utils/Logger.h"


///----------------------------------------------------///
///							関数
///----------------------------------------------------///
///====================mtlファイルの読み込み====================///
///NOTE:ディレクトリパスとファイルネームの設定を忘れずに
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

///====================OBJファイルを読む関数====================///
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	///----------------1.中で必要となる変数の宣言----------------///
	ModelData modelData;            //構築するModelData
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals;   //法線
	std::vector<Vector2> texcoords; //テクスチャ座標
	std::string line;               //ファイルから読んだ1行を格納するもの

	///----------------2.ファイルを開く----------------///
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	///----------------3.実際にファイルを読み、ModelDataを構築していく----------------///
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; //先頭の識別子を読む

		///identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f; //NOTE:同次座標を送っているためw=1
			position.x *= -1.0f; //反転
			positions.push_back(position);

		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y; // y軸を反転
			texcoords.push_back(texcoord);

		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f; //反転
			normals.push_back(normal);

		} else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
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

		} else if (identifier == "mtllib") {
			//MaterialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同１改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	///----------------4.ModelDataを返す----------------///
	return modelData;
}

///====================リソースリークチェッカー====================///
struct D3DResourceLeakCheker {
	~D3DResourceLeakCheker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			//開放を忘れてエラーが出た場合、205行目をコメントアウト
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			//debug->Release();
		}
	}
};



///----------------------------------------------------///
/// Windowsアプリでのエントリーポイント(main関数)
///----------------------------------------------------///
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	///====================ウィンドウ生成====================///
	std::unique_ptr<WinApp> win = std::make_unique<WinApp>();
	win->CreateGameWindow(L"CG2");

	///====================リークチェック====================///
	D3DResourceLeakCheker leakCheck;

	///====================ダイレクトX生成====================///
	//インスタンスの取得
	std::unique_ptr<DirectXManager> dxManager = std::make_unique<DirectXManager>();
	//ダイレクトXの初期化
	dxManager->InitializeDirectX(win.get());

	///====================入力クラス====================///
	//ユニークポインタ
	std::unique_ptr<Input> input = std::make_unique<Input>();
	//入力の初期化
	input->Initialize(win->GetWindowClass().hInstance, win->GetWindowHandle());

	///====================テクスチャマネージャ====================///
	TextureManager::Getinstance()->initialize(dxManager.get());

	///====================スプライト系クラス====================///
	///----------------スプライト基盤システム----------------///
	//ユニークポインタ
	std::unique_ptr<SpriteManager> spriteManager = std::make_unique<SpriteManager>();
	//スプライト共通部の初期化
	spriteManager->Initialize(dxManager.get());

	///----------------スプライト----------------///
	//ユニークポインタ
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	//スプライトの初期化
	sprite->Initialize(spriteManager.get());
	// 複数枚描画用
	std::vector<std::unique_ptr<Sprite>> sprites;
	for (uint32_t i = 0; i < 5; ++i) {
		// ユニークポインタでスプライトを作成
		//NOTE:autoを使用せず明示的を心がけろ
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize(spriteManager.get());

		// std::move で vector に追加
		//NOTE:unique_ptr はコピーができないので、std::move を使ってオーナーシップを移動させる必要がある
		sprites.push_back(std::move(sprite));
	}


	///====================ModelResourceを生成====================///
	/// ===モデルデータの読み込み=== ///
	ModelData modelData = LoadObjFile("resources", "axis.obj");
	/// ===頂点リソースを作る=== ///
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource = dxManager->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	/// ===頂点バッファビューを作成する=== ///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();				//リソースの先頭アドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());	//使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);									//1頂点あたりのサイズ
	/// ===頂点リソースにデータを書き込む=== ///
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>( &vertexData ));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());


	///====================VettexResourceSpriteを生成(スプライト用)====================////'
	///----------------頂点リソースの作成----------------///
	//NOTE:一般的にこれらのデータはオブジェクト事に必要である
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResouceSprite = dxManager->CreateBufferResource(sizeof(VertexData) * 6);

	///----------------VettexBufferViewSpriteを作成する----------------///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResouceSprite->GetGPUVirtualAddress();
	//使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	///----------------リソースにデータを書き込む----------------///
	VertexData* vertexDataSprite = nullptr;
	//書き込むためのアドレス
	vertexResouceSprite->Map(0, nullptr,
		reinterpret_cast<void**>( &vertexDataSprite ));
	//左下
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[0].texCoord = { 0.0f,1.0f };
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };
	//上
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[1].texCoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[2].texCoord = { 1.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };
	///二番目の三角形
	//左下
	vertexDataSprite[3].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[3].texCoord = { 0.0f,0.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };
	//上
	vertexDataSprite[4].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[4].texCoord = { 1.0f,0.0f };
	vertexDataSprite[4].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexDataSprite[5].position = { 640.5f,360.0f,0.0f,1.0f };
	vertexDataSprite[5].texCoord = { 1.0f,1.0f };
	vertexDataSprite[5].normal = { 0.0f,0.0f,-1.0f };

	///====================並行光源用のリソース====================///
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource = dxManager->CreateBufferResource(sizeof(DirectionalLight));
	//並行光源リソースデータ
	DirectionalLight* directionalLightData = nullptr;
	//並行光源書き込み用データ
	DirectionalLight directionalLight{};
	//書き込むためのアドレス取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>( &directionalLightData ));

	///----------------リソースデータへの書き込み(初期設定)----------------///
	directionalLight.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight.direction = { 0.0f,-1.0f,0.0f };
	directionalLight.intensity = 1.0f;
	*directionalLightData = directionalLight;


	///====================マテリアル用のリソース(3D)====================///
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource = dxManager->CreateBufferResource(sizeof(Material));
	//マテリアルデータ
	Material* materialData = nullptr;
	//マテリアルデータ書き込み用変数
	Material material = { {1.0f, 1.0f, 1.0f, 1.0f},true };
	//書き込むためのアドレス取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>( &materialData ));
	//今回は赤を書き込む
	*materialData = material;
	materialData->uvTransform = Identity4x4();

	///====================WVP用のリソース Matrix4x4 1つ分のサイズを用意====================///
	//wvp用のリソースを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource = dxManager->CreateBufferResource(sizeof(TransformationMatrix));
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

	///====================textureResourceの読み込み====================///
	/// ===一枚目=== ///
	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxManager->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource = dxManager->CreateTextureResource(metadata);
	dxManager->UploadTextureData(textureResource, mipImages);

	/// ===二枚目=== ///
	//Textureを読んで転送する
	//DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	DirectX::ScratchImage mipImages2 = dxManager->LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource2 = dxManager->CreateTextureResource(metadata2);
	dxManager->UploadTextureData(textureResource2, mipImages2);


	///====================実際にShaderResourceViewを作る====================///
	///----------------一枚目----------------///
	//metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHadleCPU = dxManager->GetSRVCPUDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHadleGPU = dxManager->GetSRVGPUDescriptorHandle(1);
	//SRVの生成
	dxManager->GetDevice().Get()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHadleCPU);

	///----------------二枚目----------------///
	//metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHadleCPU2 = dxManager->GetSRVCPUDescriptorHandle(2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHadleGPU2 = dxManager->GetSRVGPUDescriptorHandle(2);
	//SRVの生成
	dxManager->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHadleCPU2);


	///====================ViewportとScissor====================///
	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = win->GetWindowWidth();
	viewport.Height = win->GetWindowHeight();
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようになる
	scissorRect.left = 0;
	scissorRect.right = win->GetWindowWidth();
	scissorRect.top = 0;
	scissorRect.bottom = win->GetWindowHeight();


	///====================コマンドリスト====================///
	//コマンドリストの設定
	dxManager->CloseCommandList();
	//コマンドキック
	dxManager->ExecuteCommandList();
	//フェンス生成
	dxManager->FenceGeneration();
	//Imguiの初期化
	dxManager->ImGuiInitialize();


	///====================メインループ用変数====================///
	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//カメラの作成
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	///----------------2Dオブジェクト用----------------///
	//TransformSprite
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
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

	///----------------------------------------------------///
	/// メインループ
	///----------------------------------------------------///
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			/// ===Inputクラス=== ///
			input->Update();

			///====================開発用UIの処理====================///
			///----------------ImGuiのフレーム開始----------------///
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			///----------------3Dオブジェクト設定----------------///
			ImGui::Begin("3D Object");
			// カラーピッカーを表示
			ImGui::Text("3D Material Settings");
			ImGui::ColorPicker4("Color", &material.color.x, 0.01f);
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

			///----------------2Dオブジェクト設定----------------///
			ImGui::Begin("2D Object");
			// カラーピッカーを表示
			ImGui::Text("2D Material Settings");
			ImGui::ColorPicker4("Color", &materialSprite.x, 0.01f);
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

			///====================ゲーム処理====================///
			///----------------カメラ処理----------------///
			transform.rotate.y += 0.001f;
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			transformationMatrix.World = worldMatrix;
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);

			///----------------3Dオブジェクト処理----------------///
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(win->GetWindowWidth()) / float(win->GetWindowHeight()), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = Multiply4x4(worldMatrix, Multiply4x4(viewMatrix, projectionMatrix));
			transformationMatrix.WVP = worldViewProjectionMatrix;
			*transformationMatrixData = transformationMatrix;

			///----------------2Dオブジェクト処理----------------///
			//大きさのセット
			sprite->SetSize(Vector2{ transformSprite.scale.x,transformSprite.scale.y });
			//回転のセット
			sprite->SetRotation(transformSprite.rotate.x);
			//座標のセット
			sprite->SetPosition(Vector2{ transformSprite.translate.x, transformSprite.translate.y });

			materialData->uvTransform = Identity4x4();


			///====================Spriteクラス====================///
			//単体
			sprite->Update();
			//複数枚処理
			//NOTE:中身の分だけfor文を回す
			float offset = 100.0f;//ずらす距離
			int index = 0;
			for (const auto& sprite : sprites) {
				// スプライトを更新 (operator-> でアクセス)
				sprite->Update();
				sprite->SetSize(Vector2{ 0.05f,0.1f });
				sprite->SetRotation(sprite->GetRotation() + spritesRotate);
				sprite->SetPosition(Vector2{ index * offset, 1.0f });
				index++;
			}

			///====================コマンドを積む====================///
			///ImGuiの内部コマンド生成
			ImGui::Render();
			///ループ前処理
			dxManager->PreDraw();
			///共通描画設定
			spriteManager->CommonDrawSetup();

			///----------------3D描画----------------///
			dxManager->GetCommandList()->RSSetViewports(1, &viewport);
			dxManager->GetCommandList()->RSSetScissorRects(1, &scissorRect);

			dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
			// インデックスバッファをバインド
			//dxManager->GetCommandList()->IASetIndexBuffer(&indexBufferView);
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
			//テクスチャの切り替え
			dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, usaMonsterBall ? textureSrvHadleGPU2 : textureSrvHadleGPU);
			// DirectionalLight用のCBV設定 (b1)
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
			// 描画コマンド(頂点データ)
			//dxManager->GetCommandList()->DrawInstanced(kNumVertices, 1, 0, 0);
			// 描画コマンド(インデックスデータ)
			//dxManager->GetCommandList()->DrawIndexedInstanced(kNumIndices, 1, 0, 0, 0);
			// 描画コマンド(モデルデータ)
			dxManager->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);


			///----------------2D描画----------------///
			///Spriteクラス
			sprite->Draw(textureSrvHadleGPU);
			//複数枚描画
			for (const auto& sprite : sprites) {
				// スプライトを描画
				sprite->Draw(textureSrvHadleGPU);
			}

			// ImGui描画
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxManager->GetCommandList().Get());

			///ループ後処理
			dxManager->PostDraw();
		}
	}


	///====================開放処理====================///

	///----------------ImGuiの終了処理----------------///
	//srvDescriptorHeap->Release();  // シェーダーリソースビュー用ディスクリプタヒープの解放
	ImGui_ImplDX12_Shutdown();  // ImGuiのDirectX12サポート終了
	ImGui_ImplWin32_Shutdown();  // ImGuiのWin32サポート終了
	ImGui::DestroyContext();  // ImGuiコンテキストの破棄

	///----------------テクスチャマネージャ----------------///
	TextureManager::Getinstance()->Finalize();	//終了処理

	/// ===ダイレクトX=== ///
	dxManager->ReleaseDirectX();  // DirectXの解放処理
	//delete dxManager;

	/// ===ウィンドウの終了=== ///
	win->CloseWindow();

	return 0;
}
