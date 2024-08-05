#include <Windows.h>
/// ===自作クラス=== ///
#include "base/WinApp.h"
#include "base/DirectXManager.h"
#include "input/Input.h"
#include "base/SpriteManager.h"
#include "base/Sprite.h"
#include "MAG/MAudio.h"
/// ===Win関連=== ///
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
/// ===コムポインタ=== ///
#include <wrl.h>
/// ===ファイル読み込み用=== ///
#include <fstream>
#include <sstream>
/// ===自作関数=== //
//構造体
#include "math/structure/Vector3.h"
#include "math/structure/Vector4.h"
#include "math/structure/Matrix4x4.h"
#include "math/structure/Transform.h"
#include "math/structure/Matrix3x3.h"
//3x3行列演算
#include "math/Calc3x3.h"
using namespace Calc3x3;
//4x4行列演算
#include "math/Calc4x4.h"
using namespace Calc4x4;
//3次元アフィン演算
#include"math/AffineCalc.h"
using namespace AffineCalc;
//レンダリングパイプライン
#include"math/RendPipeLine.h"
using namespace RendPipeLine;
//Wstring変換
#include "base/utils/WstringUtility.h"
//ログ出力
#include "base/utils/Logger.h"
using namespace Calc3x3;


///==============================================///
//構造体
///==============================================///
const struct Vector2 {
	float x;
	float y;
};

/// ===頂点データ=== ///　
const struct VertexData {
	Vector4 position;
	Vector2 texCoord;
	Vector3 normal;
};

/// ===マテリアル=== ///
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

/// ===トランスレートマトリックス=== ///
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

/// ===並行光源=== ///
//NOTE:光源の色、向き、光度表す。向きは必ず正規化しておくこと。
struct DirectionalLight {
	Vector4 color;		//ライトの色
	Vector3 direction;	//ライトの向き
	float intensity;	//光度

};

/// ===マテリアルデータ=== ///
struct MaterialData {
	std::string textureFilePath;
};

/// ===ModelData=== ///
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

/// ===サウンドセット=== ///
struct SoundSet {
	int dataHandle;
	uint32_t voiceHandle;
	float volume = 0.0f;
};


///=====================================================/// 
///mtlファイルの読み込み
///=====================================================///
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


///
/// OBJファイルを読む関数
///
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	/// ===1.中で必要となる変数の宣言=== ///
	ModelData modelData;            //構築するModelData
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals;   //法線
	std::vector<Vector2> texcoords; //テクスチャ座標
	std::string line;               //ファイルから読んだ1行を格納するもの

	/// ===2.ファイルを開く=== ///
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	/// ===3.実際にファイルを読み、ModelDataを構築していく=== ///
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

	/// ===4.ModelDataを返す=== ///
	return modelData;
}

///=====================================================/// 
///リソースリークチェッカー
///=====================================================///
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


///==============================================///
///Windowsアプリでのエントリーポイント(main関数)
///==============================================///
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	///----------------------------------------///
	//ウィンドウ生成
	///----------------------------------------///
	WinApp* win = new WinApp;
	win->CreateGameWindow(L"CG2");

	///----------------------------------------/// 
	///リークチェック
	///----------------------------------------///
	D3DResourceLeakCheker leakCheck;

	///----------------------------------------///
	///ダイレクトX生成
	///----------------------------------------///
	//インスタンスの取得
	DirectXManager* dxManager = new DirectXManager;
	//ダイレクトXの初期化
	dxManager->InitializeDirectX(win);

	///----------------------------------------/// 
	///入力クラス
	///----------------------------------------///
	//ポインタ
	Input* input = nullptr;
	//入力の初期化
	input = new Input();
	input->Initialize(win->GetWindowClass().hInstance, win->GetWindowHandle());

	///-------------------------------------------///
	///オーディオクラス
	///-------------------------------------------///
	//自作オーディオ
	MAudio* mAudio = nullptr;
	mAudio = MAudio::GetInstance();
	mAudio->ListAudioDevices();
	mAudio->Initialize("resources/music/", L"");


	///----------------------------------------///
	///スプライト系クラス
	///----------------------------------------///
	/// ===スプライト基盤システム=== ///
	SpriteManager* spriteManager = nullptr;
	//スプライト共通部の初期化
	spriteManager = new SpriteManager;
	spriteManager->Initialize(dxManager);

	///-------------------------------------------/// 
	///ModelResourceを生成
	///-------------------------------------------///
	/// ===StanfordBunnyの読み込み=== ///
	ModelData modelDataBunny = LoadObjFile("resources", "bunny.obj");
	/// ===頂点リソースを作る=== ///
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResourceBunny = dxManager->CreateBufferResource(sizeof(VertexData) * modelDataBunny.vertices.size());
	/// ===頂点バッファビューを作成する=== ///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewBunny{};
	vertexBufferViewBunny.BufferLocation = vertexResourceBunny->GetGPUVirtualAddress();				//リソースの先頭アドレスから使う
	vertexBufferViewBunny.SizeInBytes = UINT(sizeof(VertexData) * modelDataBunny.vertices.size());	//使用するリソースのサイズは頂点サイズ
	vertexBufferViewBunny.StrideInBytes = sizeof(VertexData);									//1頂点あたりのサイズ
	/// ===頂点リソースにデータを書き込む=== ///
	VertexData* vertexDataBunny = nullptr;
	vertexResourceBunny->Map(0, nullptr, reinterpret_cast<void**>( &vertexDataBunny ));
	std::memcpy(vertexDataBunny, modelDataBunny.vertices.data(), sizeof(VertexData) * modelDataBunny.vertices.size());


	/// ===Teapotの読み込み=== ///
	ModelData modelDataTeapot = LoadObjFile("resources", "teapot.obj");
	/// ===頂点リソースを作る=== ///
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResourceTeapot = dxManager->CreateBufferResource(sizeof(VertexData) * modelDataTeapot.vertices.size());
	/// ===頂点バッファビューを作成する=== ///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewTeapot{};
	vertexBufferViewTeapot.BufferLocation = vertexResourceTeapot->GetGPUVirtualAddress();				//リソースの先頭アドレスから使う
	vertexBufferViewTeapot.SizeInBytes = UINT(sizeof(VertexData) * modelDataTeapot.vertices.size());	//使用するリソースのサイズは頂点サイズ
	vertexBufferViewTeapot.StrideInBytes = sizeof(VertexData);									//1頂点あたりのサイズ
	/// ===頂点リソースにデータを書き込む=== ///
	VertexData* vertexDataTeapot = nullptr;
	vertexResourceTeapot->Map(0, nullptr, reinterpret_cast<void**>( &vertexDataTeapot ));
	std::memcpy(vertexDataTeapot, modelDataTeapot.vertices.data(), sizeof(VertexData) * modelDataTeapot.vertices.size());



	///----------------------------------------///
	//VettexResourceSpriteを生成(スプライト用)
	///----------------------------------------///
	/// ===頂点リソースの作成=== ///
	//NOTE:一般的にこれらのデータはオブジェクト事に必要である
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResouceSprite = dxManager->CreateBufferResource(sizeof(VertexData) * 6);

	/// ===VettexBufferViewSpriteを作成する=== ///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResouceSprite->GetGPUVirtualAddress();
	//使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	/// ===リソースにデータを書き込む=== ///
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

	///----------------------------------------///
	//VettexResourceSpriteを生成(スプライト用)
	///----------------------------------------///
	/// ===頂点リソースの作成=== ///
	Microsoft::WRL::ComPtr <ID3D12Resource> indexResourceSprite = dxManager->CreateBufferResource(sizeof(uint32_t) * 6);

	/// ===VettexBufferViewSpriteを作成する=== ///
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	//リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースサイズは頂点3つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//1頂点あたりのサイズ
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	/// ===リソースにデータを書き込む=== ///
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>( &indexDataSprite ));
	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 4; indexDataSprite[5] = 2;

	///----------------------------------------///
	//並行光源用のリソース
	///----------------------------------------///
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource = dxManager->CreateBufferResource(sizeof(DirectionalLight));
	//並行光源リソースデータ
	DirectionalLight* directionalLightData = nullptr;
	//並行光源書き込み用データ
	DirectionalLight directionalLight{};
	//書き込むためのアドレス取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>( &directionalLightData ));

	/// ===リソースデータへの書き込み(初期設定)=== ///
	directionalLight.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight.direction = { 0.0f,-1.0f,0.0f };
	directionalLight.intensity = 1.0f;
	*directionalLightData = directionalLight;

	///----------------------------------------///
	//マテリアル用のリソース(3D)
	///----------------------------------------///
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


	///----------------------------------------///
	//マテリアル用のリソース(2D)
	///----------------------------------------///
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResourceSprite = dxManager->CreateBufferResource(sizeof(Material));
	//マテリアルデータ
	Material* materialDataSprite = nullptr;
	//マテリアルデータ書き込み用変数
	Material materialSprite = { {1.0f, 1.0f, 1.0f, 1.0f},false };
	//書き込むためのアドレス取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>( &materialDataSprite ));
	//今回は赤を書き込む
	*materialDataSprite = materialSprite;
	//UVトランスフォーム
	materialDataSprite->uvTransform = Identity4x4();

	///----------------------------------------///
	//WVP用のリソース Matrix4x4 1つ分のサイズを用意
	///----------------------------------------///
	//wvp用のリソースを作る(Bunny)
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResourceBunny = dxManager->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixDataBunny = nullptr;
	//書き込み用変数
	TransformationMatrix transformationMatrixBunny;
	//書き込むためのアドレスを取得
	transformationMatrixResourceBunny->Map(0, nullptr, reinterpret_cast<void**>( &transformationMatrixDataBunny ));
	//書き込み
	transformationMatrixBunny.WVP = Identity4x4();
	//単位行列を書き込む
	*transformationMatrixDataBunny = transformationMatrixBunny;

	//wvp用のリソースを作る(Teapot)
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResourceTeapot = dxManager->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixDataTeapot = nullptr;
	//書き込み用変数
	TransformationMatrix transformationMatrixTeapot;
	//書き込むためのアドレスを取得
	transformationMatrixResourceTeapot->Map(0, nullptr, reinterpret_cast<void**>( &transformationMatrixDataTeapot ));
	//書き込み
	transformationMatrixTeapot.WVP = Identity4x4();
	//単位行列を書き込む
	*transformationMatrixDataTeapot = transformationMatrixTeapot;

	///----------------------------------------///
	//Sprite用リソース Matrial4x4 1つ
	///----------------------------------------///
	//wvp用のリソースを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResourceSprite = dxManager->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//書き込み用変数
	TransformationMatrix transformationMatrixSprite;
	//書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>( &transformationMatrixDataSprite ));
	//書き込み
	transformationMatrixSprite.WVP = Identity4x4();
	//単位行列を書き込む
	*transformationMatrixDataSprite = transformationMatrixSprite;


	///----------------------------------------///
	//textureResourceの読み込み
	///----------------------------------------///
	/// ===一枚目=== ///
	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxManager->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource = dxManager->CreateTextureResource(metadata);
	dxManager->UploadTextureData(textureResource, mipImages);

	/// ===二枚目(Bunny)=== ///
	//Textureを読んで転送する
	DirectX::ScratchImage mipImagesBunny = dxManager->LoadTexture(modelDataBunny.material.textureFilePath);
	const DirectX::TexMetadata& metadataBunny = mipImagesBunny.GetMetadata();
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResourceBunny = dxManager->CreateTextureResource(metadataBunny);
	dxManager->UploadTextureData(textureResourceBunny, mipImagesBunny);

	/// ===三枚目(teapot)=== ///
	DirectX::ScratchImage mipImagesteapot = dxManager->LoadTexture(modelDataTeapot.material.textureFilePath);
	const DirectX::TexMetadata& metadatateapot = mipImagesteapot.GetMetadata();
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResourceteapot = dxManager->CreateTextureResource(metadatateapot);
	dxManager->UploadTextureData(textureResourceteapot, mipImagesteapot);

	///----------------------------------------///
	//実際にShaderResourceViewを作る
	///----------------------------------------///
	/// ===一枚目=== ///
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

	/// ===二枚目(Bunny)=== ///
	//metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescBunny{};
	srvDescBunny.Format = metadataBunny.format;
	srvDescBunny.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescBunny.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDescBunny.Texture2D.MipLevels = UINT(metadataBunny.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHadleCPUBunny = dxManager->GetSRVCPUDescriptorHandle(2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHadleGPUBunny = dxManager->GetSRVGPUDescriptorHandle(2);
	//SRVの生成
	dxManager->GetDevice()->CreateShaderResourceView(textureResourceBunny.Get(), &srvDescBunny, textureSrvHadleCPUBunny);

	/// ===三枚目(teapot)=== ///
	//metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescteapot{};
	srvDescteapot.Format = metadatateapot.format;
	srvDescteapot.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescteapot.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDescteapot.Texture2D.MipLevels = UINT(metadatateapot.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHadleCPUteapot = dxManager->GetSRVCPUDescriptorHandle(3);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHadleGPUteapot = dxManager->GetSRVGPUDescriptorHandle(3);
	//SRVの生成
	dxManager->GetDevice()->CreateShaderResourceView(textureResourceteapot.Get(), &srvDescteapot, textureSrvHadleCPUteapot);



	///----------------------------------------///
	//ViewportとScissor
	///----------------------------------------///
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


	///----------------------------------------///
	//コマンドリスト
	///----------------------------------------///
	//コマンドリストの設定
	dxManager->CloseCommandList();
	//コマンドキック
	dxManager->ExecuteCommandList();
	//フェンス生成
	dxManager->FenceGeneration();
	//Imguiの初期化
	dxManager->ImGuiInitialize();


	///----------------------------------------///
	//メインループ用変数
	///----------------------------------------///
	/// ===Transform変数を作る=== ///
	//Bunny
	Transform transformBunny{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	bool isDrawbunny = true;//描画するか
	//teapot
	Transform transformTeapot{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	bool isDrawTeapot = false;//描画するか

	//カメラの作成
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	//CPUで動かす用のTransform
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//スプライト用
	bool isDrawSprite = false;//描画するか

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

	/// ===audio=== ///
	SoundSet testSound;
	testSound.dataHandle = mAudio->LoadWave("Cola_drums.wav");
	testSound.volume = 0.4f;

	///========================================///
	//メインループ
	///========================================///
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			/// ===Inputクラス=== ///
			input->Update();


			// ImGuiのフレーム開始
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			/// ===開発用UIの処理=== ///
			ImGui::Begin("3D Object");
			/// --カラーピッカーを表示-- ///
			ImGui::Text("3D Material Settings");
			ImGui::ColorPicker4("Color", &material.color.x, 0.01f);
			*materialData = material;
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();

			/// --ライティング-- ///
			ImGui::Text("DirectionalLighting");
			ImGui::DragFloat3("Light direction", &directionalLight.direction.x, 0.01f);
			directionalLight.direction = Normalize(directionalLight.direction);
			*directionalLightData = directionalLight;

			/// --カメラのトランスフォーム設定-- //
			ImGui::Text("Camera Transform");
			ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x, 0.01f);
			ImGui::DragFloat3("cameraRotate", &cameraTransform.rotate.x, 0.01f);
			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();

			/// --3Dオブジェクトのトランスフォーム設定-- //
			ImGui::Text("3D Object Transform");
			ImGui::Checkbox("is Draw Bunny", &isDrawbunny);
			ImGui::DragFloat3("3D Bunny Rotate", &transformBunny.rotate.x, 0.01f);
			ImGui::DragFloat3("3D Bunny Translate", &transformBunny.translate.x, 0.01f);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Checkbox("is Draw Teapot", &isDrawTeapot);
			ImGui::DragFloat3("3D Teapot Rotate", &transformTeapot.rotate.x, 0.01f);
			ImGui::DragFloat3("3D Teapot Translate", &transformTeapot.translate.x, 0.01f);
			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();
			ImGui::End();

			ImGui::Begin("2D Object");
			/// --2Dオブジェクトのトランスフォーム設定-- ///
			ImGui::Text("2D Object Transform");
			ImGui::Checkbox("is Draw Sprite", &isDrawSprite);
			ImGui::DragFloat3("2D Translate", &transformSprite.translate.x, 1.0f);
			// テクスチャの切り替え設定
			ImGui::Text("Texture Settings");
			ImGui::DragFloat2("UVTransform", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVTScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::End();

			/// ====ゲーム処理==== ///
			///サウンド再生
			if (!mAudio->IsPlaying(testSound.voiceHandle)) {
				testSound.voiceHandle = mAudio->PlayWave(testSound.dataHandle);
				mAudio->SetVolume(testSound.voiceHandle, testSound.volume);
			}

			///カメラ処理
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);

			///3Dオブジェクト処理(Bunny)
			if (isDrawbunny) {
				transformBunny.rotate.y += 0.001f;
			}
			Matrix4x4 worldMatrixBunny = MakeAffineMatrix(transformBunny.scale, transformBunny.rotate, transformBunny.translate);
			transformationMatrixBunny.World = worldMatrixBunny;
			Matrix4x4 projectionMatrixBunny = MakePerspectiveFovMatrix(0.45f, float(win->GetWindowWidth()) / float(win->GetWindowHeight()), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixBunny = Multiply4x4(worldMatrixBunny, Multiply4x4(viewMatrix, projectionMatrixBunny));
			transformationMatrixBunny.WVP = worldViewProjectionMatrixBunny;
			*transformationMatrixDataBunny = transformationMatrixBunny;

			///3Dオブジェクト処理(Teapot)
			if (isDrawTeapot) {
				transformTeapot.rotate.y += 0.001f;
			}
			Matrix4x4 worldMatrixTeapot = MakeAffineMatrix(transformTeapot.scale, transformTeapot.rotate, transformTeapot.translate);
			transformationMatrixTeapot.World = worldMatrixTeapot;
			Matrix4x4 projectionMatrixTeapot = MakePerspectiveFovMatrix(0.45f, float(win->GetWindowWidth()) / float(win->GetWindowHeight()), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixTeapot = Multiply4x4(worldMatrixTeapot, Multiply4x4(viewMatrix, projectionMatrixTeapot));
			transformationMatrixTeapot.WVP = worldViewProjectionMatrixTeapot;
			*transformationMatrixDataTeapot = transformationMatrixTeapot;


			//2Dオブジェクト処理
			//sprite用のWorldViewProjectionMatrixを作る
			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			transformationMatrixSprite.World = worldMatrixSprite;
			Matrix4x4 viewMatrxSprite = Identity4x4();
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(win->GetWindowWidth()), float(win->GetWindowHeight()), 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = Multiply4x4(worldMatrixSprite, Multiply4x4(viewMatrxSprite, projectionMatrixSprite));
			transformationMatrixSprite.WVP = worldViewProjectionMatrixSprite;
			*transformationMatrixDataSprite = transformationMatrixSprite;

			Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			uvTransformMatrix = Multiply4x4(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			uvTransformMatrix = Multiply4x4(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			materialDataSprite->uvTransform = uvTransformMatrix;

			materialData->uvTransform = Identity4x4();

			/// ====コマンドを積む==== ///
			///ImGuiの内部コマンド生成
			ImGui::Render();
			///ループ前処理
			dxManager->PreDraw();
			///共通描画設定
			spriteManager->CommonDrawSetup();

			///3D描画///
			dxManager->GetCommandList()->RSSetViewports(1, &viewport);
			dxManager->GetCommandList()->RSSetScissorRects(1, &scissorRect);
			// DirectionalLight用のCBV設定 (b1)
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

			//モデル(Bunny)
			dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewBunny);
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceBunny->GetGPUVirtualAddress());
			dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2,textureSrvHadleGPUBunny);//テクスチャ
			if (isDrawbunny) {
				dxManager->GetCommandList()->DrawInstanced(UINT(modelDataBunny.vertices.size()), 1, 0, 0);
			}
			//モデル(teapot)
			dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewTeapot);
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceTeapot->GetGPUVirtualAddress());
			dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHadleGPUteapot);//テクスチャ
			if (isDrawTeapot) {
				dxManager->GetCommandList()->DrawInstanced(UINT(modelDataTeapot.vertices.size()), 1, 0, 0);
			}


			//テクスチャの切り替え	
			//dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, usaMonsterBall ? textureSrvHadleGPUBunny : textureSrvHadleGPU);
			// 描画コマンド(頂点データ)
			//dxManager->GetCommandList()->DrawInstanced(kNumVertices, 1, 0, 0);
			// 描画コマンド(インデックスデータ)
			//dxManager->GetCommandList()->DrawIndexedInstanced(kNumIndices, 1, 0, 0, 0);
			// 描画コマンド(モデルデータ)


			///2D描画///
			//NOTE:Material用のCBuffer(色)とSRV(Texture)は3Dの三角形と同じものを使用。無駄を省け。
			//NOTE:同じものを使用したな？気をつけろ、別々の描画をしたいときは個別のオブジェクトとして宣言し直せ。
			//Spriteの描画
			dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
			//Index使用スプライト
			dxManager->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
			//マテリアル
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			//transformationMatrixVBufferの場所を設定
			dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			//使用するテクスチャ
			dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHadleGPU);
			//描画！(ドロ‐コール)
			if (isDrawSprite) {
				dxManager->GetCommandList()->DrawInstanced(6, 1, 0, 0);
			}
			//描画!
			//dxManager->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

			// ImGui描画
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxManager->GetCommandList().Get());

			///ループ後処理
			dxManager->PostDraw();
		}
	}


	///----------------------------------------///
	//開放処理
	///----------------------------------------///
	/// ===入力クラス=== ///
	delete input;

	/// ===オーディオクラス=== ///
	mAudio->Finalize();

	/// ===スプライト=== ///
	delete spriteManager;
	//delete sprite;

	/// ===ImGuiの終了処理=== ///
	//srvDescriptorHeap->Release();  // シェーダーリソースビュー用ディスクリプタヒープの解放
	ImGui_ImplDX12_Shutdown();  // ImGuiのDirectX12サポート終了
	ImGui_ImplWin32_Shutdown();  // ImGuiのWin32サポート終了
	ImGui::DestroyContext();  // ImGuiコンテキストの破棄

	/// ===ダイレクトX=== ///
	dxManager->ReleaseDirectX();  // DirectXの解放処理
	delete dxManager;

	/// ===ウィンドウの終了=== ///
	win->CloseWindow();

	return 0;
}
