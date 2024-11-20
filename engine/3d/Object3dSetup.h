/*********************************************************************
 * \file   Object3dSetup.h
 * \brief
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#pragma once
#include "DirectXCore.h"


 ///=============================================================================
 ///						クラス
class Object3dSetup {
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
	///						 ローカルメンバ関数
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
};