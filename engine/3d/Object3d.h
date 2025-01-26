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

struct CameraForGpu {
	Vector3 worldPosition;
};

class Object3dSetup;
class Camera;
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

	/// \brief ImGui描画
	void ChangeTexture(const std::string &texturePath);

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

	/**----------------------------------------------------------------------------
	 * \brief  カメラバッファの作成
	 * \note
	 */
	void CreateCameraBuffer();

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
	 * \brief  SetTransform トランスフォーメーションの設定
	 * \param  transform トランスフォーメーション
	 * \note
	 */
	void SetTransform(const Transform& transform) { transform_ = transform; }

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
	void SetRotation(const Vector3& rotate) { transform_.rotate = rotate; }
	/**----------------------------------------------------------------------------
	 * \brief  GetRotate 回転の取得
	 * \return Vector3 回転
	 * \note
	 */
	const Vector3& GetRotation() const { return transform_.rotate; }

	/**----------------------------------------------------------------------------
	 * \brief  SetTranslate 移動の設定
	 * \param  translate 移動
	 * \note
	 */
	void SetPosition(const Vector3& translate) { transform_.translate = translate; } 
	/**----------------------------------------------------------------------------
	 * \brief  GetTranslate 移動の取得
	 * \return Vector3 移動
	 * \note
	 */
	const Vector3& GetPosition() const { return transform_.translate; }

	/**----------------------------------------------------------------------------
	 * \brief  SetCamera カメラの設定
	 * \param  camera
	 */
	void SetCamera(Camera* camera) { this->camera_ = camera; }

	/**----------------------------------------------------------------------------
	 * \brief  SetDirectionalLight 並行光源の設定
	 * \param  color
	 * \param  direction
	 * \param  intensity
	 */
	void SetDirectionalLight(const Vector4 &color, const Vector3 &direction, float intensity) {
		directionalLightData_->color = color;
		directionalLightData_->direction = direction;
		directionalLightData_->intensity = intensity;
	}

	/**----------------------------------------------------------------------------
	 * \brief  GetDirectionalLight 並行光源の取得
	 * \return 
	 */
	const DirectionalLight &GetDirectionalLight() const { return *directionalLightData_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetMaterialColor マテリアルカラーの設定
	 * \param  color
	 */
	void SetMaterialColor(const Vector4 &color) { model_->SetMaterialColor(color); }

	/**----------------------------------------------------------------------------
	 * \brief  GetMaterialColor マテリアルカラーの取得
	 * \return
	 */
	Vector4 GetMaterialColor() const { return model_->GetMaterialColor(); }

	/**----------------------------------------------------------------------------
	 * \brief  SetShininess 光沢度の設定
	 * \param  shininess
	 */
	void SetShininess(float shininess) { model_->SetShininess(shininess); }

	/**----------------------------------------------------------------------------
	 * \brief  GetShininess 光沢度の取得
	 * \return
	 */
	float GetShininess() const { return model_->GetShininess(); }

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
	//カメラ
	Microsoft::WRL::ComPtr <ID3D12Resource> cameraBuffer_;


	//---------------------------------------
	// バッファリソース内のデータを指すポインタ
	//トランスフォーメーションマトリックス
	TransformationMatrix* transformationMatrixData_ = nullptr;
	//並行光源
	DirectionalLight* directionalLightData_ = nullptr;
	//カメラ
	CameraForGpu *cameraData_ = nullptr;

	//--------------------------------------
	// Transform
	Transform transform_ = {};

	//--------------------------------------
	// カメラ
	Camera* camera_ = nullptr;
};

