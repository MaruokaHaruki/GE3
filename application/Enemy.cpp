/*********************************************************************
 * \file   Enemy.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "Enemy.h"
#include "Player.h"
#include "MathFunc4x4.h"
#include "AffineTransformations.h"
#include "Vector3.h"

///=============================================================================
///						初期化　
void Enemy::Initialize(Object3d *object3d) {
	//========================================
	// Object3D
	object3d_ = object3d;

	//========================================
	// 当たり判定との同期
	BaseObject::Initialize(transform.translate, 0.1f);
}

///=============================================================================
///						
void Enemy::Update( const Vector3& playerPos) {
	playerPosition = playerPos;

	//========================================
	// 移動処理
	Move();


    //========================================
    // Object3D
    object3d_->SetTransform(transform);
    object3d_->Update();

	//========================================
	// 当たり判定との同期
	BaseObject::Update(transform.translate);
}

///=============================================================================
///						
void Enemy::Draw() {
	object3d_->Draw();
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
	//プレイヤーの攻撃判定に当たったら
	if(dynamic_cast<Player *>( other ) != nullptr) {
		//後ろに吹っ飛ぶ
		transform.translate.x += 0.1f;
	}
}

///=============================================================================
///						
void Enemy::OnCollisionStay(BaseObject *other) {
    if(dynamic_cast<Player *>( other ) != nullptr) {}
}

///=============================================================================
///						
void Enemy::OnCollisionExit(BaseObject *other) {
    if(dynamic_cast<Player *>( other ) != nullptr) {}
}

///=============================================================================
///						
void Enemy::Move() {
    /// ===プレイヤーとの距離を計算=== ///
    // プレイヤー位置を取得
    Vector3 playerPos = playerPosition;
    // プレイヤーへのベクトル
    Vector3 direction = playerPos - transform.translate;
    // 距離を取得
    float distance = Length(direction);

    // ベクトルの正規化
    direction = Normalize(direction);

    /// ===八方向の移動ベクトル（45度ずつ）=== ///
    const std::vector<Vector3> directions = {
    {1, 0, 0}, {1, 0, 1}, {0, 0, 1}, {-1, 0, 1},
    {-1, 0, 0}, {-1, 0, -1}, {0, 0, -1}, {1, 0, -1}
    };

    /// ===最も近い八方向ベクトルを選択=== ///
    float maxDot = -1.0f;
    int bestIndex = 0;
    for (int i = 0; i < directions.size(); ++i) {
    // 内積で方向の近さを判定
    float dot = Dot(direction, directions[i]);
    if (dot > maxDot) {
    maxDot = dot;
    bestIndex = i;
    }
    }

    /// ===プレイヤーとの距離が近くなった場合、最も近いベクトルの隣のベクトルを選択=== ///
    Vector3 selectedDirection;
    if (distance < minDistance) {
    // プレイヤーから離れる場合
    selectedDirection = directions[(bestIndex + 7) % directions.size()];
    } else if (distance > maxDistance) {
    // プレイヤーに接近する場合
    selectedDirection = directions[bestIndex];
    } else {
    // プレイヤーとの距離が適切な範囲内であれば、最も近いベクトルを選択
    selectedDirection = directions[(bestIndex + 6) % directions.size()];
    }

    /// ===加速度を選択された八方向ベクトルに基づいて設定=== ///
    if (distance < minDistance) {
    // プレイヤーから遠ざかる加速度
    acceleration_ = selectedDirection * -speed_;
    } else if (distance > maxDistance) {
    // プレイヤーに接近する加速度
    acceleration_ = selectedDirection * speed_;
    } else {
    // プレイヤーとの距離が適切な範囲内なら加速度をゼロに
    acceleration_ = selectedDirection * speed_;
    }

    /// ===摩擦による速度減衰=== ///
    velocity_ = velocity_ * friction_;

    /// ===加速度を速度に加算=== ///
    velocity_ = velocity_ + acceleration_;

    /// ===速度を最大速度で制限=== ///
    if (Length(velocity_) > maxSpeed_) {
    velocity_ = Normalize(velocity_) * maxSpeed_;
    }

    /// ===速度を基に新しい位置を計算=== ///
    Vector3 newPosition = transform.translate + velocity_;

    /// ===新しい位置を適用=== ///
    transform.translate = newPosition;

	// 移動範囲の制限を適用
    if(transform.translate.x > moveLimit) transform.translate.x = moveLimit;
    if(transform.translate.x < -moveLimit) transform.translate.x = -moveLimit;
    if(transform.translate.z > moveLimit) transform.translate.z = moveLimit;
    if(transform.translate.z < -moveLimit) transform.translate.z = -moveLimit;
}
