/*********************************************************************
 * \file   TextureManager.h
 * \brief  テクスチャ管理用クラス
 *
 * \author Harukichimaru
 * \date   October 2024
 *********************************************************************/
#pragma once
#include <string>
#include "DirectXManager.h"

 ///--------------------------------------------------------------
 ///							構造体
 /**
  * \brief テクスチャデータ
  */
struct TextureData {
	std::string filePath;
	DirectX::TexMetadata metadata;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
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
	static TextureManager* Getinstance();

	/**----------------------------------------------------------------------------
	* \brief 初期化
	* \param dxManager ダイレクトXマネージャーのポインタ
	* \note  生ポインタの受け渡しを行うこと
	*/
	void Initialize(DirectXManager* dxManager);

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
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

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





	///--------------------------------------------------------------
	///							 メンバ変数
private:

	///---------------------------------------
	/// シングルトン化
	static TextureManager* instance_;	/* シングルトンインスタンス */

	///---------------------------------------
	/// 設定
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = default;
	TextureManager& operator = (TextureManager&) = default;

	///---------------------------------------
	/// DirectXManagerポインタ
	DirectXManager* dxManager_ = nullptr;;

	///---------------------------------------
	/// テクスチャデータ
	std::vector<TextureData> textureDatas_;

	///---------------------------------------
	/// SRVインデックスの開始番号
	//NOTE:ImGuiが使っている番号を開けてその後ろのSRVヒープ1番から使用する
	const uint32_t kSRVIndexTop = 1;
};

