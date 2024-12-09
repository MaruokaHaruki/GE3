// Emit.h
#pragma once
#include "Particle.h"
#include "ParticleSetup.h"
#include <vector>

class Emit {
    ///--------------------------------------------------------------
    ///							メンバ関数
public:

    /// \brief 初期化
    void Initialize(ParticleSetup* particleSetup);

    /// \brief 更新
    void Update();

    /// \brief 描画 
    void Draw();

	void SetPosition(const Vector3& position) { position_ = position; }

    ///--------------------------------------------------------------
    ///							静的メンバ関数
private:
    void EmitParticles();

    ///--------------------------------------------------------------
    ///							メンバ変数
private:
    ParticleSetup* particleSetup_ = nullptr;
    std::vector<std::unique_ptr<Particle>> particles_;
	//パーティクルの時間情報群
	std::vector<float> times_;
    
	Vector3 position_ = { 0.0f, 0.0f, 0.0f }; // パーティクルの発生位置

	float speed_ = 1.0f; // パーティクルの速度
    float emitRate_ = 0.1f; // パーティクルの発生頻度
    float emitTimer_ = 0.0f;
    float particleLifetime_ = 2.0f; // パーティクルの消える時間
};
