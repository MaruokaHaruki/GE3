/*********************************************************************
 * \file   Player.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "Player.h"
#include "Input.h"

///=============================================================================
///						初期化
void Player::Initialize(Object3d *object3d) {
	//========================================
	// Object3D
	object3d_ = object3d;
}

///=============================================================================
///						更新
void Player::Update() {
	//========================================
	// キーで移動処理WASD
	if(Input::GetInstance()->PushKey(DIK_W)) {
		transform.translate.z += 0.1f;
	}
	if(Input::GetInstance()->PushKey(DIK_S)) {
		transform.translate.z -= 0.1f;
	}
	if(Input::GetInstance()->PushKey(DIK_A)) {
		transform.translate.x -= 0.1f;
	}
	if(Input::GetInstance()->PushKey(DIK_D)) {
		transform.translate.x += 0.1f;
	}

	object3d_->SetTransform(transform);
	object3d_->Update();
}

///=============================================================================
///						描画
void Player::Draw() {
	object3d_->Draw();
}

///=============================================================================
///						
void Player::OnCollisionEnter(BaseObject *other) {
}

///=============================================================================
///						
void Player::OnCollisionStay(BaseObject *other) {
}

///=============================================================================
///						
void Player::OnCollisionExit(BaseObject *other) {
}
