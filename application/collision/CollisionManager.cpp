#include "CollisionManager.h"
#include "BaseObject.h"
#include "ImguiSetup.h"
#include <algorithm>
#include <cmath>

void CollisionManager::Initialize(Object3d *object3d) {
	object3d_ = object3d;
}

void CollisionManager::Update() {
}

void CollisionManager::Draw() {
	//if(isHitDraw_) {
	//	for(auto &baseObj : Objects_) {
	//		// 当たり判定の位置にオブジェクトを配置
	//		object3d_->SetPosition(baseObj->GetCollider()->GetPosition());
	//		object3d_->Update();
	//		object3d_->Draw();
	//	}
	//}
}

///=============================================================================
///						Imguiの描画
void CollisionManager::DrawImGui() {
	//あたってるオブジェクトの数
	ImGui::Begin("CollisionManager");
	ImGui::Text("Colliding Objects: %d", Objects_.size());
	//HitBoxの表示
	ImGui::Checkbox("HitBox", &isHitDraw_);
	ImGui::End();
}

///-------------------------------------------///
/// リセット
///-------------------------------------------///
void CollisionManager::Reset() {
	// リストを空っぽにする
	Objects_.clear();
}

///-------------------------------------------///
/// コライダーのリストに登録
///-------------------------------------------///
void CollisionManager::AddCollider(BaseObject *baseObj) {
	// コライダーをリストに追加
	Objects_.push_back(baseObj);
}

///-------------------------------------------///
/// コライダー2つの衝突判定と応答
///-------------------------------------------///
void CollisionManager::CheckColliderPair(BaseObject *baseObjA, BaseObject *baseObjB) {
	if(baseObjA->GetCollider()->Intersects(*baseObjB->GetCollider())) {
		// 衝突している
		if(baseObjA->GetCollidingObjects().find(baseObjB) == baseObjA->GetCollidingObjects().end()) {
			// 衝突開始
			baseObjA->OnCollisionEnter(baseObjB);
			baseObjA->GetCollidingObjects().insert(baseObjB);
			baseObjB->OnCollisionEnter(baseObjA);
			baseObjB->GetCollidingObjects().insert(baseObjA);
		} else {
			// 衝突継続
			baseObjA->OnCollisionStay(baseObjB);
			baseObjB->OnCollisionStay(baseObjA);
		}
	} else {
		// 衝突していない
		if(baseObjA->GetCollidingObjects().find(baseObjB) != baseObjA->GetCollidingObjects().end()) {
			// 衝突終了
			baseObjA->OnCollisionExit(baseObjB);
			baseObjA->GetCollidingObjects().erase(baseObjB);
			baseObjB->OnCollisionExit(baseObjA);
			baseObjB->GetCollidingObjects().erase(baseObjA);
		}
	}
}

///-------------------------------------------///
/// すべての当たり判定チェック
///-------------------------------------------///
void CollisionManager::CheckAllCollisions() {
	for(auto itrA = Objects_.begin(); itrA != Objects_.end(); ++itrA) {
		for(auto itrB = std::next(itrA); itrB != Objects_.end(); ++itrB) {
			CheckColliderPair(*itrA, *itrB);
		}
	}
}


