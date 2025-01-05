/*********************************************************************
 * \file   CameraManager.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "CameraManager.h"

///=============================================================================
///						シングルトンインスタンスの取得
CameraManager* CameraManager::GetInstance() {
    static CameraManager instance;
    return &instance;
}

///=============================================================================
///						終了処理
void CameraManager::Finalize() {
    cameras_.clear();
}

///=============================================================================
///                     カメラの追加
void CameraManager::AddCamera(const std::string& name) {
	//カメラを作成
	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->Initialize();

	//カメラを登録
    cameras_[name] = std::move(camera);
}

///=============================================================================
///						カメラの取得
Camera* CameraManager::GetCamera(const std::string& name) const {
    auto it = cameras_.find(name);
    if (it != cameras_.end()) {
        return it->second.get();
    }
    return nullptr;
}

///=============================================================================
///						使用カメラの設定
void CameraManager::SetCurrentCamera(const std::string& name) {
    if (cameras_.find(name) != cameras_.end()) {
        currentCameraName_ = name;
    }
}

///=============================================================================
///                     現在のカメラの取得
Camera* CameraManager::GetCurrentCamera() const {
    return GetCamera(currentCameraName_);
}

///=============================================================================
///                     カメラの更新
void CameraManager::UpdateAll() {
    for (auto& pair : cameras_) {
        if (pair.second) {
            pair.second->Update();
        }
    }
}

///=============================================================================
///                     デストラクタ
CameraManager::~CameraManager() {
    cameras_.clear();
}
