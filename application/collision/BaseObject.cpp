#include "BaseObject.h"

///=============================================================================
///						初期化
void BaseObject::Initialize(Vector3& position, float radius) {
	// コライダーの生成
	collider_ = std::make_unique<Collider>();

	// キャラの位置とコライダーの位置を同期
	collider_->SetPosition(position); // カプセルの位置を設定
	collider_->SetRadius(radius); // カプセルの半径を設定
}

///=============================================================================
///						更新
void BaseObject::Update(Vector3 &position) {
	collider_->SetPosition(position);
}
