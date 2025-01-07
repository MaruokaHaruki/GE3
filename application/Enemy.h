/*********************************************************************
 * \file   Enemy.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
#include "BaseObject.h"
#include "Object3d.h"

///=============================================================================
///						敵クラス
class Enemy : public BaseObject {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(Object3d *object3d);

	/// \brief 更新
	void Update(const Vector3& playerPos);

	/// \brief 描画 
	void Draw();

	/// \brief ImGui描画
	void ImGuiDraw();

	/// \brief 衝突開始時の処理
	void OnCollisionEnter(BaseObject *other) override;

	/// \brief 衝突継続時の処理
	void OnCollisionStay(BaseObject *other) override;

	/// \brief 衝突終了時の処理
	void OnCollisionExit(BaseObject *other) override;

	///--------------------------------------------------------------
	///							静的メンバ関数
private:

	///--------------------------------------------------------------
	///							入出力関数
public:

	/**----------------------------------------------------------------------------
	 * \brief  Move 
	 */
	void Move();

	/**----------------------------------------------------------------------------
	 * \brief  Attack
	 */
	bool GetIsAlive() const { return isAlive; }

	///--------------------------------------------------------------
	///							メンバ変数
private:
	//========================================
	// Object3D
	Object3d *object3d_ = nullptr;
	// 場所
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//========================================
	// プレイヤーの位置情報
	Vector3 playerPosition{ 0.0f,0.0f,0.0f };

	//========================================
	// 移動系
	// 加速度
	// 現在の速度
	Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };
	// 現在の加速度
	Vector3 acceleration_ = { 0.0f, 0.0f, 0.0f };
	// 最大速度
	float maxSpeed = 0.8f;
	// プレイヤーに近づかない最小距離
	float minDistance = 1.0f;
	// プレイヤーに接近する最大距離
	float maxDistance = 2.0f;
	//加算速度
	float speed_ = 0.1f;

	// 摩擦係数、速度を減衰させるために使用
	float friction_ = 1.98f;
	// 最大速度
	float maxSpeed_ = 0.05f;
	//========================================
	// 移動範囲
	float moveLimit = 2.0f;

	//========================================
	// 死亡フラグ
	bool isAlive = true;
};

