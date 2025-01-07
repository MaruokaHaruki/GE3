/*********************************************************************
 * \file   Player.h
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#pragma once
#include "BaseObject.h"
#include "Object3d.h"
#include "PlayerWepon.h"

///=============================================================================
///						プレイヤークラス
class Player : public BaseObject {

  ///--------------------------------------------------------------
  ///							メンバ関数
public:
	/// \brief 初期化
	void Initialize(Object3d *object3d);

	/// \brief 更新
	void Update();

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

	/**----------------------------------------------------------------------------
	 * \brief  Move
	 */
	void Move();

	/**----------------------------------------------------------------------------
	 * \brief  Attack
	 */
	void Attack();

	/**----------------------------------------------------------------------------
	 * \brief  Dodge
	 */
	void Dodge();

	/**----------------------------------------------------------------------------
	 * \brief  AnimationRun アニメーション実行
	 */
	void AnimationRun();

	///--------------------------------------------------------------
	///							入出力関数
public:

	//========================================
	// 武器の取得
	PlayerWepon *GetWepon() { return wepon_.get(); }

	/**----------------------------------------------------------------------------
	 * \brief  GetPosition 場所の取得
	 * \return Vector3
	 * \note
	 */
	Vector3 GetPosition() const { return transform.translate; }

	///--------------------------------------------------------------
	///							メンバ変数
private:
	//========================================
	// Object3D
	Object3d *object3d_ = nullptr;
	// 場所
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//========================================
	// 移動系
	// 加速度
	Vector3  acceleration{ 0.0f,0.0f,0.0f };
	//減速
	float deceleration = 0.7f;
	// 速度
	Vector3 velocity{ 0.0f,0.0f,0.0f };
	// 最大速度
	float maxSpeed = 0.8f;

	//========================================
	// 移動制限
	float moveLimit = 2.0f;

	//========================================
	// アニメーション
	int count = 0;

	//========================================
	// 回避フラグ
	bool isDodge = false;
	//回避クールタイム
	int dodgeCoolTime = 0;

	//========================================
	// ヒットフラグ
	bool isHitEnter = false;
	bool isHitStay = false;
	bool isHitExit = false;

	//========================================
	// 武器
	std::unique_ptr<PlayerWepon> wepon_;
	// 武器の判定時間
	int weponHitTime = 0;
	int weponHitTimeReset = 8;
};

