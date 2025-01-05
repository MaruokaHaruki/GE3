/*********************************************************************
 * \file   CameraManager.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
#include <memory>
#include <map>
#include <string>
#include "Camera.h"

///=============================================================================
///						カメラマネージャークラス
class CameraManager {
    ///--------------------------------------------------------------
	///						 メンバ関数
public:
	/// \brief シングルトンインスタンスの取得
    static CameraManager* GetInstance();

    /// \brief デストラクタ
    ~CameraManager();

	/// \brief 終了処理
    void Finalize();

	/// \brief カメラの追加
    void AddCamera(const std::string& name);

	/// \brief カメラの取得
    Camera* GetCamera(const std::string& name) const;

	/// \brief 現在のカメラの設定
    void SetCurrentCamera(const std::string& name);

	/// \brief 現在のカメラの取得
    Camera* GetCurrentCamera() const;

	/// \brief 全てのカメラの更新
    void UpdateAll();

    ///--------------------------------------------------------------
	///						 メンバ変数
private:
    //========================================
	//　コンストラクタ
    CameraManager() = default;
    //========================================
	// カメラのコンテナ
    std::map<std::string, std::unique_ptr<Camera>> cameras_;
	//現在のカメラ名
    std::string currentCameraName_;
};
