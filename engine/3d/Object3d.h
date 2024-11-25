/*********************************************************************
 * \file   Object3d.h
 * \brief
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#pragma once
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "Transform.h"
#include "Model.h"
#include "ModelManager.h"
 //========================================
 // DX12include
#include<d3d12.h>
#include<dxgi1_6.h>
#include <wrl/client.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
//========================================
// DXC
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

class Object3dSetup;
class Object3d {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(Object3dSetup* object3dSetup);

	/// \brief 更新
	void Update();

	/// \brief 描画 
	void Draw();

	///--------------------------------------------------------------
	///						 静的メンバ関数
private:
	/**----------------------------------------------------------------------------
	 * \brief  トランスフォーメーションマトリックスバッファの作成
	 * \note
	 */
	void CreateTransformationMatrixBuffer();

	/**----------------------------------------------------------------------------
	 * \brief  並行光源の作成
	 * \note
	 */
	void CreateDirectionalLight();

	///--------------------------------------------------------------
	///							入出力関数
public:

	/**----------------------------------------------------------------------------
	* \brief  SetModel モデルの設定
	* \param  filePath ファイルパス
	* \note
	*/
	void SetModel(const std::string& filePath) {model_ = ModelManager::GetInstance()->FindModel(filePath);}

	/**----------------------------------------------------------------------------
	 * \brief  SetModel モデルの設定
	 * \param  model モデル
	 * \note
	 */
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	/**----------------------------------------------------------------------------
	 * \brief  GetScale スケールの取得
	 * \return Vector3 スケール
	 * \note
	 */
	const Vector3& GetScale() const { return transform_.scale; }

	/**----------------------------------------------------------------------------
	 * \brief  SetRotate 回転の設定
	 * \param  rotate 回転
	 * \note
	 */
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	/**----------------------------------------------------------------------------
	 * \brief  GetRotate 回転の取得
	 * \return Vector3 回転
	 * \note
	 */
	const Vector3& GetRotate() const { return transform_.rotate; }

	/**----------------------------------------------------------------------------
	 * \brief  SetTranslate 移動の設定
	 * \param  translate 移動
	 * \note
	 */
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; } 
	/**----------------------------------------------------------------------------
	 * \brief  GetTranslate 移動の取得
	 * \return Vector3 移動
	 * \note
	 */
	const Vector3& GetTranslate() const { return transform_.translate; }

	///--------------------------------------------------------------
	///							メンバ変数
private:

	//---------------------------------------
	// オブジェクト3Dセットアップポインタ
	Object3dSetup* object3dSetup_ = nullptr;

	//---------------------------------------
	// モデルデータ
	Model* model_ = nullptr;

	//---------------------------------------
	//トランスフォーメーションマトリックス
	Microsoft::WRL::ComPtr <ID3D12Resource> transfomationMatrixBuffer_;
	//並行光源
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightBuffer_;

	//---------------------------------------
	// バッファリソース内のデータを指すポインタ
	//トランスフォーメーションマトリックス
	TransformationMatrix* transformationMatrixData_ = nullptr;
	//並行光源
	DirectionalLight* directionalLightData_ = nullptr;

	//--------------------------------------
	// Transform
	Transform transform_;
	// カメラのTransform
	Transform cameraTransform_;
};

