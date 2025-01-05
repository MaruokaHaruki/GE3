/*********************************************************************
 * \file   MRFramework.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
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
#include "Camera.h"
#include "CameraManager.h"
#include "SceneManager.h"

///=============================================================================
///						FrameWorkクラス
class MRFramework {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 仮想デストラクタ
	virtual ~MRFramework() = default;

	/// \brief メインループ
	void Run();

	/// \brief 初期化
	virtual void Initialize();

	/// \brief 更新
	virtual void Update();

	/// \brief 描画 
	virtual void Draw() = 0;

	/// \brief 終了処理
	virtual void Finalize();

	///--------------------------------------------------------------
	///						 静的メンバ関数
public:

	/// @brief フレームワーク共通前処理
	void FrameworkPreDraw();

	/// @brief フレームワーク共通後処理
	void FrameworkPostDraw();

	/// @brief ImGuiの更新前処理
	void ImGuiPreDraw();

	/// @brief ImGuiの更新後処理
	void ImGuiPostDraw();

	/// @brief オブジェクト2D共通描画設定
	void Object2DCommonDraw();

	/// @brief パーティクル共通描画設定
	void ParticleCommonDraw();

	/// @brief オブジェクト3D共通描画設定
	void Object3DCommonDraw();

	///--------------------------------------------------------------
	///							入出力関数
public:
	/// \brief 終了リクエストの取得
	virtual bool IsEndRequest() const { return isEndRequest_; }

	///--------------------------------------------------------------
	///							メンバ変数
protected:
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
	// シーンマネージャ
	std::unique_ptr<SceneManager> sceneManager_;
};
