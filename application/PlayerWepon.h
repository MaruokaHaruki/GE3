/*********************************************************************
 * \file   PlayerWepon.h
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#pragma once
#include "BaseObject.h"
#include "Object3d.h"

class PlayerWepon : public BaseObject {
	///--------------------------------------------------------------
	///							メンバ関数
public:
	/// \brief 初期化
	void Initialize();

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

	///--------------------------------------------------------------
	///							入出力関数
public:

	/**----------------------------------------------------------------------------
	 * \brief  SetPosition 場所の設定
	 * \param  position 場所
	 */
	void SetPosition(Vector3 position) {
		transform.translate = position;
	}

	///--------------------------------------------------------------
	///							メンバ変数
private:
	// 場所
	Transform transform{ {3.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//========================================
	// アニメーション
	int count = 0;

	//========================================
	// ヒットフラグ
	bool isHitEnter = false;
	bool isHitStay = false;
	bool isHitExit = false;
};

