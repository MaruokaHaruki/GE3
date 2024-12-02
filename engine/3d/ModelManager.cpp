/*********************************************************************
 * \file   ModelManager.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#include "ModelManager.h"

///=============================================================================
///						インスタンス設定
ModelManager* ModelManager::instance_ = nullptr;

///=============================================================================
///						インスタンス生成
ModelManager* ModelManager::GetInstance() {
	if(instance_ == nullptr) {
		instance_ = new ModelManager();
	}
	return instance_;

}

///=============================================================================
///						初期化
void ModelManager::Initialize(DirectXCore* dxCore) {
	//========================================
	// モデル共通部の初期化
	modelSetup_ = std::make_unique<ModelSetup>();
	modelSetup_->Initialize(dxCore);
}

///=============================================================================
///						モデルの読み込み
void ModelManager::LoadMedel(const std::string& filePath) {
	//========================================
	// 読み込み済みモデルを検索
	if(models_.contains(filePath)) {
		//早期リターン！
		return;
	}

	//========================================
	// モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelSetup_.get(), "resources/model", filePath);

	//========================================
	// モデルを登録
	models_.insert(std::make_pair(filePath, std::move(model)));
}

///=============================================================================
///						モデルデータの検索
Model* ModelManager::FindModel(const std::string& filePath) {
	//========================================
	// モデルの検索
	auto it = models_.find(filePath);
	if(it != models_.end()) {
		return it->second.get();
	}
	//========================================
	// 検索ヒットしない場合はnullptrを返す
	return nullptr;
}

///=============================================================================
///						終了処理
void ModelManager::Finalize() {
	//インスタンスの削除
	delete instance_;
	instance_ = nullptr;
}
