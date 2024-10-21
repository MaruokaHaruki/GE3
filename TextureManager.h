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
	void initialize();

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

	///====================テクスチャデータ====================///
	std::vector<TextureData> textureDatas_;



};

