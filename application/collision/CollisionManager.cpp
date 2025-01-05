#include "CollisionManager.h"
#include "BaseObject.h"
#include <algorithm>
#include <cmath>

void CollisionManager::Initialize() {
	colliders_.clear();
}

void CollisionManager::Update() {
	// 衝突判定（全組み合わせをチェック）
	for(size_t i = 0; i < colliders_.size(); ++i) {
		for(size_t j = i + 1; j < colliders_.size(); ++j) {
			auto colliderA = colliders_[i];
			auto colliderB = colliders_[j];

			// 球体同士の衝突判定
			Vector3 posA = colliderA->GetPosition();
			Vector3 posB = colliderB->GetPosition();
			float radiusA = colliderA->GetRadius();
			float radiusB = colliderB->GetRadius();

			float dx = posA.x - posB.x;
			float dy = posA.y - posB.y;
			float dz = posA.z - posB.z;
			float distanceSq = dx * dx + dy * dy + dz * dz;
			float radiusSum = radiusA + radiusB;

			BaseObject *objA = colliderA->GetOwner();
			BaseObject *objB = colliderB->GetOwner();

			if(distanceSq <= radiusSum * radiusSum) {
				// 衝突している場合
				// オブジェクトAの処理
				if(objA->GetCollidingObjects().find(objB) == objA->GetCollidingObjects().end()) {
					objA->OnCollisionEnter(objB);
					objA->GetCollidingObjects().insert(objB);
				} else {
					objA->OnCollisionStay(objB);
				}
				// オブジェクトBの処理
				if(objB->GetCollidingObjects().find(objA) == objB->GetCollidingObjects().end()) {
					objB->OnCollisionEnter(objA);
					objB->GetCollidingObjects().insert(objA);
				} else {
					objB->OnCollisionStay(objA);
				}
			} else {
				// 衝突していない場合
				// オブジェクトAの処理
				if(objA->GetCollidingObjects().find(objB) != objA->GetCollidingObjects().end()) {
					objA->OnCollisionExit(objB);
					objA->GetCollidingObjects().erase(objB);
				}
				// オブジェクトBの処理
				if(objB->GetCollidingObjects().find(objA) != objB->GetCollidingObjects().end()) {
					objB->OnCollisionExit(objA);
					objB->GetCollidingObjects().erase(objA);
				}
			}
		}
	}
}

void CollisionManager::AddCollider(const std::shared_ptr<Collider> &collider) {
	colliders_.push_back(collider);
}

void CollisionManager::RemoveCollider(const std::shared_ptr<Collider> &collider) {
	colliders_.erase(std::remove(colliders_.begin(), colliders_.end(), collider), colliders_.end());
}

