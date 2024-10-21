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
void TextureManager::initialize() {
	///----------------SRVの数と同数----------------///



}

///====================終了処理====================///
void TextureManager::Finalize() {
	delete instance_;
	instance_ = nullptr;
}
