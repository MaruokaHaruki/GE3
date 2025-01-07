/*********************************************************************
 * \file   TitleScene.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
#include "BaseScene.h"
//========================================
// Game
#include "Object3d.h"
#include "Sprite.h"

///=============================================================================
///						タイトルシーンクラス
class TitleScene : public BaseScene {
	///--------------------------------------------------------------
	///							メンバ関数
public:
	/// \brief 初期化
	void Initialize(SpriteSetup *spriteSetup, Object3dSetup *object3dSetup, ParticleSetup *particleSetup) override;

	/// \brief 終了処理
	void Finalize() override;

	/// \brief 更新
	void Update() override;

	/// @brie 2D描画
	void Object2DDraw() override;

	/// \brief 3D描画 
	void Object3DDraw() override;

	/// \brief パーティクル描画
	void ParticleDraw() override;

	/// \brief ImGui描画
	void ImGuiDraw() override;

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
	// オブジェクト
	std::unique_ptr<Object3d> objTitle_ = nullptr;
	// 場所
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//回転用
	float angle = 0.0f;

	//========================================
	// スプライト
	std::unique_ptr<Sprite> pressSprite_;
	Transform transformSprite{ {256.0f,256.0f,256.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//マテリアル
	Vector4 materialSprite = {1.0f, 1.0f, 1.0f, 1.0f};
	//UV座標
	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

};

