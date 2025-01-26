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
#include "Camera.h"
 //---------------------------------------
 // ファイル読み込み関数
#include <fstream>
#include <sstream>
//---------------------------------------
// 数学関数　
#include <cmath>
#include "MathFunc4x4.h"
#include "AffineTransformations.h"
#include "TextureManager.h"


///=============================================================================
///						初期化
void Object3d::Initialize(Object3dSetup* object3dSetup) {
	//========================================
	// 引数からSetupを受け取る
	this->object3dSetup_ = object3dSetup;

	//========================================
	//トランスフォーメーションマトリックスバッファの作成
	CreateTransformationMatrixBuffer();
	//並行光源の作成
	CreateDirectionalLight();
	//カメラバッファの作成
	CreateCameraBuffer();

	//========================================
	// ワールド行列の初期化
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//========================================
	// カメラの取得
	camera_ = object3dSetup_->GetDefaultCamera();
}

///=============================================================================
///						更新
void Object3d::Update() {
	camera_ = object3dSetup_->GetDefaultCamera();
	cameraData_->worldPosition = camera_->GetTransform().translate;

	// TransformからWorld行列を作成
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix;

	//========================================
	// カメラがセットされている場合はビュー行列を作成
	if(camera_) {
		// カメラのビュー行列を取得
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		// ワールドビュープロジェクション行列を計算
		worldViewProjectionMatrix = Multiply4x4(worldMatrix, viewProjectionMatrix);
	} else {
		// カメラがセットされていない場合はワールド行列をそのまま使う
		// NOTE:カメラがセットされてなくても描画できるようにするため
		worldViewProjectionMatrix = worldMatrix;
	}

	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;
	transformationMatrixData_->WorldInvTranspose = Inverse4x4(worldMatrix);
}

///=============================================================================
///						描画
// NOTE:見た目を持たないオブジェクトが存在する
void Object3d::Draw() {
	if (!transfomationMatrixBuffer_) {
		throw std::runtime_error("One or more buffers are not initialized.");
	}

	// コマンドリスト取得
	auto commandList = object3dSetup_->GetDXManager()->GetCommandList();

	// トランスフォーメーションマトリックスバッファの設定
	commandList->SetGraphicsRootConstantBufferView(1, transfomationMatrixBuffer_->GetGPUVirtualAddress());

	// 並行光源の設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightBuffer_->GetGPUVirtualAddress());

	// カメラバッファの設定
	commandList->SetGraphicsRootConstantBufferView(4, cameraBuffer_->GetGPUVirtualAddress());

	// 描画コール
	if (model_) {
		model_->Draw();
	}
}


///=============================================================================
///						テクスチャの変更
void Object3d::ChangeTexture(const std::string &texturePath) {
	model_->ChangeTexture(texturePath);
}


///--------------------------------------------------------------
///						 座標変換行列
void Object3d::CreateTransformationMatrixBuffer() {
	// 定数バッファのサイズを 256 バイトの倍数に設定
	size_t bufferSize = (sizeof(TransformationMatrix) + 255) & ~255;
	transfomationMatrixBuffer_ = object3dSetup_->GetDXManager()->CreateBufferResource(bufferSize);
	// 書き込み用変数
	TransformationMatrix transformationMatrix = {};
	// 書き込むためのアドレスを取得
	transfomationMatrixBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	// 書き込み
	transformationMatrix.WVP = Identity4x4();
	// 単位行列を書き込む
	*transformationMatrixData_ = transformationMatrix;
}

///--------------------------------------------------------------
///						 並行光源の作成
void Object3d::CreateDirectionalLight() {
	size_t bufferSize = (sizeof(DirectionalLight) + 255) & ~255;
	directionalLightBuffer_ = object3dSetup_->GetDXManager()->CreateBufferResource(bufferSize);
	// 並行光源書き込み用データ
	DirectionalLight directionalLight{};
	// 書き込むためのアドレス取得
	directionalLightBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 書き込み
	directionalLight.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight.direction = { 0.0f,-1.0f,0.0f };
	directionalLight.intensity = 16.0f;
	*directionalLightData_ = directionalLight;
}

///--------------------------------------------------------------
///						 カメラバッファの作成
void Object3d::CreateCameraBuffer() {
	size_t bufferSize = (sizeof(CameraForGpu) + 255) & ~255;
	cameraBuffer_ = object3dSetup_->GetDXManager()->CreateBufferResource(bufferSize);
	cameraBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	// カメラの位置を書き込む
	CameraForGpu cameraForGpu = {};
	cameraForGpu.worldPosition = { 1.0f, 1.0f, 1.0f };
	*cameraData_ = cameraForGpu;
}
