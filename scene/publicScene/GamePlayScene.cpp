/*********************************************************************
 * \file   GamePlayScene.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#include "GamePlayScene.h"
#include "CameraManager.h"

///=============================================================================
///						初期化
void GamePlayScene::Initialize(SpriteSetup *spriteSetup, Object3dSetup *object3dSetup, ParticleSetup *particleSetup) {
	//適当に引数を使用
	//引数を使用しない場合は警告を出さないようにする
	spriteSetup;
	object3dSetup;
	particleSetup;
	//========================================
	// カメラ設定
	CameraManager::GetInstance()->GetCamera("DefaultCamera")->SetTransform({ {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,2.3f,-8.0f} });

	//========================================
	// テクスチャの読み込み
	//右向き
	TextureManager::GetInstance()->LoadTexture("player_right.png");
	TextureManager::GetInstance()->LoadTexture("player_right_run_01.png");
	TextureManager::GetInstance()->LoadTexture("player_right_run_02.png");
	TextureManager::GetInstance()->LoadTexture("player_right_run_03.png");
	//左向き
	TextureManager::GetInstance()->LoadTexture("player_left.png");
	TextureManager::GetInstance()->LoadTexture("player_left_run_01.png");
	TextureManager::GetInstance()->LoadTexture("player_left_run_02.png");
	TextureManager::GetInstance()->LoadTexture("player_left_run_03.png");
	//チュートリアル
	TextureManager::GetInstance()->LoadTexture("move.png");


	//========================================
	// オブジェクト読み込み
	//3Dオブジェクトの読み込み
	ModelManager::GetInstance()->LoadMedel("ground.obj");
	ModelManager::GetInstance()->LoadMedel("player.obj");
	ModelManager::GetInstance()->LoadMedel("hitCircle.obj");
	ModelManager::GetInstance()->LoadMedel("enemy.obj");

	//========================================
	// スプライトクラス(Game)
	//ユニークポインタ
	moveSprite_ = std::make_unique<Sprite>();
	//スプライトの初期化
	moveSprite_->Initialize(spriteSetup, "move.png");
	//サイズ
	moveSprite_->SetSize({ 256.0f,128.0f });
	moveSprite_->SetPosition(Vector2{ 500.0f, 600.0f });

	//========================================
	// 地面
	objGround_ = std::make_unique<Object3d>();
	objGround_->Initialize(object3dSetup);
	objGround_->SetModel("ground.obj");
	//地面の初期化
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(objGround_.get());

	//========================================
	// プレイヤー
	objPlayer_ = std::make_unique<Object3d>();
	objPlayer_->Initialize(object3dSetup);
	objPlayer_->SetModel("player.obj");
	//プレイヤーの初期化
	player_ = std::make_unique<Player>();
	player_->Initialize(objPlayer_.get());

	//========================================
	//復数敵の初期化
	//========================================
	// 複数の敵
	for (int i = 0; i < enemyNum_; ++i) {
		auto objEnemy = std::make_unique<Object3d>();
		objEnemy->Initialize(object3dSetup);
		objEnemy->SetModel("enemy.obj");
		objEnemyList_.push_back(std::move(objEnemy));

		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize(objEnemyList_.back().get());
		//敵の位置をランダムに設定
		enemy->SetPosition({ static_cast<float>( rand() % 10 ),0.0f,static_cast<float>( rand() % 10 ) });

		enemyList_.push_back(std::move(enemy));
	}

	//========================================
	// パーティクルクラス
	particle_ = std::make_unique<Particle>();
	//パーティクルの初期化
	particle_->Initialize(particleSetup);
	particle_->SetCustomTextureSize({ 10.0f,10.0f });
	//パーティクルグループの作成
	particle_->CreateParticleGroup("Particle", "sandWind.png");
	// パーティクルエミッター
	particleEmitter_ =
		std::make_unique<ParticleEmitter>(particle_.get(),
			"Particle",
			Transform{ {0.1f,0.1f,0.1f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} },
			4,
			0.5f,
			true);
	

	//========================================
	// 当たり判定
	objCollisionManager_ = std::make_unique<Object3d>();
	objCollisionManager_->Initialize(object3dSetup);
	objCollisionManager_->SetModel("hitCircle.obj");
	//当たり判定の初期化
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize(objCollisionManager_.get());

	
}

///=============================================================================
///						終了処理
void GamePlayScene::Finalize() {
	//========================================
	// BGMの停止
	MAudioG::GetInstance()->StopWav("Beast-Mode.wav");
}

///=============================================================================
///						更新
void GamePlayScene::Update() {
	defeatedEnemies_ = 0;

	//========================================
	// BGMの再生
	if(MAudioG::GetInstance()->IsWavPlaying("Beast-Mode.wav") == false) {
		MAudioG::GetInstance()->PlayWav("Beast-Mode.wav");
	}
	//=======================================
	//画面中央下に配置
	moveSprite_->Update();
	//========================================
	// 地面
	ground_->Update();
	//========================================
	// プレイヤー
	player_->Update();
	//========================================
	// 複数の敵
	for (auto& enemy : enemyList_) {
		enemy->Update(player_->GetPosition());
	}
	//========================================
	// パーティクル系
	particleEmitter_->SetTranslate(player_->GetTransform().translate);
	//パーティクルの更新
	particle_->Update();
	//パーティクルエミッターの更新
	particleEmitter_->Update();
	//========================================
	// 当たり判定
	//リセット
	collisionManager_->Reset();
	//復数の敵
	for(auto &enemy : enemyList_) {
		collisionManager_->AddCollider(enemy.get());
	}
	collisionManager_->AddCollider(player_.get());
	collisionManager_->AddCollider(player_->GetPlayerWepon());
	//更新
	collisionManager_->Update();

	//========================================
	// 敵が何人生きているか集計
	for(auto &enemy : enemyList_) {
		if(enemy->IsAlive()) {
			defeatedEnemies_++;
		}
	}
	//もし敵が全滅していたらクリア
	if(defeatedEnemies_ <= requiredDefeatedEnemies_) {
		sceneNo = SCENE::CLEAR;
	}

}

///=============================================================================
///						
void GamePlayScene::Object2DDraw() {
	//========================================
	// チュートリアル
	moveSprite_->Draw();

}

///=============================================================================
///						
void GamePlayScene::Object3DDraw() {
	//========================================
	// 地面
	ground_->Draw();

	//========================================
	// プレイヤー
	player_->Draw();

	//========================================
	// 複数の敵
	for (auto& enemy : enemyList_) {
		enemy->Draw();
	}

	//========================================
	// 当たり判定
	collisionManager_->Draw();
}

///=============================================================================
///						
void GamePlayScene::ParticleDraw() {
	//========================================
	// パーティクル描画
	particle_->Draw();
	particleEmitter_->Draw();
}

///=============================================================================
///						ImGui描画
void GamePlayScene::ImGuiDraw() {
#ifdef _DEBUG
	ImGui::Begin("DebugScene");
	ImGui::Text("Hello, GamePlayScene!");
	//倒された敵の数
	ImGui::Text("Defeated Enemies: %d", defeatedEnemies_);
	//必要な敵の数
	ImGui::Text("Required Defeated Enemies: %d", requiredDefeatedEnemies_);

	ImGui::End();
	//========================================
	// プレイヤー
	player_->ImGuiDraw();

	//========================================
	// 敵

	//========================================
	// 当たり判定
	collisionManager_->DrawImGui();
#endif // _DEBUG
}
