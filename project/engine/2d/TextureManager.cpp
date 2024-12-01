/*********************************************************************
 * \file   TextureManager.cpp
 * \brief  テクスチャ管理
 *
 * \author Harukichimaru
 * \date   October 2024
 * \note
 *********************************************************************/
#include "TextureManager.h"

 ///=============================================================================
 ///						インスタンス設定
TextureManager* TextureManager::instance_ = nullptr;

///=============================================================================
///							インスタンス生成
TextureManager* TextureManager::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new TextureManager;
	}
	return instance_;
}

///=============================================================================
///								初期化
void TextureManager::Initialize(DirectXCore* dxCore) {
	//---------------------------------------
	// SRVの数と同期
	textureDatas_.reserve(DirectXCore::kMaxSRVCount_);
	//---------------------------------------
	// 引数でdxManagerを受取
	dxCore_ = dxCore;
}

///=============================================================================
///						テクスチャファイルの読み込み
//TODO:作成中
void TextureManager::LoadTexture(const std::string& filePath) {
	//---------------------------------------
	// 読み込み済みテクスチャを検索
	auto it = std::find_if(
		textureDatas_.begin(),
		textureDatas_.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath;
		}
	);
	// 早期リターン
	if (it != textureDatas_.end()) {
		return;
	}

	//---------------------------------------
	// テクスチャファイルを読んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//---------------------------------------
	// mipmapの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//---------------------------------------
	// テクスチャデータ追加
	textureDatas_.resize(textureDatas_.size() + 1);
	//追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas_.back();

	//---------------------------------------
	// テクスチャデータの書き込み
	//ファイルパス
	textureData.filePath = filePath;
	//テクスチャメタデータの取得
	textureData.metadata = mipImages.GetMetadata();
	//テクスチャリソースの作成
	textureData.resource = dxCore_->CreateTextureResource(textureData.metadata);
	//テクスチャデータの要素数番号をSRVのインデックスとする
	uint32_t srvIndex = static_cast<uint32_t>( textureDatas_.size() - 1 + kSRVIndexTop );
	//中間リソース
	textureData.interMediateResource = dxCore_->UploadTextureData(textureData.resource, mipImages);
	//各ハンドルを取得
	textureData.srvHandleCPU = dxCore_->GetSRVCPUDescriptorHandle(srvIndex); //CPU
	textureData.srvHandleGPU = dxCore_->GetSRVGPUDescriptorHandle(srvIndex); //GPU

	//---------------------------------------
	// SRVの生成
	//metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	//SRVの生成
	dxCore_->GetDevice().Get()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);
}

///=============================================================================
///								終了処理
void TextureManager::Finalize() {
	//インスタンスの削除
	delete instance_;
	instance_ = nullptr;
}

///=============================================================================
///					SRVテクスチャインデックスの開始番号
uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath) {
	//---------------------------------------
	// 読み込み済みテクスチャデータを検索
	auto it = std::find_if(
		textureDatas_.begin(),
		textureDatas_.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	//---------------------------------------
	// データの判別
	if (it != textureDatas_.end()) {
		//読み込み済みなら要素番号を返す
		uint32_t textureIndex = static_cast<uint32_t>( std::distance(textureDatas_.begin(), it) );
		return textureIndex;

	}
	//---------------------------------------
	// 検索化ヒットしない場合は停止
	assert(0);
	return 0;
}

///=============================================================================
///						GPUハンドルの取得
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex) {
	// 範囲外指定チェック
	assert(textureIndex < textureDatas_.size());
	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas_[textureIndex];
	return textureData.srvHandleGPU;
}

///=============================================================================
///						メタデータの取得
const DirectX::TexMetadata& TextureManager::GetMetadata(uint32_t textureIndex) {
	// 範囲外指定チェック
	assert(textureIndex < textureDatas_.size());
	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas_[textureIndex];
	return textureData.metadata;
}
