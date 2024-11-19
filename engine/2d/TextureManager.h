/*********************************************************************
 * \file   TextureManager.h
 * \brief  テクスチャ管理用クラス
 *
 * \author Harukichimaru
 * \date   October 2024
 *********************************************************************/
#pragma once
#include <string>
#include "DirectXCore.h"

 ///--------------------------------------------------------------
 ///							構造体
 /**
  * \brief テクスチャデータのセット
  * \brief filePath ファイルパス
  * \brief metadata メタデータ
  * \brief resource リソース
  * \brief srvHandleCPU CPU用SRVハンドル
  * \brief srvHandleGPU GPU用SRVハンドル
  */
struct TextureData {
	std::string filePath;
	DirectX::TexMetadata metadata;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr <ID3D12Resource> interMediateResource;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
};

///=============================================================================
///								クラス
class TextureManager {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	
	/**----------------------------------------------------------------------------
	 * \brief Getinstance 
	 * \return インスタンス
	 */
	static TextureManager* GetInstance();

	/**----------------------------------------------------------------------------
	* \brief 初期化
	* \param dxManager ダイレクトXマネージャーのポインタ
	* \note  生ポインタの受け渡しを行うこと
	*/
	void Initialize(DirectXCore* dxManager);

	/**----------------------------------------------------------------------------
	 * \brief ファイルの読み込み
	 * \param filePath ファイルパス
	 */
	void LoadTexture(const std::string& filePath);

	/**----------------------------------------------------------------------------
	 * \brief  テクスチャデータの作成
	 * \param  metadata メタデータの受け渡し
	 * \return テクスチャデータ
	 */
	//Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	/**----------------------------------------------------------------------------
	 * \brief	終了処理
	 * \details 必ずダイレクトX初期化より前に行うこと
	 */
	void Finalize();

	/**----------------------------------------------------------------------------
	 * \brief  SRVテクスチャインデックスの開始番号の取得
	 * \param  filePath ファイルパス
	 * \return SRVテクスチャインデックスの開始番号
	 * \note   検索化ヒットしない場合は停止するぞ
	 */
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);	
	
	/**----------------------------------------------------------------------------
	 * \brief  GPUハンドルの取得
	 * \param  textureIndex
	 * \return GPUハンドル
	 * \note   高速化には必要ダヨ
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

	/**----------------------------------------------------------------------------
	 * \brief  GetMetadata メタデータの取得
	 * \param  textureIndex テクスチャインデックス
	 * \return 
	 * \note   
	 */
	const DirectX::TexMetadata& GetMetadata(uint32_t textureIndex);



	///--------------------------------------------------------------
	///							 メンバ変数
private:

	//---------------------------------------
	// シングルトンインスタンス
	static TextureManager* instance_;

	//---------------------------------------
	// 設定
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = default;
	TextureManager& operator = (TextureManager&) = default;

	//---------------------------------------
	// DirectXCoreポインタ
	DirectXCore* dxCore_ = nullptr;;

	//---------------------------------------
	// テクスチャデータ
	std::vector<TextureData> textureDatas_;

	//---------------------------------------
	// SRVインデックスの開始番号
	//NOTE:ImGuiが使っている番号を開けてその後ろのSRVヒープ1番から使用する
	const uint32_t kSRVIndexTop = 1;
};

