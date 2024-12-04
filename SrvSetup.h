/*********************************************************************
 * \file   SrvSetup.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   December 2024
 * \note   
 *********************************************************************/
#pragma once
#include "DirectXCore.h"

class SrvSetup {

	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(DirectXCore* dxCore);

	/**----------------------------------------------------------------------------
	 * \brief  PreDraw ループ前処理
	 */
	void PreDraw();
	
	/**----------------------------------------------------------------------------
	 * \brief  Allocate メモリ確保
	 * \return 
	 */
	uint32_t Allocate();

	/**----------------------------------------------------------------------------
	 * \brief  CreateSRVforTexture2D SRV生成(テクスチャ用)
	 * \param  srvIndex インデックス
	 * \param  pResource リソース
	 * \param  format フォーマット
	 * \param  mipLevels ミップレベル
	 */
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT mipLevels);

	/**----------------------------------------------------------------------------
	 * \brief  CreateSRVStructuredBuffer SRV生成(構造化バッファ用)
	 * \param  srvIndex インデックス
	 * \param  pResource リソース
	 * \param  format フォーマット
	 * \param  structureByteStride 構造体のバイトサイズ
	 */
	void CreateSRVStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT structureByteStride);


	///--------------------------------------------------------------
	///							静的メンバ関数
private:

	///--------------------------------------------------------------
	///							入出力関数
public:
	/**----------------------------------------------------------------------------
	* \brief  GetSRVCPUDescriptorHandle SRVの指定番号のCPUディスクリプタ‐ハンドルを取得
	* \param  index インデックス
	* \return
	* \note
	*/
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	/*
	* \brief  GetSRVGPUDescriptorHandle SRVの指定番号のGPUディスクリプタ‐ハンドルを取得
	* \param  index インデックス
	* \return
	* \note
	*/
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	/**----------------------------------------------------------------------------
	 * \brief  SetGraphicsRootDescriptorTable グラフィックスルートディスクリプタテーブルの設定
	 * \param  rootParameterIndex 
	 * \param  srvIndex
	 */
	void SetGraphicsRootDescriptorTable(uint32_t rootParameterIndex, uint32_t srvIndex);

	//========================================
	// 最大SRV数
	static const uint32_t kMaxSRVCount_ = 512;

	///--------------------------------------------------------------
	///							メンバ変数
private:
	//========================================
	// DirectXCoreポインタ
	DirectXCore* dxCore_ = nullptr;

	//========================================
	//SRV用ディスクリプタサイズ
	uint32_t descriptorSizeSRV_ = 0;
	//SRVディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

	//========================================
	// 次に使用するディスクリプタのインデックス
	uint32_t useIndex_ = 0;


};

