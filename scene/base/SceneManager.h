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
#include "AbstractSceneFactory.h"

///=============================================================================
///						シーンマネージャ
class SceneManager {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(SpriteSetup *spriteSetup, Object3dSetup *object3dSetup, ParticleSetup *particleSetup);

	/// @brief 終了処理
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

	/// @brief シーンファクトリーのSetter
	void SetSceneFactory(AbstractSceneFactory *sceneFactory) {
		sceneFactory_ = sceneFactory;
	}

	///--------------------------------------------------------------
	///							メンバ変数
private:
	//========================================
	// シーンファクトリーポインタ
	AbstractSceneFactory *sceneFactory_ = nullptr;

	//========================================
	// 今のシーン
	std::unique_ptr<BaseScene> nowScene_;
	// 次のシーン
	std::unique_ptr<BaseScene> nextScene_;

	//========================================
	// 現在のシーン番号　
	int currentSceneNo_ = 0;
	// 前のシーン番号
	int prevSceneNo_ = -1;

	//========================================
	// Sprite共通部
	SpriteSetup *spriteSetup_ = nullptr;
	// 3Dオブジェクト共通部
	Object3dSetup *object3dSetup_ = nullptr;
	// パーティクル共通部
	ParticleSetup *particleSetup_ = nullptr;
};