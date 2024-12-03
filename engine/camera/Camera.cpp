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
//	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(object3dSetup_->GetDXManager()->GetWinApp().GetWindowWidth()) / float(object3dSetup_->GetDXManager()->GetWinApp().GetWindowHeight()), 0.1f, 100.0f);
	:transform_({ {1.0f,1.0f,1.0f},{0.2f,0.0f,0.0f},{0.0f,6.0f,-30.0f} })
	, horizontalFieldOfView_(0.45f)
	, aspectRatio_(static_cast<float>( WinApp::kWindowWidth_ ) / static_cast<float>( WinApp::kWindowHeight_ ))
	, nearClipRange_(0.1f)
	, farClipRange_(100.0f)
	, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	, viewMatrix_(Inverse4x4(worldMatrix_))
	, projectionMatrix_(MakePerspectiveFovMatrix(horizontalFieldOfView_, aspectRatio_, nearClipRange_, farClipRange_))
	, viewProjectionMatrix_(Multiply4x4(viewMatrix_, projectionMatrix_)) {
}

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
	projectionMatrix_ = MakePerspectiveFovMatrix(
		horizontalFieldOfView_,
		aspectRatio_,
		nearClipRange_,
		farClipRange_);

	//========================================
	// ビュー・プロジェクション行列を計算
	viewProjectionMatrix_ = Multiply4x4(viewMatrix_, projectionMatrix_);
}

///=============================================================================
///						描画
void Camera::Draw() {
}
