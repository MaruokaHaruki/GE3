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


	///--------------------------------------------------------------
	///							メンバ変数
private:
	//========================================
	// Object3D
	Object3d *object3d_ = nullptr;
	// 場所
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
};

