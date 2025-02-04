/*********************************************************************
 * \file   Enemy.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#define _USE_MATH_DEFINES
#include <cmath>
#include "Enemy.h"
#include "Player.h"
#include "PlayerWepon.h"
#include "MathFunc4x4.h"
#include "AffineTransformations.h"
#include "Vector3.h"
#include "LineManager.h"
//数学関数
#define _USE_MATH_DEFINES

///=============================================================================
///						初期化　
void Enemy::Initialize(Object3d *object3d) {
	//========================================
	// Object3D
	object3d_ = object3d;
	//========================================
	// 当たり判定との同期
	BaseObject::Initialize(transform.translate, 0.2f);
}

///=============================================================================
///						
void Enemy::Update(const Vector3 &playerPos) {
	//========================================
	// ライフが0以下になったら
	if(life <= 0) {
		isAlive = false;
	}

	playerPosition = playerPos;
	//========================================
	// 移動処理
	if(isAlive) {
		Move();
	}
	//========================================
	// 撃破時のエフェクト
	if(!isAlive) {
			//放射状に線を描画してエフェクトを出す
		float currentLength = maxLength * ( frameCount / 60.0f ); // 60フレームで最大長に達する
		for(int i = 0; i < numLines; ++i) {
			float angle = ( 2.0f * M_PI / numLines ) * i;
			for(int j = -1; j <= 1; ++j) {
				float verticalAngle = ( M_PI / 2.0f ) * j;
				Vector3 direction = { cos(angle) * cos(verticalAngle), sin(verticalAngle), sin(angle) * cos(verticalAngle) };
				Vector3 endPosition = transform.translate + direction * currentLength;
				LineManager::GetInstance()->DrawLine(transform.translate, endPosition, { 1.0f, 0.0f, 0.0f });
			}
		}
		if(frameCount <= 32) {
			frameCount++;
		}
	}

	//========================================
	// Object3D
	object3d_->SetTransform(transform);
	object3d_->Update();

	//========================================
	// 当たり判定との同期
	if(isAlive) {
		BaseObject::Update(transform.translate);
	} else {
		Vector3 deadPos = { 0.0f, -10.0f, 0.0f };
		BaseObject::Update(deadPos);
	}
}

///=============================================================================
///						
void Enemy::Draw() {
	if(isAlive) {
		object3d_->Draw();
	}
}

///=============================================================================
///						
void Enemy::ImGuiDraw() {
	//エネミーの情報
	ImGui::Begin("Enemy");
	ImGui::Text("Position: %f, %f, %f", transform.translate.x, transform.translate.y, transform.translate.z);
	ImGui::Text("Rotation: %f, %f, %f", transform.rotate.x, transform.rotate.y, transform.rotate.z);
	ImGui::Text("Scale: %f, %f, %f", transform.scale.x, transform.scale.y, transform.scale.z);
	ImGui::Separator();
	ImGui::End();
}

///=============================================================================
///						
void Enemy::OnCollisionEnter(BaseObject *other) {
	//========================================
	//プレイヤーの攻撃判定に当たったら
	if(dynamic_cast<Player *>( other ) != nullptr) {
		//後ろに吹っ飛ぶ
		transform.translate.x += 0.1f;
	}
	//========================================
	// プレイヤーの武器
	if(dynamic_cast<PlayerWepon *>( other ) != nullptr) {
		// プレイヤーの攻撃に当たったら
		//ライフを減らす
		life--;
	}
}

///=============================================================================
///						
void Enemy::OnCollisionStay(BaseObject *other) {
	if(isAlive) {
		if(dynamic_cast<Player *>( other ) != nullptr) {
			// プレイヤーとの当たり判定
			// ここにプレイヤーとの当たり判定時の処理を追加
		}

		//他の敵との当たり判定
		if(dynamic_cast<Enemy *>( other ) != nullptr) {
			// 当たったら現在の動きと反対方向へ
			Vector3 oppositeDirection = -velocity_;
			transform.translate = transform.translate + oppositeDirection * 0.1f;
		}
	}
}

///=============================================================================
///						
void Enemy::OnCollisionExit(BaseObject *other) {
	if(dynamic_cast<Player *>( other ) != nullptr) {}
}

///=============================================================================
///						
void Enemy::Move() {
	// プレイヤーとの距離を計算
	Vector3 playerPos = playerPosition;
	Vector3 direction = playerPos - transform.translate;
	float distance = Length(direction);
	direction = Normalize(direction);

	// 突進のクールダウンを減少
	if(dashCooldown > 0) {
		dashCooldown--;
	}

	// 突進中の場合
	if(isDashing) {
		// 突進の持続時間を減少
		dashDuration--;

		// 突進の持続時間が終了したら突進を終了
		if(dashDuration <= 0) {
			isDashing = false;
			dashCooldown = maxDashCooldown;
		}

		// 突進中はプレイヤーに向かって高速で移動
		acceleration_ = direction * speed_ * 3.0f;
	} else {
		// プレイヤーとの距離が近すぎる場合は離れる
		if(distance < minDistance) {
			acceleration_ = direction * -speed_;
		}
		// プレイヤーとの距離が遠すぎる場合は近づく
		else if(distance > maxDistance) {
			acceleration_ = direction * speed_;
		} else {
			// プレイヤーとの距離が適切な範囲内なら加速度をゼロに
			acceleration_ = { 0.0f, 0.0f, 0.0f };
		}

		// 突進のクールダウンが終了したら突進を開始
		if(dashCooldown <= 0) {
			isDashing = true;
			dashDuration = maxDashDuration;
		}
	}

	// 摩擦による速度減衰
	velocity_ = velocity_ * friction_;

	// 加速度を速度に加算
	velocity_ = velocity_ + acceleration_;

	// 速度を最大速度で制限
	if(Length(velocity_) > maxSpeed_) {
		velocity_ = Normalize(velocity_) * maxSpeed_;
	}

	// 速度を基に新しい位置を計算
	Vector3 newPosition = transform.translate + velocity_;

	// 新しい位置を適用
	transform.translate = newPosition;

	// 移動範囲の制限を適用
	if(transform.translate.x > moveLimit) transform.translate.x = moveLimit;
	if(transform.translate.x < -moveLimit) transform.translate.x = -moveLimit;
	if(transform.translate.z > moveLimit) transform.translate.z = moveLimit;
	if(transform.translate.z < -moveLimit) transform.translate.z = -moveLimit;
}

