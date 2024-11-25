/*********************************************************************
 * \file   Object3d.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#include "Object3d.h"
#include "Object3dSetup.h"
 //---------------------------------------
 // ファイル読み込み関数
#include <fstream>
#include <sstream>
//---------------------------------------
// 数学関数　
#include <cmath>
#include "Calc4x4.h"
#include "AffineCalc.h"
#include "TextureManager.h"
#include "RendPipeLine.h"


///=============================================================================
///						初期化
void Object3d::Initialize(Object3dSetup* object3dSetup) {
	//========================================
	// 引数からSetupを受け取る
	this->object3dSetup_ = object3dSetup;

	//========================================
	// モデルデータの読み込み
	LoadObjFile("resources", "axis.obj");

	//========================================
	// 頂点バッファの作成
	CreateVertexBuffer();
	//インデックスバッファの作成
	//CreateIndexBuffer();
	//マテリアルバッファの作成
	CreateMaterialBuffer();
	//トランスフォーメーションマトリックスバッファの作成
	CreateTransformationMatrixBuffer();
	//並行光源の作成
	CreateDirectionalLight();

	//========================================
	// テクスチャの読み込み	
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	//読み込んだテクスチャのインデックスを取得
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);

	//========================================
	// ワールド行列の初期化
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };
}

///=============================================================================
///						更新
void Object3d::Update() {
    // TransformからWorld行列を作成
    Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    // cameraTransformからcameraMatrixを作成
    Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
    // cameraTransformからviewMatrixを作成
    Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);
    //---------------------------------------
    // 正射影行列の作成
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(object3dSetup_->GetDXManager()->GetWinApp().GetWindowWidth()) / float(object3dSetup_->GetDXManager()->GetWinApp().GetWindowHeight()), 0.1f, 100.0f);
    //---------------------------------------
    // ワールド・ビュー・プロジェクション行列を計算
    Matrix4x4 worldViewProjectionMatrix = Multiply4x4(worldMatrix, Multiply4x4(viewMatrix, projectionMatrix));
    transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;
}

///=============================================================================
///						描画
void Object3d::Draw() {

	if(!vertexBuffer_ /*|| !indexBuffer_*/ || !materialBuffer_ || !transfomationMatrixBuffer_) {
		throw std::runtime_error("One or more buffers are not initialized.");
	}
	// コマンドリスト取得
	auto commandList = object3dSetup_->GetDXManager()->GetCommandList();

	//VertexBufferViewの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//マテリアルバッファの設定
	commandList->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGPUVirtualAddress());
	//トランスフォーメーションマトリックスバッファの設定
	commandList->SetGraphicsRootConstantBufferView(1, transfomationMatrixBuffer_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの設定
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	//並行光源の設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightBuffer_->GetGPUVirtualAddress());
	//描画(DrawCall)
	commandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}




///=============================================================================
///						ローカルメンバ関数
///--------------------------------------------------------------
///						 ファイル読み込み関数
///NOTE:ディレクトリパスとファイルネームの設定を忘れずに
MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
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
void Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
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
	modelData_ = modelData;
}

///--------------------------------------------------------------
///						 頂点データの作成
void Object3d::CreateVertexBuffer() {
	//========================================
	// 頂点リソースを作る
	vertexBuffer_ = object3dSetup_->GetDXManager()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	//========================================
	// 頂点バッファビューを作成する
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();				//リソースの先頭アドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());	//使用するリソースのサイズは頂点サイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);									//1頂点あたりのサイズ
	//========================================
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &vertexData ));
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

///--------------------------------------------------------------
///						 マテリアルデータの作成
void Object3d::CreateMaterialBuffer() {
	materialBuffer_ = object3dSetup_->GetDXManager()->CreateBufferResource(sizeof(Material));
	//マテリアルデータ
	materialData_ = nullptr;
	//マテリアルデータ書き込み用変数
	Material material = { {1.0f, 1.0f, 1.0f, 1.0f},true };
	//書き込むためのアドレス取得
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &materialData_ ));
	//今回は赤を書き込む
	*materialData_ = material;
	materialData_->uvTransform = Identity4x4();
}

///--------------------------------------------------------------
///						 座標変換行列
void Object3d::CreateTransformationMatrixBuffer() {
	//wvp用のリソースを作る
	transfomationMatrixBuffer_ = object3dSetup_->GetDXManager()->CreateBufferResource(sizeof(TransformationMatrix));
	//書き込み用変数
	TransformationMatrix transformationMatrix;
	//書き込むためのアドレスを取得
	transfomationMatrixBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &transformationMatrixData_ ));
	//書き込み
	transformationMatrix.WVP = Identity4x4();
	//単位行列を書き込む
	*transformationMatrixData_ = transformationMatrix;
}

///--------------------------------------------------------------
///						 並行光源の作成
void Object3d::CreateDirectionalLight() {
	directionalLightBuffer_ = object3dSetup_->GetDXManager()->CreateBufferResource(sizeof(DirectionalLight));
	//並行光源書き込み用データ
	DirectionalLight directionalLight{};
	//書き込むためのアドレス取得
	directionalLightBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &directionalLightData_ ));
	//書き込み
	directionalLight.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight.direction = { 0.0f,-1.0f,0.0f };
	directionalLight.intensity = 1.0f;
	*directionalLightData_ = directionalLight;
}
