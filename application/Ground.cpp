/*********************************************************************
 * \file   Ground.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "Ground.h"
///=============================================================================
///						初期化
void Ground::Initialize(Object3d *object3d) {
	object3d_ = object3d;
	object3d_->SetTransform(transform);
	object3d_->Update();
}

///=============================================================================
///						更新
void Ground::Update() {
	object3d_->Update();
}

///=============================================================================
///						描画
void Ground::Draw() {
	object3d_->Draw();
}
