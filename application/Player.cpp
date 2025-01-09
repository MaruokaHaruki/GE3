/*********************************************************************
 * \file   Player.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#include "Player.h"
#include "Enemy.h"
#include "Input.h"
#include "MAudioG.h"
#include "CameraManager.h"

///=============================================================================
///						初期化
void Player::Initialize(Object3d *object3d) {
	//========================================
	// Object3D
	object3d_ = object3d;
	velocity = { 0.0f, 0.0f, 0.0f };
	acceleration = { 0.0f, 0.0f, 0.0f };
	maxSpeed = 0.1f; // 最大速度を設定

	//========================================
	// 当たり判定との同期
	BaseObject::Initialize(transform.translate, 0.1f);
}

///=============================================================================
///						更新
void Player::Update() {
	//========================================
	// アニメーション
	AnimationRun();

	//========================================
	// 移動処理
	Move();

	//========================================
	// Object3D
	object3d_->SetTransform(transform);
	object3d_->Update();

	//========================================
	// 回避処理
	if(Input::GetInstance()->PushKey(DIK_SPACE)) {
		Dodge();
	}
	//コントローラーのボタン
	if(Input::GetInstance()->TriggerButton(Input::BUTTON_A)) {
		Dodge();
	}

	//========================================
	// 当たり判定との同期
	BaseObject::Update(transform.translate);

	//========================================
	// 追跡カメラ
	ChaseCamera();
}

///=============================================================================
///						描画
void Player::Draw() {
	object3d_->Draw();
}

///=============================================================================
/// 
void Player::ImGuiDraw() {
	//========================================
	// Player情報すべて
	ImGui::Begin("Player");
	ImGui::DragFloat3("Position", &transform.translate.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transform.rotate.x, 0.01f);
	ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f);
	ImGui::Separator();
	//あたってる状態をフラグで表示
	ImGui::Text("HitEnter: %s", isHitEnter ? "true" : "false");
	ImGui::Text("HitStay: %s", isHitStay ? "true" : "false");
	ImGui::Text("HitExit: %s", isHitExit ? "true" : "false");
	ImGui::Separator();
	ImGui::End();
}

///=============================================================================
///						接触開始処理
void Player::OnCollisionEnter(BaseObject *other) {
	//========================================
	// 敵との衝突判定
	if(dynamic_cast<Enemy *>( other ) != nullptr) {
		//現在の進行方向と逆にVloocityを加算
		velocity.x *= -velocity.x;
		velocity.z *= -velocity.z;
	}
	velocity.x *= -velocity.x;
	velocity.z *= -velocity.z;

	//========================================
	// フラグ
	isHitEnter = true;
	//離れたフラグをリセット
	isHitExit = false;
}

///=============================================================================
///						接触継続処理
void Player::OnCollisionStay(BaseObject *other) {
	if(dynamic_cast<Enemy *>( other ) != nullptr) {}
	//========================================
	// フラグ
	isHitStay = true;
	//開始フラグをリセット
	isHitEnter = false;
}

///=============================================================================
///						接触終了処理
void Player::OnCollisionExit(BaseObject *other) {

	if(dynamic_cast<Enemy *>( other ) != nullptr) {}

	//========================================
	// フラグ
	isHitExit = true;
	//継続フラグをリセット
	isHitStay = false;
}

///=============================================================================
///						移動処理
void Player::Move() {
	//========================================
	// キーで移動処理WASD
	if(Input::GetInstance()->PushKey(DIK_W)) {
		acceleration.z = 0.01f;
	} else if(Input::GetInstance()->PushKey(DIK_S)) {
		acceleration.z = -0.01f;
	} else {
		acceleration.z = 0.0f;
	}

	if(Input::GetInstance()->PushKey(DIK_A)) {
		acceleration.x = -0.01f;
	} else if(Input::GetInstance()->PushKey(DIK_D)) {
		acceleration.x = 0.01f;
	} else {
		acceleration.x = 0.0f;
	}

	//========================================
	// コントローラのスティック操作
	acceleration.x += Input::GetInstance()->GetLeftStickX() * 0.01f;
	acceleration.z += Input::GetInstance()->GetLeftStickY() * 0.01f;

	//========================================
	// 速度に加速度を加算
	velocity.x += acceleration.x;
	velocity.z += acceleration.z;

	//========================================
	// 最大速度を超えないようにする
	if(velocity.x > maxSpeed) velocity.x = maxSpeed;
	if(velocity.x < -maxSpeed) velocity.x = -maxSpeed;
	if(velocity.z > maxSpeed) velocity.z = maxSpeed;
	if(velocity.z < -maxSpeed) velocity.z = -maxSpeed;

	//========================================
	// 速度を位置に加算
	transform.translate.x += velocity.x;
	transform.translate.z += velocity.z;

	//========================================
	// 移動範囲の制限を適用
	if(transform.translate.x > moveLimit) transform.translate.x = moveLimit;
	if(transform.translate.x < -moveLimit) transform.translate.x = -moveLimit;
	if(transform.translate.z > moveLimit) transform.translate.z = moveLimit;
	if(transform.translate.z < -moveLimit) transform.translate.z = -moveLimit;

	//========================================
	// 減速処理	
	velocity.x *= deceleration;
	velocity.z *= deceleration;
}

///=============================================================================
///						回避
void Player::Dodge() {
	//========================================
	// 回避処理
	isDodge = true;

	//========================================
	// スティックの傾きの方向に回避
	acceleration.x = Input::GetInstance()->GetLeftStickX() * 0.8f;
	acceleration.z = Input::GetInstance()->GetLeftStickY() * 0.8f;

	//========================================
	// 速度に加速度を加算
	velocity.x += acceleration.x;
	velocity.z += acceleration.z;
}

///=============================================================================
///						アニメーション
void Player::AnimationRun() {
	// 移動中は一定時間ごとに走りアニメーションをする
	if(acceleration.x != 0.0f || acceleration.z != 0.0f) {
		count++;
		// 0.8秒ごとにアニメーションを変える
		if(acceleration.x > 0.0f) {
			if(count % 7 == 0) {
				object3d_->ChangeTexture("player_right_run_01.png");
			}
			if(count % 14 == 0) {
				object3d_->ChangeTexture("player_right_run_02.png");
			}
			if(count % 21 == 0) {
				object3d_->ChangeTexture("player_right_run_03.png");
			}
		} else if(acceleration.x < 0.0f) {
			if(count % 7 == 0) {
				object3d_->ChangeTexture("player_left_run_01.png");
			}
			if(count % 14 == 0) {
				object3d_->ChangeTexture("player_left_run_02.png");
			}
			if(count % 21 == 0) {
				object3d_->ChangeTexture("player_left_run_03.png");
			}
		} else if(acceleration.z != 0.0f) {
			if(velocity.x > 0.0f) {
				if(count % 7 == 0) {
					object3d_->ChangeTexture("player_right_run_01.png");
				}
				if(count % 14 == 0) {
					object3d_->ChangeTexture("player_right_run_02.png");
				}
				if(count % 21 == 0) {
					object3d_->ChangeTexture("player_right_run_03.png");
				}
			} else if(velocity.x < 0.0f) {
				if(count % 7 == 0) {
					object3d_->ChangeTexture("player_left_run_01.png");
				}
				if(count % 14 == 0) {
					object3d_->ChangeTexture("player_left_run_02.png");
				}
				if(count % 21 == 0) {
					object3d_->ChangeTexture("player_left_run_03.png");
				}
			}
		}
	} else {
		// 移動していない場合は停止中の画像に切り替える
		if(velocity.x > 0.0f) {
			object3d_->ChangeTexture("player_right.png");
		} else if(velocity.x < 0.0f) {
			object3d_->ChangeTexture("player_left.png");
		}
	}
}

///=============================================================================
///						追跡カメラ
void Player::ChaseCamera() {
	// カメラマネージャーからデフォルトカメラを取得
	Camera *camera = CameraManager::GetInstance()->GetCamera("DefaultCamera");
	if(!camera) return;

	// 現在のカメラの位置を取得
	Vector3 currentCameraPos = camera->GetTransform().translate;

	// 目標とするカメラの位置を計算
	Vector3 targetCameraPos = { transform.translate.x, 2.3f, transform.translate.z - 8.0f };

	// イージング係数（0.0f～1.0f）、値を小さくすると追従が遅くなります
	float easingFactor = 0.05f;

	// イージングを適用して新しいカメラ位置を計算
	Vector3 newCameraPos = currentCameraPos + ( targetCameraPos - currentCameraPos ) * easingFactor;

	// カメラの回転角度（固定）
	Vector3 cameraRotation = { 0.3f, 0.0f, 0.0f };

	// カメラのスケール（固定）
	Vector3 cameraScale = { 1.0f, 1.0f, 1.0f };

	// カメラのトランスフォームを更新
	camera->SetTransform({ cameraScale, cameraRotation, newCameraPos });

	// カメラの更新
	CameraManager::GetInstance()->GetCamera("DefaultCamera")->SetTransform({ cameraScale, cameraRotation, newCameraPos });
}
