///===================================================================///
///						テクスチャマネージャー.h
///===================================================================///
#include <string>
#include "base/DirectXManager.h"


///----------------------------------------------------///
///						構造体
///----------------------------------------------------///
/// <summary>
/// テクスチャ1枚分のデータ
/// </summary>
struct TextureData {
	std::string filePath;
	DirectX::TexMetadata metadata;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
};

#pragma once
class TextureManager {
	///----------------------------------------------------///
	///						メンバ関数　 
	///----------------------------------------------------///
public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static TextureManager* Getinstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void initialize(DirectXManager* dxManager);

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath"></param>
	void LoadTexture(const std::string& filePath);

	/// <summary>
	/// テクスチャリソースの生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);


	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	///----------------------------------------------------///
	///						メンバ変数
	///----------------------------------------------------///
private:
	///====================シングルトン化====================///
	static TextureManager* instance_;
	///----------------設定----------------///
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = default;
	TextureManager& operator = (TextureManager&) = default;

	///====================DirectXManagerポインタ====================///
	DirectXManager* dxManager_ = nullptr;;

	///====================テクスチャデータ====================///
	std::vector<TextureData> textureDatas_;

	///====================SRVインデックスの開始番号====================///
	//NOTE:ImGuiが使っている番号を開けてその後ろのSRVヒープ1番から使用する
	const uint32_t kSRVIndexTop = 1;
};

