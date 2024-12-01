/*********************************************************************
 * \file   ModelManager.h
 * \brief
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#pragma once
//========================================
// 自作ヘッダファイル
#include "Model.h"
#include "ModelSetup.h"
//========================================
// 標準ライブラリ
#include <memory>
#include <map>
#include <string>

class ModelManager {
	///--------------------------------------------------------------
	///						 シングルトン化
private:
	static ModelManager* instance_;
	//コンストラクタ
	ModelManager() = default;
	//デストラクタ
	~ModelManager() = default;
	//コピーコンストラクタ
	ModelManager(const ModelManager&) = delete;
	//代入演算子
	ModelManager& operator=(const ModelManager&) = delete;

	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(DirectXCore* dxCore);

	/// \brief 更新
	void Update();

	/// \brief 描画 
	void Draw();

	/**----------------------------------------------------------------------------
	 * \brief  LoadMedel モデルの読み込み
	 * \param  filePath ファイルパス
	 * \note   
	 */
	void LoadMedel(const std::string& filePath);

	/**----------------------------------------------------------------------------
	 * \brief  FindModel モデルデータの検索
	 * \param  filePath ファイルパス
	 * \return Model* モデルデータ
	 * \note   
	 */
	Model* FindModel(const std::string& filePath);

	/**----------------------------------------------------------------------------
	 * \brief  Finalize 終了処理
	 * \note   
	 */
	void Finalize();
	
	///--------------------------------------------------------------
	///							静的メンバ関数
private:

	///--------------------------------------------------------------
	///							入出力関数
public:
	/**----------------------------------------------------------------------------
	 * \brief  GetInstance インスタンス取得
	 * \return ModelManager* インスタンス
	* \note
	*/
	static ModelManager* GetInstance();


	///--------------------------------------------------------------
	///							メンバ変数
private:
	//========================================
	// モデル共通部
	std::unique_ptr<ModelSetup> modelSetup_ = nullptr;

	//========================================
	// モデルデータコンテナ
	// NOTE:vectorだと検索が遅いのでmapを使う
	std::map<std::string, std::unique_ptr<Model>> models_;

};

