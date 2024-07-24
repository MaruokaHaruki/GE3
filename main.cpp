#include <Windows.h>
/// ===自作クラス=== ///
#include "base/WinApp.h"
#include "base/DirectXManager.h"
#include "input/Input.h"
#include "base/SpriteManager.h"
#include "base/Sprite.h"
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
#include "selfMath/structure/Vector3.h"
#include "selfMath/structure/Vector4.h"
#include "selfMath/structure/Matrix4x4.h"
#include "selfMath/structure/Transform.h"
#include "selfMath/structure/Matrix3x3.h"
//3x3行列演算
#include "selfMath/3x3Calc.h"
//4x4行列演算
#include "selfMath/4x4Calc.h"
//3次元アフィン演算
#include"selfMath/3dAffineCalc.h"
//レンダリングパイプライン
#include"selfMath/RendPipeLine.h"
//Wstring変換
#include "base/utils/WstringConve.h"
//ログ出力
#include "base/utils/Log.h"


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
	DirectXManager* DXManager = new DirectXManager;
	//ダイレクトXの初期化
	DXManager->InitializeDirectX(win);

	///----------------------------------------/// 
	///入力クラス
	///----------------------------------------///
	//ポインタ
	Input* input = nullptr;
	//入力の初期化
	input = new Input();
	input->Initialize(win->GetWindowClass().hInstance, win->GetWindowHandle());

	///----------------------------------------///
	///スプライト系クラス
	///----------------------------------------///
	///// ===スプライト基盤システム=== ///
	//SpriteManager* spriteManager = nullptr;
	////スプライト共通部の初期化
	//spriteManager = new SpriteManager;
	//spriteManager->Initialize();

	///// ===スプライト=== ///
	//Sprite* sprite = nullptr;
	////スプライトの初期化
	//sprite = new Sprite;
	//sprite->Initialize();


	///----------------------------------------///
	//PSO
	///----------------------------------------///
	/// ===RootSignature作成=== ///
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	/// ===DescriptorRangeの設定=== ///
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // から始まる
	descriptorRange[0].NumDescriptors = 1; //
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//descriptorRange[1].BaseShaderRegister = 0; // CBVがb0から始まる
	//descriptorRange[1].NumDescriptors = 3; // b0, b1, b2
	//descriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	/// ===RootParameter作成=== ///
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0; // b0

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0; // b0

	/// ===DescropterTable=== ///
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pParameters = rootParameters;				//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);	//配列の長さ
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	/// ====DirectionalLight=== ///
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//CBV
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShader
	rootParameters[3].Descriptor.ShaderRegister = 1; // b1


	/// ===Samplerの設定=== ///
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0; // s0
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;				//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);	//配列の長さ
	descriptionRootSignature.pStaticSamplers = staticSamplers;				//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);	//配列の長さ

	/// ===シリアライズしてバイナリにする=== ///
	Microsoft::WRL::ComPtr <ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr <ID3DBlob> errorBlob = nullptr;
	DXManager->SetHr(D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob));
	if (FAILED(DXManager->GetHr())) {
		Log(reinterpret_cast<char*>( errorBlob->GetBufferPointer() ));
		assert(false);
	}

	/// ===バイナリを元に生成=== ///
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;
	DXManager->SetHr(DXManager->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	assert(SUCCEEDED(DXManager->GetHr()));

	/// ===InputLayoutの設定を行う=== ///
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	//頂点データ
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//画像座標データ
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//法線データ
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	/// ===BlendStateの設定を行う=== ///
	//BlandState
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	/// ===RasterrizerStateの設定を行う=== ///
	//RasterrizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面（時計回り）の表示をしない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	/// ===Shaderをcompileする=== ///
	//Microsoft::WRL::ComPtr <IDxcBlob> vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl",
	//	L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	Microsoft::WRL::ComPtr <IDxcBlob> vertexShaderBlob = DXManager->CompileShader(L"Object3D.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	//Microsoft::WRL::ComPtr <IDxcBlob> pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl",
	//	L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	Microsoft::WRL::ComPtr <IDxcBlob> pixelShaderBlob = DXManager->CompileShader(L"Object3D.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	/// ===PSOを生成する=== ///
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジのタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定(気にしない)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencillStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencillDesc{};
	//Depthの機能を有効化する
	depthStencillDesc.DepthEnable = true;
	//書き込み
	depthStencillDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencillDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//depthStencillの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencillDesc;
	//
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	DXManager->SetHr(DXManager->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState)));
	assert(SUCCEEDED(DXManager->GetHr()));

	///-------------------------------------------/// 
	///ModelResourceを生成
	///-------------------------------------------///
	/// ===モデルデータの読み込み=== ///
	ModelData modelData = LoadObjFile("resources", "axis.obj");
	/// ===頂点リソースを作る=== ///
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource = DXManager->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	/// ===頂点バッファビューを作成する=== ///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();				//リソースの先頭アドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());	//使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);									//1頂点あたりのサイズ
	/// ===頂点リソースにデータを書き込む=== ///
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>( &vertexData ));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());


	///----------------------------------------///
	//VettexResourceSpriteを生成(スプライト用)
	///----------------------------------------///
	/// ===頂点リソースの作成=== ///
	//NOTE:一般的にこれらのデータはオブジェクト事に必要である
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResouceSprite = DXManager->CreateBufferResource(sizeof(VertexData) * 6);

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
	Microsoft::WRL::ComPtr <ID3D12Resource> indexResourceSprite = DXManager->CreateBufferResource(sizeof(uint32_t) * 6);

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
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource = DXManager->CreateBufferResource(sizeof(DirectionalLight));
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
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource = DXManager->CreateBufferResource(sizeof(Material));
	//マテリアルデータ
	Material* materialData = nullptr;
	//マテリアルデータ書き込み用変数
	Material material = { {1.0f, 1.0f, 1.0f, 1.0f},true };
	//書き込むためのアドレス取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>( &materialData ));
	//今回は赤を書き込む
	*materialData = material;
	materialData->uvTransform = IdentityMatrix();



	///----------------------------------------///
	//マテリアル用のリソース(2D)
	///----------------------------------------///
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResourceSprite = DXManager->CreateBufferResource(sizeof(Material));
	//マテリアルデータ
	Material* materialDataSprite = nullptr;
	//マテリアルデータ書き込み用変数
	Material materialSprite = { {1.0f, 1.0f, 1.0f, 1.0f},false };
	//書き込むためのアドレス取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>( &materialDataSprite ));
	//今回は赤を書き込む
	*materialDataSprite = materialSprite;
	//UVトランスフォーム
	materialDataSprite->uvTransform = IdentityMatrix();

	///----------------------------------------///
	//WVP用のリソース Matrix4x4 1つ分のサイズを用意
	///----------------------------------------///
	//wvp用のリソースを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource = DXManager->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixData = nullptr;
	//書き込み用変数
	TransformationMatrix transformationMatrix;
	//書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>( &transformationMatrixData ));
	//書き込み
	transformationMatrix.WVP = IdentityMatrix();
	//単位行列を書き込む
	*transformationMatrixData = transformationMatrix;

	///----------------------------------------///
	//Sprite用リソース Matrial4x4 1つ
	///----------------------------------------///
	//wvp用のリソースを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResourceSprite = DXManager->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//書き込み用変数
	TransformationMatrix transformationMatrixSprite;
	//書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>( &transformationMatrixDataSprite ));
	//書き込み
	transformationMatrixSprite.WVP = IdentityMatrix();
	//単位行列を書き込む
	*transformationMatrixDataSprite = transformationMatrixSprite;


	///----------------------------------------///
	//textureResourceの読み込み
	///----------------------------------------///
	/// ===一枚目=== ///
	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = DXManager->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource = DXManager->CreateTextureResource(metadata);
	DXManager->UploadTextureData(textureResource, mipImages);

	/// ===二枚目=== ///
	//Textureを読んで転送する
	//DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	DirectX::ScratchImage mipImages2 = DXManager->LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource2 = DXManager->CreateTextureResource(metadata2);
	DXManager->UploadTextureData(textureResource2, mipImages2);


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
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHadleCPU = DXManager->GetSRVCPUDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHadleGPU = DXManager->GetSRVGPUDescriptorHandle(1);
	//SRVの生成
	DXManager->GetDevice().Get()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHadleCPU);

	/// ===二枚目=== ///
	//metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHadleCPU2 = DXManager->GetSRVCPUDescriptorHandle(2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHadleGPU2 = DXManager->GetSRVGPUDescriptorHandle(2);
	//SRVの生成
	DXManager->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHadleCPU2);




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
	//コマンドリストの決定
	///----------------------------------------///
	DXManager->CloseCommandList();


	///----------------------------------	------///
	//コマンドキック
	///----------------------------------------///
	DXManager->ExecuteCommandList();


	///----------------------------------------///
	//フェンス生成
	///----------------------------------------///
	DXManager->FenceGeneration();


	///----------------------------------------///
	//Imguiの初期化
	///----------------------------------------///
	DXManager->ImGuiInitialize();


	///----------------------------------------///
	//メインループ用変数
	///----------------------------------------///
	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//カメラの作成
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	//CPUで動かす用のTransform
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

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
			ImGui::Begin("Window");
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

			// オブジェクトのトランスフォーム設定
			ImGui::Text("2D Object Transform");
			ImGui::DragFloat3("2D Translate", &transformSprite.translate.x, 1.0f);
			// テクスチャの切り替え設定
			ImGui::Text("Texture Settings");
			ImGui::Checkbox("Use Monster Ball", &usaMonsterBall);

			// 空白とセパレータ
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();

			ImGui::DragFloat2("UVTransform", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVTScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVTransform", &uvTransformSprite.translate.z);

			ImGui::End();



			/// ====ゲーム処理==== ///
			//カメラ処理
			transform.rotate.y += 0.001f;
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			transformationMatrix.World = worldMatrix;
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
			//3Dオブジェクト処理
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(win->GetWindowWidth()) / float(win->GetWindowHeight()), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));
			transformationMatrix.WVP = worldViewProjectionMatrix;
			*transformationMatrixData = transformationMatrix;
			//2Dオブジェクト処理
			//sprite用のWorldViewProjectionMatrixを作る
			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			transformationMatrixSprite.World = worldMatrixSprite;
			Matrix4x4 viewMatrxSprite = IdentityMatrix();
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(win->GetWindowWidth()), float(win->GetWindowHeight()), 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = MultiplyMatrix(worldMatrixSprite, MultiplyMatrix(viewMatrxSprite, projectionMatrixSprite));
			transformationMatrixSprite.WVP = worldViewProjectionMatrixSprite;
			*transformationMatrixDataSprite = transformationMatrixSprite;

			Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			uvTransformMatrix = MultiplyMatrix(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			uvTransformMatrix = MultiplyMatrix(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			materialDataSprite->uvTransform = uvTransformMatrix;

			materialData->uvTransform = IdentityMatrix();

			/// ====ImGuiの内部コマンド生成==== ///
			ImGui::Render();

			//// バックバッファの決定
			//DXManager->SettleCommandList();
			//// バリア設定
			//DXManager->SetupTransitionBarrier();

			//// 描画ターゲットの設定とクリア
			//DXManager->RenderTargetPreference(dsvHandle);

			DXManager->PreDraw();


			// ImGuiの描画用DescriptorHeap設定
			//ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get()};
			//DXManager->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

			/// ====コマンドを積む==== ///
			///3D描画
			DXManager->GetCommandList()->RSSetViewports(1, &viewport);
			DXManager->GetCommandList()->RSSetScissorRects(1, &scissorRect);
			DXManager->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

			DXManager->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
			DXManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
			// インデックスバッファをバインド
			//DXManager->GetCommandList()->IASetIndexBuffer(&indexBufferView);
			DXManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			DXManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			DXManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
			//テクスチャの切り替え
			DXManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, usaMonsterBall ? textureSrvHadleGPU2 : textureSrvHadleGPU);
			// DirectionalLight用のCBV設定 (b1)
			DXManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
			// 描画コマンド(頂点データ)
			//DXManager->GetCommandList()->DrawInstanced(kNumVertices, 1, 0, 0);
			// 描画コマンド(インデックスデータ)
			//DXManager->GetCommandList()->DrawIndexedInstanced(kNumIndices, 1, 0, 0, 0);
			// 描画コマンド(モデルデータ)
			DXManager->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);


			///2D描画
			//NOTE:Material用のCBuffer(色)とSRV(Texture)は3Dの三角形と同じものを使用。無駄を省け。
			//NOTE:同じものを使用したな？気をつけろ、別々の描画をしたいときは個別のオブジェクトとして宣言し直せ。
			//Spriteの描画
			DXManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
			//Index使用スプライト
			DXManager->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
			//マテリアル
			DXManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			//transformationMatrixVBufferの場所を設定
			DXManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			//使用するテクスチャ
			DXManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHadleGPU);
			//描画！(ドロ‐コール)
			//DXManager->GetCommandList()->DrawInstanced(6, 1, 0, 0);
			//描画!
			//DXManager->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

			// ImGui描画
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DXManager->GetCommandList().Get());

			DXManager->PostDraw();
		}
	}


	///----------------------------------------///
	//開放処理
	///----------------------------------------///
	/// ===入力クラス=== ///
	delete input;

	/// ===スプライト=== ///
	//delete spriteManager;
	//delete sprite;

	/// ===ImGuiの終了処理=== ///
	//srvDescriptorHeap->Release();  // シェーダーリソースビュー用ディスクリプタヒープの解放
	ImGui_ImplDX12_Shutdown();  // ImGuiのDirectX12サポート終了
	ImGui_ImplWin32_Shutdown();  // ImGuiのWin32サポート終了
	ImGui::DestroyContext();  // ImGuiコンテキストの破棄

	/// ===ダイレクトX=== ///
	DXManager->ReleaseDirectX();  // DirectXの解放処理
	delete DXManager;

	/// ===ウィンドウの終了=== ///
	win->CloseWindow();

	return 0;
}
