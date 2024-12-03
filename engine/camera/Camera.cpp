/*********************************************************************
 * \file   Camera.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   December 2024
 * \note
 *********************************************************************/
#include "Camera.h"
#include "WinApp.h"
//---------------------------------------
// 自作数学関数
#include "MathFunc4x4.h"
#include "AffineTransformations.h"

///=============================================================================
///						デフォルトコンストラクタ
Camera::Camera() 
	:transform_({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} })
	, horizontalFieldOfView_(0.45f)
	, aspectRatio_(WinApp::kWindowWidth_ / WinApp::kWindowHeight_)
	, nearClipRange_(0.1f)
	, farClipRange_(100.0f)
	, worldMatrix_(Identity4x4())
	, viewMatrix_(Identity4x4())
	, projectionMatrix_(Identity4x4())
	, viewProjectionMatrix_(Identity4x4())
{}

///=============================================================================
///						初期化
void Camera::Initialize() {
}

///=============================================================================
///						更新
void Camera::Update() {
	// ---------------------------------------
	// cameraTransformからcameraMatrixを作成
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	// cameraTransformからviewMatrixを作成
	viewMatrix_ = Inverse4x4(worldMatrix_);

	//---------------------------------------
	// 正射影行列の作成
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(
		horizontalFieldOfView_,
		aspectRatio_,
		nearClipRange_,
		farClipRange_);

	//========================================
	// ビュー・プロジェクション行列を計算
	Matrix4x4 viewProjectionMatrix = Multiply4x4(viewMatrix_, projectionMatrix);


}

///=============================================================================
///						描画
void Camera::Draw() {
}
