// Emit.h
#pragma once
#include "Particle.h"
#include "ParticleSetup.h"
#include <vector>

class ParticleEmitter {
    ///--------------------------------------------------------------
    ///							メンバ関数
public:

    // コンストラクタ
    ParticleEmitter(Particle* particle, const std::string& name, const Transform& transform, uint32_t count, float frequency, bool repeat = false);


    /// \brief 更新
    void Update();

    /// \brief 描画 
    void Draw();

    /**----------------------------------------------------------------------------
     * \brief  Emit 
     */
	void Emit();

    /**----------------------------------------------------------------------------
     * \brief  SetRepeat 
     * \param  repeat
     */
    void SetRepeat(bool repeat);

    ///--------------------------------------------------------------
    ///							静的メンバ関数
private:


    ///--------------------------------------------------------------
    ///							メンバ変数
private:
    Particle* particle_; // ParticleManagerのインスタンスを保持
    std::string name_;                 // パーティクルグループ名
    Transform transform_;              // エミッターの位置・回転・スケール
    uint32_t count_;                   // 発生させるパーティクルの数
    float frequency_;                  // 発生頻度
    float elapsedTime_;                // 経過時間
    bool repeat_;                      // 繰り返し発生させるかどうかのフラグ

};
