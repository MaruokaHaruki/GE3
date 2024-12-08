/*********************************************************************
 * \file   ParticleSetup.h
 * \brief 
 * 
 * \author Harukichimaru
 * \date   December 2024
 * \note   
 *********************************************************************/
#pragma once
#include "DirectXCore.h"
#include "Camera.h"

class ParticleSetup {
	///--------------------------------------------------------------
	///						 メンバ関数
public:
	/**----------------------------------------------------------------------------
	* \brief  Initialize 初期化
	* \param  dxManager ダイレクトXマネージャー
	* \note
	*/
	void Initialize(DirectXCore* dxCore);

	/**----------------------------------------------------------------------------
	* \brief  CommonDrawSetup 共通描画設定
	* \note
	*/
	void CommonDrawSetup();


	///--------------------------------------------------------------
	///						 静的メンバ関数
private:

	/**----------------------------------------------------------------------------
	* \brief  CreateRootSignature ルートシグネチャーの作成
	* \note
	*/
	void CreateRootSignature();

	/**----------------------------------------------------------------------------
	* \brief  CreateGraphicsPipeline グラフィックスパイプラインの作成
	* \note
	*/
	void CreateGraphicsPipeline();

	///--------------------------------------------------------------
	///							入出力関数
public:
	/**----------------------------------------------------------------------------
	* \brief  GetDXManager DirectXCore取得
	* \return
	* \note
	*/
	DirectXCore* GetDXManager() const { return dxCore_; }

	/**----------------------------------------------------------------------------
	* \brief  SetCamera デフォルトカメラの設定
	* \param  camera
	*/
	void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
	/*
	* \brief  GetCamera デフォルトカメラの取得
	* \return
	*/
	Camera* GetDefaultCamera() { return defaultCamera_; }


	///--------------------------------------------------------------
	///							メンバ変数
private:
	//========================================
	// DirectXCoreポインタ
	DirectXCore* dxCore_ = nullptr;

	//========================================
	// RootSignature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	//========================================
	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	//========================================
	// デフォルトカメラ
	Camera* defaultCamera_ = nullptr;
};

