/*********************************************************************
 * \file   Line.h
 * \brief
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
#include <vector>
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "Transform.h"
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

struct LineVertex {
	Vector3 position;
	Vector4 color;
};

class Camera;
class LineSetup;
class Line {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(LineSetup *lineSetup);

	/// \brief 更新
	void Update();

	/// \brief 描画 
	void Draw();

	/**----------------------------------------------------------------------------
	 * \brief  ClearLines 
	 */
	void ClearLines();

	/**----------------------------------------------------------------------------
	 * \brief  DrawLine ライン描画
	 * \param  start 始点
	 * \param  end 終点
	 * \param  color 色
	 */
	void DrawLine(const Vector3 &start, const Vector3 &end, const Vector4 &color);

	///--------------------------------------------------------------
	///						 静的メンバ関数
private:
	/**----------------------------------------------------------------------------
	 * \brief  CreateVertexBuffer 頂点バッファの作成
	 */
	void CreateVertexBuffer();

	/**----------------------------------------------------------------------------
	 * \brief  CreateTransformationMatrixBuffer 
	 * \return 
	 */
	void CreateTransformationMatrixBuffer();

	///--------------------------------------------------------------
	///							入出力関数
public:

	

	/**----------------------------------------------------------------------------
	* \brief  SetTransform トランスフォーメーションの設定
	* \param  transform トランスフォーメーション
	* \note
	*/
	void SetTransform(const Transform& transform) { transform_ = transform; }

	/**----------------------------------------------------------------------------
	 * \brief  GetTransform 
	 * \return 
	 */
	Transform GetTransform() const { return transform_; }

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

	///--------------------------------------------------------------
	///							メンバ変数
private:

	//---------------------------------------
	// オブジェクト3Dセットアップポインタ
	LineSetup* lineSetup_ = nullptr;

	//---------------------------------------
	// 頂点データ
	std::vector<LineVertex> vertices_;

	//---------------------------------------
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	// バッファリソースの使い道を指すポインタ
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};

	//---------------------------------------
	//トランスフォーメーションマトリックス
	Microsoft::WRL::ComPtr <ID3D12Resource> transfomationMatrixBuffer_;

	//---------------------------------------
	// バッファリソース内のデータを指すポインタ
	//トランスフォーメーションマトリックス
	TransformationMatrix* transformationMatrixData_ = nullptr;

	//--------------------------------------
	// Transform
	Transform transform_ = {};

	//--------------------------------------
	// カメラ
	Camera* camera_ = nullptr;
};

