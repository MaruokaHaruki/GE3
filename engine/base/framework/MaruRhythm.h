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
// Game
#include "Camera.h"
#include "Sprite.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Object3d.h"
#include "Model.h"


#include "GamePlayScene.h"
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
	///							メンバ変
private:
	//========================================
	// ゲームシーン
	std::unique_ptr<GamePlayScene> gamePlayScene_;
};

