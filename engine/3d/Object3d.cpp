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


	//トランスフォーメーションマトリックスバッファの作成
	CreateTransformationMatrixBuffer();
	//並行光源の作成
	CreateDirectionalLight();

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
// NOTE:見た目を持たないオブジェクトが存在する
void Object3d::Draw() {
	//========================================
	// バッファが初期化されていない場合はエラーを出す
	if(!transfomationMatrixBuffer_) {
		throw std::runtime_error("One or more buffers are not initialized.");
	}
	// コマンドリスト取得
	auto commandList = object3dSetup_->GetDXManager()->GetCommandList();
	//トランスフォーメーションマトリックスバッファの設定
	commandList->SetGraphicsRootConstantBufferView(1, transfomationMatrixBuffer_->GetGPUVirtualAddress());
	//並行光源の設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightBuffer_->GetGPUVirtualAddress());
	// 描画コール
	if(model_) {
		model_->Draw();
	}
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
