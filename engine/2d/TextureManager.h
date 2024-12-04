/*********************************************************************
 * \file   TextureManager.h
 * \brief  テクスチャ管理用クラス
 *
 * \author Harukichimaru
 * \date   October 2024
 *********************************************************************/
#pragma once
#include <unordered_map>
#include <string>
#include "DirectXCore.h"
#include "SrvSetup.h"

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
	DirectX::TexMetadata metadata{};
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr <ID3D12Resource> interMediateResource;
	uint32_t srvIndex = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};
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
	void Initialize(DirectXCore* dxManager, const std::string& textureDirectoryPath, SrvSetup* srvSetup);

	/**----------------------------------------------------------------------------
  * \brief ファイルの読み込み
  * \param filePath ファイルパス
  */
	void LoadTexture(const std::string& filePath);

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
	uint32_t GetTextureIndex(const std::string& filePath);		
	
	/**----------------------------------------------------------------------------
  * \brief  GPUハンドルの取得
  * \param  textureIndex
  * \return GPUハンドル
  * \note   高速化には必要ダヨ
  */
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

	/**----------------------------------------------------------------------------
  * \brief  GetMetadata メタデータの取得
  * \param  textureIndex テクスチャインデックス
  * \return 
  * \note   
  */
	const DirectX::TexMetadata& GetMetadata(const std::string& filePath);



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
	std::unordered_map<std::string, TextureData> textureDatas_;

	//---------------------------------------
	// SRVインデックスの開始番号
	//NOTE:ImGuiが使っている番号を開けてその後ろのSRVヒープ1番から使用する
	const uint32_t kSRVIndexTop = 1;
	//SrvSetupポインタ
	SrvSetup* srvSetup_ = nullptr;

	//---------------------------------------
	// ディレクトリパス
	std::string kTextureDirectoryPath = "resources/texture";
};
