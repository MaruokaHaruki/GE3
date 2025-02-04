/*********************************************************************
 * \file   GamePlayScene.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
#include "BaseScene.h"
#include <memory>
//========================================
// Game
#include "Camera.h"
#include "Sprite.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Object3d.h"
#include "Model.h"
#include "MAudioG.h"
#include "CollisionManager.h"
#include "Player.h"
#include "Ground.h"
#include "Enemy.h"

///=============================================================================
///						ゲームプレイシーンクラス
class GamePlayScene : public BaseScene {
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
	// 当たり判定
	std::unique_ptr<CollisionManager> collisionManager_;
	//3Dオブジェクト
	std::unique_ptr<Object3d> objCollisionManager_;

	//========================================
	// 地面
	std::unique_ptr<Ground> ground_;
	// 3dオブジェクト
	std::unique_ptr<Object3d> objGround_;

	//========================================
	// プレイヤー
	std::unique_ptr<Player> player_;
	// 3dオブジェクト
	std::unique_ptr<Object3d> objPlayer_;

	//========================================
	//　敵
	std::unique_ptr<Enemy> enemy_;
	// 3dオブジェクト
	std::unique_ptr<Object3d> objEnemy_;
	//複数体の敵の格納
	std::vector<std::unique_ptr<Enemy>> enemyList_;
	std::vector<std::unique_ptr<Object3d>> objEnemyList_;
	//敵の数
	int enemyNum_ = 32;
	//========================================
	// スプライト
	std::unique_ptr<Sprite> moveSprite_;
	//========================================
	// パーティクル
	std::unique_ptr<Particle> particle_;
	//エミッター
	std::unique_ptr<ParticleEmitter> particleEmitter_;
	//========================================
	// 倒した敵の数
	int defeatedEnemies_ = 0;
	// クリアに必要な敵の数
	const int requiredDefeatedEnemies_ = 2;
};

