/*********************************************************************
 * \file   SpriteSetup.h
 * \brief  スプライト管理クラス
 * 
 * \author Harukichimaru
 * \date   November 2024
 * \note   スプライト共通部
 *********************************************************************/
#pragma once
#include "DirectXCore.h"

///=============================================================================
///						クラス
class SpriteSetup {
public:
	///--------------------------------------------------------------
	///						 メンバ関数

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
	///						 入出力関数
public:
	/**----------------------------------------------------------------------------
	 * \brief  GetDXManager ダイレクトXマネージャーの取得
	 * \return 
	 * \note   
	 */
	DirectXCore* GetDXManager() const { return dxCore_; }


	///--------------------------------------------------------------
	///						 メンバ関数
private:
	

	///====================DirectXCoreポインタ====================///
	DirectXCore* dxCore_ = nullptr;

	///====================RootSignature====================///
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	///====================グラフィックスパイプライン====================///
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
};

