/*********************************************************************
 * \file   SceneManager.h
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#pragma once
#include <memory>
#include "BaseScene.h"

///=============================================================================
///						シーンマネージャ
class SceneManager {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(SpriteSetup *spriteSetup, Object3dSetup *object3dSetup, ParticleSetup *particleSetup);

	/// @brief 
	void Finalize();

	/// @brief 更新処理
	void Update();

	/// @brief 描画
	void Object2DDraw();

	/// @brief 描画
	void Object3DDraw();

	/// @brief 描画
	void ParticleDraw();

	/// @brief ImGui描画
	void ImGuiDraw();

	///--------------------------------------------------------------
	///							メンバ変数
private:
	//========================================
	// 今のシーン
	std::unique_ptr<BaseScene> nowScene_;
	// 次のシーン
	std::unique_ptr<BaseScene> nextScene_;


	//========================================
	// 現在のシーン番号　
	int currentSceneNo_;
	// 前のシーン番号
	int prevSceneNo_;

	//========================================
	// Sprite共通部
	SpriteSetup *spriteSetup_ = nullptr;
	// 3Dオブジェクト共通部
	Object3dSetup *object3dSetup_ = nullptr;
	// パーティクル共通部
	ParticleSetup *particleSetup_ = nullptr;
};