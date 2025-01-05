/*********************************************************************
 * \file   MaruRhythm.h
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#pragma once
///=============================================================================
///						インクルードファイル
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
//---------------------------------------
// コムポインタ
#include <wrl.h>
#include <memory> // std::unique_ptr
//---------------------------------------
// ファイル読み込み用
#include <fstream>
#include <sstream>
//========================================
// Framework
#include "WinApp.h"
#include "DirectXCore.h"
#include "ImguiSetup.h"
#include "SrvSetup.h"
#include "Input.h"
#include "MAudioG.h"
#include "SpriteSetup.h"
#include "TextureManager.h"
#include "ParticleSetup.h"
#include "Object3dSetup.h"
#include "ModelManager.h"
//
#include "CameraManager.h"

//========================================
// Game
#include "Camera.h"
#include "Sprite.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Object3d.h"
#include "Model.h"

#include "MRFramework.h"

///=============================================================================
///						MaruRhythmクラス
class MaruRhythm : public MRFramework {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize() override;

	/// \brief 終了処理
	void Finalize() override;

	/// \brief 更新
	void Update() override;

	/// \brief 描画 
	void Draw() override;

	///--------------------------------------------------------------
	///							静的メンバ関数
private:

	///--------------------------------------------------------------
	///							入出力関数
public:

	///--------------------------------------------------------------
	///							メンバ変数(FrameWork)
private:
	// オーディオ
	MAudioG *audio_;

	///--------------------------------------------------------------
	///						 Game
	//========================================
	// カメラ
	std::unique_ptr<Camera> camera_;
	//========================================
	// スプライト
	std::unique_ptr<Sprite> sprite_;
	//========================================
	// 3dオブジェクト
	std::unique_ptr<Object3d> object3d_;
	//========================================
	// パーティクル
	std::unique_ptr<Particle> particle_;
	//エミッター
	std::unique_ptr<ParticleEmitter> particleEmitter_;

	///--------------------------------------------------------------
	///						 テスト用
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//========================================
	// 2Dオブジェクト用
	//TransformSprite
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

