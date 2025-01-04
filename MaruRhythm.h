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

//========================================
// Game
#include "Camera.h"
#include "Sprite.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Object3d.h"
#include "Model.h"
#include "MAudioG.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Transform.h"

///=============================================================================
///						MaruRhythmクラス
class MaruRhythm {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize();

	/// \brief 終了処理
	void Finalize();

	/// \brief 更新
	void Update();

	/// \brief 描画 
	void Draw();

	///--------------------------------------------------------------
	///							静的メンバ関数
private:

	///--------------------------------------------------------------
	///							入出力関数
public:
	/**----------------------------------------------------------------------------
	 * \brief  IsEndRequest 終了リクエストの取得
	 * \return 終了リクエスト
	 */
	bool IsEndRequest() const { return isEndRequest_; }

	///--------------------------------------------------------------
	///							メンバ変数(FrameWork)
private:
	//========================================
	// ゲーム終了フラグ
	bool isEndRequest_ = false;

	//========================================
	// ウィンドウクラス
	std::unique_ptr<WinApp> win_;
	//========================================
	// ダイレクトX
	std::unique_ptr<DirectXCore> dxCore_;
	//========================================
	// ImGui
	std::unique_ptr<ImguiSetup> imguiSetup_;
	//========================================
	// SrvSetup
	std::unique_ptr<SrvSetup> srvSetup_;
	//========================================
	// 入力
	std::unique_ptr<Input> input_;
	//========================================
	// スプライト共通部
	std::unique_ptr<SpriteSetup> spriteSetup_;
	//========================================
	// パーティクルセットアップ
	std::unique_ptr<ParticleSetup> particleSetup_;
	//========================================
	// 3Dオブジェクト共通部
	std::unique_ptr<Object3dSetup> object3dSetup_;
	//========================================
	// モデルセットアップ
	std::unique_ptr<ModelSetup> modelSetup_;
	//========================================
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

