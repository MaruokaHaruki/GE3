#include "Particle.h"
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
#include "ParticleSetup.h"

///=============================================================================
///						
void Particle::Initialize(ParticleSetup* particleSetup) {
	//========================================
	// 引数からSetupを受け取る
	this->particleSetup_ = particleSetup;

	//========================================
	//トランスフォーメーションマトリックスバッファの作成
	CreateTransformationMatrixBuffer();
	//並行光源の作成
	CreateDirectionalLight();

	//========================================
	// ワールド行列の初期化
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//========================================
	// カメラの取得
	camera_ = particleSetup_->GetDefaultCamera();
}

///=============================================================================
///						更新
void Particle::Update() {
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
}

///=============================================================================
///						描画
void Particle::Draw() {
	//========================================
	// バッファが初期化されていない場合はエラーを出す
	if(!transfomationMatrixBuffer_) {
		throw std::runtime_error("One or more buffers are not initialized.");
	}
	// コマンドリスト取得
	auto commandList = particleSetup_->GetDXManager()->GetCommandList();
	//トランスフォーメーションマトリックスバッファの設定
	commandList->SetGraphicsRootConstantBufferView(1, transfomationMatrixBuffer_->GetGPUVirtualAddress());
	//並行光源の設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightBuffer_->GetGPUVirtualAddress());
	// 描画コール
	if(model_) {
		model_->Draw();
	}
}

///=============================================================================
///						座標変換
void Particle::CreateTransformationMatrixBuffer() {
	//wvp用のリソースを作る
	transfomationMatrixBuffer_ = particleSetup_->GetDXManager()->CreateBufferResource(sizeof(TransformationMatrix));
	//書き込み用変数
	TransformationMatrix transformationMatrix = {};
	//書き込むためのアドレスを取得
	transfomationMatrixBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &transformationMatrixData_ ));
	//書き込み
	transformationMatrix.WVP = Identity4x4();
	//単位行列を書き込む
	*transformationMatrixData_ = transformationMatrix;
}

///=============================================================================
///						
void Particle::CreateDirectionalLight() {
	directionalLightBuffer_ = particleSetup_->GetDXManager()->CreateBufferResource(sizeof(DirectionalLight));
	//並行光源書き込み用データ
	DirectionalLight directionalLight{};
	//書き込むためのアドレス取得
	directionalLightBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &directionalLightData_ ));
	//書き込み
	directionalLight.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight.direction = { 0.0f,0.0f,0.0f };
	directionalLight.intensity = 1.0f;
	*directionalLightData_ = directionalLight;
}
