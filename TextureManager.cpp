///===================================================================///
///						テクスチャーマネージャ
///===================================================================///
#include "TextureManager.h"

///====================初期化====================///
TextureManager* TextureManager::instance_ = nullptr;

///====================インスタンス取得====================///
TextureManager* TextureManager::Getinstance() {
	if (instance_ == nullptr) {
		instance_ = new TextureManager;
	}
	return instance_;
}

///====================初期化====================///
void TextureManager::initialize(DirectXManager* dxManager) {
	///----------------SRVの数と同数----------------///
	textureDatas_.reserve(DirectXManager::kMaxSRVCount_);

	/// ===引数でdxManagerを受取=== ///
	dxManager_ = dxManager;
}

///====================テクスチャファイルの読み込み====================///
//TODO:作成中
void TextureManager::LoadTexture(const std::string& filePath) {
	///----------------読み込み済みテクスチャを検索----------------///
	//検索
	auto it = std::find_if(
		textureDatas_.begin(),
		textureDatas_.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath;
		}
	);
	///----------------早期リターン----------------///
	if (it != textureDatas_.end()) {
		return;
	}

	///----------------テクスチャファイルを読んでプログラムを扱えるようにする----------------///
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	///----------------mipmapの作成----------------///
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	///----------------テクスチャデータ追加----------------///
	//テクスチャデータ追加
	textureDatas_.resize(textureDatas_.size() + 1);
	//追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas_.back();

	///----------------テクスチャデータの書き込み----------------///
	//ファイルパス
	textureData.filePath = filePath;
	//テクスチャメタデータの取得
	textureData.metadata = mipImages.GetMetadata();
	//テクスチャリソースの作成
	textureData.resource = CreateTextureResource(textureData.metadata);
	//テクスチャデータの要素数番号をSRVのインデックスとする
	uint32_t srvIndex = static_cast<uint32_t>( textureDatas_.size() );
	//各ハンドルを取得
	textureData.srvHandleCPU = dxManager_->GetSRVCPUDescriptorHandle(srvIndex); //CPU
	textureData.srvHandleGPU = dxManager_->GetSRVGPUDescriptorHandle(srvIndex); //GPU

	///----------------SRVの生成----------------///
	//metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	//SRVの生成
	dxManager_->GetDevice().Get()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);
}

///====================テクスチャリソース生成====================///
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata) {
	/// ===1.metadataを元にResouceの設定=== ///
	D3D12_RESOURCE_DESC resouceDesc{};
	resouceDesc.Width = UINT(metadata.width);								//Textureの幅
	resouceDesc.Height = UINT(metadata.height);								//Textureの高さ
	resouceDesc.MipLevels = UINT16(metadata.mipLevels);						//mipmapの数
	resouceDesc.DepthOrArraySize = UINT16(metadata.arraySize);				//奥行き or 配列Textureの配列数
	resouceDesc.Format = metadata.format;									//TextureのFormat
	resouceDesc.SampleDesc.Count = 1;										//サンプリングカウント
	resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	//Textureの次元数。普段つかているのは2次元。

	/// ===2.利用するHeapの設定===///
	//TODO:リソースの場所を変更する03_00_ex
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//プロセッサの近くに配置

	/// ===3.resouceを生成する=== ///
	Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr = dxManager_->GetDevice()->CreateCommittedResource(
		&heapProperties,					//Heapの設定
		D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定、特になし
		&resouceDesc,						//Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	//初回のResouceState。Textureは基本読むだけ
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
	return resource;
}


///====================終了処理====================///
void TextureManager::Finalize() {
	delete instance_;
	instance_ = nullptr;
}
