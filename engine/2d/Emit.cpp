// Emit.cpp
#include "Emit.h"
#include "Camera.h"
#include <cmath>
#include "MathFunc4x4.h"
#include "AffineTransformations.h"
#include "TextureManager.h"
#include <random>
#define _USE_MATH_DEFINES
#include <math.h>


void Emit::Initialize(ParticleSetup* particleSetup) {
    this->particleSetup_ = particleSetup;
    //パーティクルの生成
    for(uint32_t i = 0; i < 16; ++i) {
        std::unique_ptr<Particle> particle = std::make_unique<Particle>();
        particle->Initialize(particleSetup_);
        particle->SetModel("Particle.obj");
        particles_.push_back(std::move(particle));
		//パーティクルの時間情報を初期化
		times_.push_back(0.0f);
    }
}

void Emit::Update() {
    for(const auto& particleSet : particles_) {
    
        // 中心からランダムにゆっくりと広がるようにパーティクルの位置を設定
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distAngle(0.0f, 2 * M_PI);
        std::uniform_real_distribution<float> distRadius(0.0f, 0.5f); // 半径の範囲を小さくしてゆっくり広がるようにする
    
        float radiusStep = 0.05f; // ステップを小さくしてゆっくり広がるようにする
    
        for (size_t i = 0; i < particles_.size(); ++i) {
            float angle = distAngle(gen);
            float radius = distRadius(gen) * radiusStep * i;
            float x = radius * cos(angle) + position_.x;
            float y = radius * sin(angle) + position_.y;
            float z = position_.z; // z軸方向には広がらないようにする
    
            particles_[i]->SetPosition(Vector3{ x, y, z });
        }
        // スプライトを更新
        particleSet->Update();
    }
}

//for(const auto& particleSet : particles_) {
//
//    // 中心からランダムにゆっくりと広がるようにパーティクルの位置を設定
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<float> distAngle(0.0f, 2 * M_PI);
//    std::uniform_real_distribution<float> distRadius(0.0f, 0.5f); // 半径の範囲を小さくしてゆっくり広がるようにする
//
//    float radiusStep = 1.05f; // ステップを小さくしてゆっくり広がるようにする
//
//    for (size_t i = 0; i < particles_.size(); ++i) {
//        float angle = distAngle(gen);
//        float radius = distRadius(gen) * radiusStep * i;
//        float x = radius * cos(angle);
//        float y = radius * sin(angle);
//        float z = 0.0f; // z軸方向には広がらないようにする
//
//        particles_[i]->SetPosition(Vector3{ x, y, z });
//    }
//    // スプライトを更新
//    particleSet->Update();
//}

void Emit::Draw() {
    for(const auto& particleSet : particles_) {
        // スプライトを描画
        particleSet->Draw();
    }
}

void Emit::EmitParticles() {
}
