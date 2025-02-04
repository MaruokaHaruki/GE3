/*********************************************************************
 * \file   PlayerWepon.h
 * \brief
 *
 * \author Harukichimaru
 * \date   February 2025
 * \note
 *********************************************************************/
#pragma once
#include "BaseObject.h"
#include "Object3d.h"

///=============================================================================
///						プレイヤーの武器クラス
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
	void OnCollisionEnter(BaseObject *other);
	/// \brief 衝突継続時の処理
	void OnCollisionStay(BaseObject *other);
	/// \brief 衝突終了時の処理
	void OnCollisionExit(BaseObject *other);

	///--------------------------------------------------------------
	///							入出力関数
	/**----------------------------------------------------------------------------
	 * \brief  SetPosition	
	 * \param  position
	 */
	void SetPosition(const Vector3 &position) { transform.translate = position; }
	/**----------------------------------------------------------------------------
	 * \brief  GetPosition 
	 * \return 
	 */
	Vector3 GetPosition() const { return transform.translate; }
	

	///--------------------------------------------------------------
	///							メンバ変数
private:
	// 場所
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,-10.0f,0.0f} };
};

